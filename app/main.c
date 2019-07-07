#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "stdafx.h"
#include "ArenaCApi.h"
#include <inttypes.h> // defines macros for printf functions
#include <stdbool.h>  // defines boolean type and values
#include <string.h>       // defines strcmp functions

#define TAB1 "  "
#define TAB2 "    "

#define LOG 			       printf

#define TEST_SIZE		       10

#define FRAME_WIDTH		       256
#define FRAME_HEIGHT 		       256
#define FRAME_SIZE		       FRAME_HEIGHT * FRAME_WIDTH

#define DEVICE_TYPE_MEMORY              0
#define DEVICE_TYPE_VGA                 1
#define DEVICE_TYPE_FILTER              2

#define STATUS_OK			0
#define STATUS_OPENING_DEV_MEM_FAILED	1
#define STATUS_MAPPING_FAILED 		2
#define STATUS_UNMAPPING_FAILED		3
#define STATUS_CLOSING_DEV_MEM_FAILED	4
#define STATUS_COPYING_DATA_FAILED	5

#define IMAGE			        0
#define BRAM_SIZE		        sizeof(uint8_t) * 1024 * 256       //256 KB BRAM block
#define DEVICE_SIZE		        sizeof(uint8_t) * 1024 * 64        //64 KB BRAM block
#define NUM_IMAGES                      500
#define FRAME_HEIGHT                    256
#define FRAME_WIDTH                     256
#define FILE_NAME_PATTERN               "Images/<format>.jpg"
#define FORMAT_MONO                     "Mono8"
#define FORMAT_ANGLES                   "PolarizedAngles_0d_45d_90d_135d_Mono8"
#define FORMAT_AOLP                     "PolarizedAolp_Mono8"
#define FORMAT_DOLP                     "PolarizedDolp_Mono8"
#define PACKET_SIZE 		        1500
#define IMAGE_TIMEOUT 	                2000
#define SYSTEM_TIMEOUT 	                100
#define MAX_BUF 256


static volatile uint32_t *frame_filter,
                  *frame_display,
                  *frame_memory;

off_t offset[]={        0x40000000u,    // bram
                        0x43C10000u,    // vga
                        0x43C00000u};   // filter
const off_t size[]={    BRAM_SIZE,
                        DEVICE_SIZE,
                        DEVICE_SIZE};
union convert {
    uint32_t word;
    uint16_t halfword[2];
    uint8_t bytes[4];
} convert;

static const char * status_codes[] = {
        "OK",
        "Failed to open /dev/mem",
        "Failed to map",
        "Failed to unmap",
        "Failed to close /dev/mem",
        "Failed to copy data",
};

static const char * soc_mem[] = {
        "/dev/mem",              // 0 = block ram
        "/dev/uio0",             // 1 = vga_controller
        "/dev/uio1",             // 2 = image_filter
};

const char* get_status(int status)
{
	return status_codes[status];
}

int err(int status)
{
  LOG(get_status(status));

  return(status);
}

int map_device(volatile void **ptr, size_t size, int port, off_t offset, int device_type)
{
	int fd;

	fd = open(soc_mem[device_type], O_RDWR);
	if(fd < 1)
	{
   		return err(STATUS_OPENING_DEV_MEM_FAILED);
   	}
	LOG("\tMapping device \n");
        if(device_type == DEVICE_TYPE_MEMORY)
        {
	        *ptr = (volatile void *) mmap(NULL, size, port, MAP_SHARED, fd, offset);
        }
        else
        {
                *ptr = (volatile void *) mmap(NULL, size, port, MAP_SHARED, fd, 0);
        }
	if(*ptr == MAP_FAILED)
	{
		return err(STATUS_MAPPING_FAILED);
	}
	LOG("\tDevice mapped to user space, 0x%lx\n", (uint32_t)ptr);
	if(!close(fd))
	{//we can close the file descriptor as mmap keeps it automatically open until we munmap
	   return STATUS_OK;
	}
	else
	{
                LOG("\n mmap error");
	   return err(STATUS_CLOSING_DEV_MEM_FAILED);
        }
}

