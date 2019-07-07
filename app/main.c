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
//#include "img.h"

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

int main(void)
{
	struct timespec start, stop;
	double diff;
	int fd, idx;
	volatile uint32_t *frame_filter,
			  *frame_display,
			  *frame_memory;


  	bind_device(&frame_memory,DEVICE_TYPE_MEMORY);
	bind_device(&frame_filter,DEVICE_TYPE_FILTER);
	bind_device(&frame_display,DEVICE_TYPE_VGA);

        for(int idx = 0; idx < FRAME_SIZE; idx++)
       	{
           	*(frame_memory + idx) = 	 0xFF0000;
       	}
        write_color_bars(frame_memory);
        LOG("\n\na\n\n");
        // Enable auto-restart
	*((uint32_t *)(frame_display)) = (uint32_t) 0x81;
        LOG("\n\na\n\n");
	*((uint32_t *)(frame_filter)) = (*((uint32_t *)(frame_filter)) & 0x80) | 0x01;
	// Wait to finish
	while(!((*((uint32_t *)(frame_filter)) >> 1) & 0x1));
        LOG("\n\na\n\n");
        unbind_device(&frame_memory,DEVICE_TYPE_MEMORY);
	unbind_device(&frame_filter,DEVICE_TYPE_FILTER);
	unbind_device(&frame_display,DEVICE_TYPE_VGA);
	LOG("\nEnd of test\n");

/*
	LOG("\r\nStarting device test\n");

	for(int idx = 0; idx < FRAME_SIZE; idx++)
	{
    	*(frame_memory + idx) = (uint32_t) 0xD3D3D3D3;
	}
	LOG("%x",*(mem_i0));
	LOG("%x",*(mem_i0 + 1));
	LOG("%x",*(mem_i45));

	LOG("\r\nLooping devices\n");
	// Set autorestart




        //int jdx = 0;
	  /*XBram_read_EnableAutoRestart(&bramVidInstance);
	  XBram_read_Start(&bramVidInstance);*/
	  //*((uint32_t *)(vga)) = (*((uint32_t *)(vga)) & 0x80) | 0x01;
	//*(vga_ctrl) = (uint32_t) 0x81;
	/**((uint32_t *)(conv)) = (uint32_t) 0x80;*/

        	/*
    	LOG("test");
        while(1)
        {
                for(int idx = 0; idx < FRAME_SIZE; idx++)
            	{
                	*(mem_i0 + idx) = 	 jdx;
            	}
                jdx++;
            	/*XGradient_Start(&gradientInstance);
            	while(!XGradient_IsDone(&gradientInstance));

            	*((uint32_t *)(surface)) = (*((uint32_t *)(surface)) & 0x80) | 0x01;
            	// Wait to finish
            	while(!((*((uint32_t *)(surface)) >> 1) & 0x1));
            	LOG("test");
        }

 	// Prepare the buffers
	for(idx = 0; idx < TEST_SIZE; idx++)
	{
		*(input + idx)  = (uint32_t) 0x02020202;
		*(kernel + idx) = (uint32_t) 0x03030303;
		*(output + idx) = (uint32_t) 0x00000000;
	}
  	// Get start time
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	// Start the device
	*((uint32_t *)(device)) = (*((uint32_t *)(device)) & 0x80) | 0x01;
	// Wait to finish
	while(!((*((uint32_t *)(device)) >> 1) & 0x1));

	// Get end time
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
	for(idx = 0; idx < TEST_SIZE; idx++)
	{
		if(*(output + idx) != (uint32_t) 0x06060606)
		{
			LOG("Corrupted data : i:%X k:%X o:%X\n",*(input + idx), *(kernel + idx), *(output + idx));
			break;
		}
	}
	diff = (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1e3;
	LOG("\r\nSWtest took %f  microseconds\n\n",diff);

  	// Prepare the buffer
  	for(idx = 0; idx < TEST_SIZE; idx++)
  	{
    	*(output + idx) = (uint32_t) 0x00000000;
  	}
  	// Get start time
  	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
  	// Start sw process
  	soft_device(input, kernel, output);
  	// Get end time
  	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
  	for(idx = 0; idx < TEST_SIZE; idx++)
  	{
    	if(*(output + idx) != (uint32_t) 0x06060606)
    	{
      	LOG("Corrupted data : i:%X k:%X o:%X\n",*(input + idx), *(kernel + idx), *(output + idx));
      	break;
    	}
  	}
  	diff = (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1e3;
  	LOG("\r\nHWtest took %f  microseconds\n\n",diff);
	*/


	return 0;
}
