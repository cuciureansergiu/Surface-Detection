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

#define LOG 			printf

#define TEST_SIZE		10

#define FRAME_WIDTH		256/4	// The actual resolution is 256x256 but
#define FRAME_HEIGHT		256/4	// the data is read 4 bytes at a time
#define FRAME_SIZE		FRAME_HEIGHT * FRAME_WIDTH

#define STATUS_OK				0
#define STATUS_OPENING_DEV_MEM_FAILED		1
#define STATUS_MAPPING_FAILED 			2
#define STATUS_UNMAPPING_FAILED			3
#define STATUS_CLOSING_DEV_MEM_FAILED		4
#define STATUS_COPYING_DATA_FAILED		5

#define BRAM_CH0	0
#define BRAM_CH1	1
#define BRAM_CH2	2
#define BRAM_SIZE 	sizeof(uint8_t) * 1024 * 64 //64KB BRAM block

const off_t bram_ch[] =  {0x40000000u, 
			  0x42000000u, 
			  0x44000000u};
const off_t device =	  0x43C00000u;
const off_t addr_range =  0x00010000u;

static const char * status_codes[] = {
    "OK",
    "Failed to open /dev/mem",
    "Failed to map",
    "Failed to unmap",
    "Failed to close /dev/mem",
    "Failed to copy data",
};

const char* get_status(int status)
{
	return status_codes[status];
}

void err(int status)
{
  LOG(get_status(status));
}

int map_device(volatile void **ptr, size_t size, int port, off_t offset)
{
	int fd;

	fd = open("/dev/mem", O_RDWR);
	if(fd < 1) 
	{
   		return STATUS_OPENING_DEV_MEM_FAILED;
   	}
	LOG("Mapping device \n");
	*ptr = (volatile void *) mmap(NULL, size, port, MAP_SHARED, fd, offset);
	if(*ptr == MAP_FAILED)
	{
		return STATUS_MAPPING_FAILED;
	}
	LOG("Device mapped to user space, 0x%x\n", ptr);
	if(!close(fd))
	{//we can close the file descriptor as mmap keeps it automatically open until we munmap
	   return STATUS_OK;
	}
	else
	{
	   return STATUS_CLOSING_DEV_MEM_FAILED;
	}
}

int unmap(volatile void **ptr, size_t size)
{
   	LOG("Unmapping device \n");
   	if(!(munmap(*((void **)ptr), size) == 0))
   	{
       	return STATUS_UNMAPPING_FAILED;
   	}
   	*ptr = NULL;
   	LOG("Device unmapped");

   	return STATUS_OK;
}

int bind_bram(volatile uint32_t **ptr, uint8_t ch)
{
	LOG("Binding BRAM ch[%d] with physical address = 0x%x\n", ptr, bram_ch[ch]);
	return map_device((volatile void **)ptr, BRAM_SIZE, PROT_READ|PROT_WRITE, bram_ch[ch]);
}

int unbind_bram(volatile uint32_t **ptr)
{
	LOG("Unbinding BRAM ch with physical address = 0x%x\n", ptr);
   	return unmap((volatile void **) ptr, BRAM_SIZE);
}

int bind_device(volatile uint32_t **ptr)
{
	LOG("Binding device with physical address = 0x%x\n", device);
	return map_device((volatile void **)ptr, addr_range, PROT_READ|PROT_WRITE, device);
}

int unbind_device(volatile uint32_t **ptr)
{
	LOG("Unbinding device with physical address = 0x%x\n", ptr);
   	return unmap((volatile void **) ptr, addr_range);
}

void soft_device(uint32_t *input, uint32_t* kernel, uint32_t* output)
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



int main(void)
{
	struct timespec start, stop;
	double diff;
	int fd, idx;
	volatile uint32_t *device, *input, *kernel, *output;
    	
  bind_bram(&input,  BRAM_CH0);
	bind_bram(&kernel, BRAM_CH1);
	bind_bram(&output, BRAM_CH2);
	bind_device(&device);

	LOG("\r\nStarting device test\n");
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

	unbind_bram(&input);
	unbind_bram(&kernel);
	unbind_bram(&output);
	unbind_device(&device);

  LOG("End of test\n");

	return 0;
}