int unmap(volatile void **ptr, size_t size)
{
 	LOG("\tUnmapping device \n");
 	if(!(munmap(*((void **)ptr), size) == 0))
 	{
                return err(STATUS_UNMAPPING_FAILED);
 	}
 	*ptr = NULL;
 	LOG("\tDevice unmapped \n");

 	return STATUS_OK;
}

int bind_device(volatile uint32_t **ptr, int device_type)
{
        LOG("Binding device with physical address = 0x%lx\n",  offset[device_type]);
        return map_device((volatile void **)ptr, size[device_type], PROT_READ|PROT_WRITE, offset[device_type], device_type);
}

int unbind_device(volatile uint32_t **ptr, int device_type)
{
        LOG("Unbinding device with physical address = 0x%lx\n", offset[device_type]);
        return unmap((volatile void **) ptr, size[device_type]);
}

void soft_device(volatile uint32_t *input, volatile uint32_t* kernel, volatile uint32_t* output)
{
  int idx;

  for(idx = 0; idx < FRAME_SIZE; idx++)
  {
    // Split in bytes and multiply
    *(output + idx) =(((*(input + idx)        & 0xFF) *  (*(kernel + idx)        & 0xFF))           |
                    ((((*(input + idx) >> 8 ) & 0xFF) * ((*(kernel + idx) >> 8 ) & 0xFF )) << 8 )   |
                    ((((*(input + idx) >> 16) & 0xFF) * ((*(kernel + idx) >> 16) & 0xFF )) << 16)   |
                    ((((*(input + idx) >> 24) & 0xFF) * ((*(kernel + idx) >> 24) & 0xFF )) << 24)   );
  }
}

// This function eables the device once
void start_device(volatile uint32_t*  device)
{
	// Start the device
	*((uint32_t *)(device)) = (*((uint32_t *)(device)) & 0x80) | 0x01;
	// Wait to finish
	while(!((*((uint32_t *)(device)) >> 1) & 0x1));
}

// This functions sets the device in auto-restart mode
void loop_device(volatile uint32_t* device)
{
	// Set autorestart
	*((uint32_t *)(device)) = (*((uint32_t *)(device)) & 0x81);
}

// Read 4 32-bit pixels and save them at once
void load_frame(const uint8_t* img, uint32_t height, uint32_t width, volatile uint32_t *dest)
{
	int idx, temp_idx = 0;

	for(idx = 0; idx < height * width / 4; idx++)
	{
		*(dest + idx) = (volatile uint32_t) (*(img + temp_idx) 		     |
											 *(img + temp_idx + 1) << 8  |
											 *(img + temp_idx + 2) << 16 |
											 *(img + temp_idx + 3) << 24);
		temp_idx += 4;
	}
}

void write_color_bars(volatile uint32_t* dest)
{
        for(int idx = 0; idx < FRAME_SIZE/4; idx++)
	{
               *(dest + idx) = (uint32_t)0x9b59b6;
        }
        for(int idx = FRAME_SIZE/4; idx <FRAME_SIZE/2 ; idx++)
        {
            	*(dest + idx) = (uint32_t)0x2ecc71;
        }
        for(int idx = FRAME_SIZE/2; idx <FRAME_SIZE*3/4 ; idx++)
	{
            	*(dest + idx) = (uint32_t)0xf1c40f;
	}
        for(int idx = FRAME_SIZE*3/4; idx <FRAME_SIZE ; idx++)
	{
            	*(dest + idx) = (uint32_t)0x3498db;
	}
}

void store_image(volatile uint32_t* dest, uint8_t *src)
{
        uint32_t k = 0;
        for(int idx = 0; idx < FRAME_SIZE; idx++)
        {
                *(dest + idx) = (uint32_t)(*(src + k) << 24 |
                                           *(src + k) << 16 |
                                           *(src + k) << 8 |
                                           *(src + k++));
        }
}

