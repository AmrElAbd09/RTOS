

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
/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )



static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

#define PRESSED 		1
#define RELEASED		0

#define MIN_DELAY		10

TaskHandle_t LedTask_Handler = NULL;
TaskHandle_t ButtonTask_Handler = NULL;

uint32_t g_ButtonState = RELEASED;


void buttonTask(void *pvParameters) {
	
		uint8_t u8_PressFlag = RELEASED;
    uint32_t u32_Signal = NULL;
    
    for(;;) 
			{
        g_ButtonState = GPIO_read(PORT_0,PIN0);
        
        if (g_ButtonState == PRESSED && u8_PressFlag == RELEASED) 
					{
            u8_PressFlag = PRESSED;
					} 
				else if (g_ButtonState == RELEASED && u8_PressFlag == PRESSED) 
					{
						xTaskNotify(LedTask_Handler,u32_Signal ,eSetValueWithOverwrite);     
            u8_PressFlag = RELEASED;
					}
        
        vTaskDelay(MIN_DELAY); 
    }
}


void Led_Task (void * pvParameters)
{
	uint32_t u32_Signal = NULL;
	uint8_t u8_LedState = NULL;
	for(;;)
	{
		xTaskNotifyWait( 	0x00, 
											ULONG_MAX, 
											&u32_Signal, 
											portMAX_DELAY);
		
		u8_LedState = GPIO_read(PORT_0, PIN1);
		
		if (u8_LedState == PIN_IS_HIGH)
			{
				GPIO_write(PORT_0, PIN1,PIN_IS_LOW);
			}	
		else
			{
				GPIO_write(PORT_0, PIN1,PIN_IS_HIGH);
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

	
    /* Create Tasks here */
		xTaskCreate(	buttonTask, 
									"Button Task",
									configMINIMAL_STACK_SIZE,
									( void * ) NULL,
									2,
									&ButtonTask_Handler);
	
		xTaskCreate( Led_Task,
								 "Led_Task",
								 configMINIMAL_STACK_SIZE,
								 ( void * ) NULL,
								 1,
								 &LedTask_Handler );
								 

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


