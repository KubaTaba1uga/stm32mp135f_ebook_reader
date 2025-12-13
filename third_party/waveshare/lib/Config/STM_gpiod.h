#ifndef __GPIOD_STM
#define __GPIOD_STM

#include <stdio.h>
#include <gpiod.h>

#define STM_GPIOD_IN  0
#define STM_GPIOD_OUT 1

#define STM_GPIOD_LOW  0
#define STM_GPIOD_HIGH 1

#define NUM_MAXBUF  4
#define DIR_MAXSIZ  60

#define STM_GPIOD_DEBUG 0
#if STM_GPIOD_DEBUG 
	#define STM_GPIOD_Debug(__info,...) printf("Debug: " __info,##__VA_ARGS__)
#else
	#define STM_GPIOD_Debug(__info,...)  
#endif

// Return int wich can be used as pin. It is done like so to be in sync with RPI_gpiod interface.
int STM_GPIOD_register(int pin, char bank);

int STM_GPIOD_Export();
int STM_GPIOD_Unexport(int Pin);
int STM_GPIOD_Unexport_GPIO(void);
int STM_GPIOD_Direction(int Pin, int Dir);
int STM_GPIOD_Read(int Pin);
int STM_GPIOD_Write(int Pin, int value);

#endif