int64_t SetIntValue(acNodeMap hNodeMap, const char* nodeName, int64_t value)
{
        AC_ERROR err = AC_ERR_SUCCESS;

        // get node
        acNode hNode = NULL;
        AC_ACCESS_MODE accessMode = 0;

        err = acNodeMapGetNodeAndAccessMode(hNodeMap, nodeName, &hNode, &accessMode);
        if (err != AC_ERR_SUCCESS)
                return err;

        // check access mode
        if (accessMode != AC_ACCESS_MODE_WO && accessMode != AC_ACCESS_MODE_RW)
                return AC_ERR_ERROR;

        // retrieve the range and increment for the node.
        int64_t minimum = 0;
        int64_t maximum = 0;
        int64_t increment = 0;

        err = acIntegerGetMin(hNode, &minimum);
        if (err != AC_ERR_SUCCESS)
                return err;

        err = acIntegerGetMax(hNode, &maximum);
        if (err != AC_ERR_SUCCESS)
                return err;

        err = acIntegerGetInc(hNode, &increment);
        if (err != AC_ERR_SUCCESS)
                return err;

        // Ensure increment
        //    If a node has an increment (all integer nodes & some float nodes), only
        //    multiples of the increment can be set. Ensure this by dividing and then
        //    multiplying by the increment. If a value is between two increments,
        //    this will push it to the lower value. Most minimum values are divisible
        //    by the increment. If not, this must also be considered in the
        //    calculation.
        value = (((value - minimum) / increment) * increment) + minimum;

        // Check min/max values
        //    Values must not be less than the minimum or exceed the maximum value of
        //    a node. If a value does so, simply push it within range.
        if (value < minimum)
        {
                value = minimum;
        }
        if (value > maximum)
        {
                value = maximum;
        }

        // set integer value
        err = acIntegerSetValue(hNode, value);
        if (err != AC_ERR_SUCCESS)
                return err;

        // return value for output
        return value;
}

AC_ERROR AcquireImagesRapidly(acDevice hDevice)
{
        AC_ERROR err = AC_ERR_SUCCESS;

        // get node map
        acNodeMap hNodeMap = NULL;

        err = acDeviceGetNodeMap(hDevice, &hNodeMap);
        if (err != AC_ERR_SUCCESS)
                return err;


        char pPixelFormatInitial[MAX_BUF];
        size_t pixelFormatLen = MAX_BUF;
        int64_t widthInitial = 0;
        int64_t heightInitial = 0;
        acNode hDeviceStreamChannelPacketSizeNode = NULL;
        int64_t deviceStreamChannelPacketSizeInitial = 0;

        err = acNodeMapGetEnumerationValue(hNodeMap, "PixelFormat", pPixelFormatInitial, &pixelFormatLen) |
                  acNodeMapGetIntegerValue(hNodeMap, "Width", &widthInitial) |
                  acNodeMapGetIntegerValue(hNodeMap, "Height", &heightInitial);
        if (err != AC_ERR_SUCCESS)
        printf("Error: failed to retrieve one or more initial node values.\n");
        // get node
	err = acNodeMapGetNode(hNodeMap, "DeviceStreamChannelPacketSize", &hDeviceStreamChannelPacketSizeNode);
	if (err != AC_ERR_SUCCESS)
		return err;

        // Set low width and height
        //    Reducing the size of an image reduces the amount of bandwidth required
        //    for each image. The less bandwidth required per image, the more images
        //    can be sent over the same bandwidth.
        printf("%sSet low width and height", TAB1);
        int64_t imageWidth = SetIntValue(hNodeMap, "Width", FRAME_WIDTH);
        int64_t imageHeight = SetIntValue(hNodeMap, "Height", FRAME_HEIGHT);

        printf(" (%" PRId64 "x%" PRId64 ")\n", imageWidth, imageHeight);

        // Set small pixel format
        //    Similar to reducing the ROI, reducing the number of bits per pixel also
        //    reduces the bandwidth required for each image. The smallest pixel
        //    formats are 8-bit bayer and 8-bit mono (i.e. BayerRG8 and Mono8).
        printf("%sSet small pixel format (%s)\n", TAB1, FORMAT_MONO);

        err = acNodeMapSetEnumerationValue(hNodeMap, "PixelFormat", FORMAT_MONO);
        if (err != AC_ERR_SUCCESS)
                return err;

        err = acIntegerSetValue(hDeviceStreamChannelPacketSizeNode, 1500);
        if (err != AC_ERR_SUCCESS)
                return err;

        printf("%sStart stream with %i buffers\n", TAB1, NUM_IMAGES);

        err = acDeviceStartStream(hDevice);
        if (err != AC_ERR_SUCCESS)
                return err;

        uint8_t *buff;
        // get images
        for (int i = 1; i <= NUM_IMAGES; i++)
        {
                printf("\r%sGet image %d", TAB2, i);
                fflush(stdout);


                acBuffer hBuffer = NULL;

                // get buffer for each image
                err = acDeviceGetBuffer(hDevice, IMAGE_TIMEOUT,&hBuffer);
                if (err != AC_ERR_SUCCESS)
                        return err;

                acImageGetData(hBuffer, &buff);
                store_image(frame_memory,buff);
                *((uint32_t *)(frame_filter)) = (*((uint32_t *)(frame_filter)) & 0x80) | 0x01;
                // Wait to finish
                while(!((*(frame_filter)) >> 1) & 0x1);

                // requeue buffer for each image
                err = acDeviceRequeueBuffer(hDevice, hBuffer);
                if (err != AC_ERR_SUCCESS)
                        return err;
        }

        // stop stream
        printf("\n%sStop stream\n", TAB1);

        err = acDeviceStopStream(hDevice);
        if (err != AC_ERR_SUCCESS)
                return err;


        err = acNodeMapSetEnumerationValue(hNodeMap, "PixelFormat", pPixelFormatInitial) |
                  acNodeMapSetIntegerValue(hNodeMap, "Width", widthInitial) |
                  acNodeMapSetIntegerValue(hNodeMap, "Height", heightInitial);
        if (err != AC_ERR_SUCCESS)
                printf("Error: failed to set one or more node values back to its initial value.\n");

        return err;
}

