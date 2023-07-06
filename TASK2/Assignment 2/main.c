

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"

#include <limits.h>
#include <semphr.h>
#include <string.h>
/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )



static void prvSetupHardware( void );
/*-----------------------------------------------------------*/
#define HEAVY_LOAD_DELAY 	100000
#define TASK_1_DELAY			100
#define TASK_2_DELAY			500
#define COUNTER_MAX				10


TaskHandle_t Task_1_Handler = NULL;
TaskHandle_t Task_2_Handler = NULL;




SemaphoreHandle_t	ChannelHandle;


void Task_1 (void * pvParameters)
{
	const char* StringArray = "HELLO..";
	uint8_t u8_counter;
	for(;;)
	{
		if(xSemaphoreTake( ChannelHandle , portMAX_DELAY) == pdTRUE)
		{
			for(u8_counter = NULL; u8_counter <COUNTER_MAX; u8_counter++)
			{
				while(vSerialPutString(StringArray, strlen(StringArray)) == pdFALSE)
				{}
			}
			xSemaphoreGive(ChannelHandle);
			vTaskDelay(TASK_1_DELAY);
		}
	}
}


void Task_2 (void * pvParameters)
{
	uint8_t u8_counter;
	uint32_t u32_HeavyLoadCounter;
	const char* StringArray = "BYE..";
	for(;;)
	{
		if(xSemaphoreTake( ChannelHandle , portMAX_DELAY) == pdTRUE)
		{
			for(u8_counter = NULL; u8_counter <COUNTER_MAX; u8_counter++)
			{
				while(vSerialPutString(StringArray, strlen(StringArray)) == pdFALSE)
				{
					for(u32_HeavyLoadCounter = NULL; u32_HeavyLoadCounter < HEAVY_LOAD_DELAY; u32_HeavyLoadCounter++)
					{}
				}		
			}
			xSemaphoreGive(ChannelHandle);
			vTaskDelay(TASK_2_DELAY);
		}
	}
}

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
	ChannelHandle = xSemaphoreCreateBinary();
	xSemaphoreGive(ChannelHandle);
    /* Create Tasks here */

		xTaskCreate( Task_1,
								 "Task_1",
								 configMINIMAL_STACK_SIZE,
								 ( void * ) NULL,
								 2,
								 &Task_1_Handler );
								 
		xTaskCreate( Task_2,
								 "Task_2",
								 configMINIMAL_STACK_SIZE,
								 ( void * ) NULL,
								 1,
								 &Task_2_Handler );					 

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