#define CHECK_RETURN                                    \
        if (err != AC_ERR_SUCCESS)                      \
        {                                               \
                printf("Press enter to complete");      \
                getchar();                              \
                return -1;                              \
        }

int main()
{
	struct timespec start, stop;
	double diff;
	int fd, idx;


  	bind_device(&frame_memory,DEVICE_TYPE_MEMORY);
	bind_device(&frame_filter,DEVICE_TYPE_FILTER);
	bind_device(&frame_display,DEVICE_TYPE_VGA);

        AC_ERROR err = AC_ERR_SUCCESS;

        // prepare example
        acSystem hSystem = NULL;
        err = acOpenSystem(&hSystem);
        CHECK_RETURN;
        err = acSystemUpdateDevices(hSystem, SYSTEM_TIMEOUT);
        CHECK_RETURN;
        size_t numDevices = 0;
        err = acSystemGetNumDevices(hSystem, &numDevices);
        CHECK_RETURN;
        if (numDevices == 0)
        {
                printf("Error: no devices connected, example requires at least one device.\n");
                printf("Press enter to complete\n");
                getchar();
                return -1;
        }
        acDevice hDevice = NULL;
        err = acSystemCreateDevice(hSystem, 0, &hDevice);
        CHECK_RETURN;

        // Enable auto-restart
        *((uint32_t *)(frame_display)) = (uint32_t) 0x81;
/*
        unsigned int k = 0;
        for(int a = 0; a < FRAME_SIZE; a++)
        {
  	        // Get start time
	        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
                for(int idx = 0; idx < FRAME_SIZE; idx++)
                {
                        *(frame_memory + idx) = 	 k++;
                }
                *((uint32_t *)(frame_filter)) = (*((uint32_t *)(frame_filter)) & 0x80) | 0x01;
                // Wait to finish
                while(!((*(frame_filter)) >> 1) & 0x1);
                // Get end time
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
                diff = (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1e3;
                LOG("\r\nFrame took %f  microseconds\n\n",diff);
        }*/

        err = AcquireImagesRapidly(hDevice);
        CHECK_RETURN;

        // clean up
        err = acSystemDestroyDevice(hSystem, hDevice);
        CHECK_RETURN;
        err = acCloseSystem(hSystem);
        CHECK_RETURN;


        unbind_device(&frame_memory,DEVICE_TYPE_MEMORY);
	unbind_device(&frame_filter,DEVICE_TYPE_FILTER);
	unbind_device(&frame_display,DEVICE_TYPE_VGA);
	LOG("\nEnd of test\n");
        return -1;
}
