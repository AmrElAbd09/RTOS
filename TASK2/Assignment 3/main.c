

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
/*-----------------------Macros------------------------------*/
#define BTN_1_PORT			PORT_0
#define BTN_1_PIN				PIN0
#define BTN_2_PORT			PORT_0
#define BTN_2_PIN				PIN1

#define PERIODIC_TASK_DELAY		100
#define MIN_DELAY							5


/*-----------------Task Handlers------------------------------*/
TaskHandle_t Button1TaskHandler = NULL;
TaskHandle_t Button2TaskHandler = NULL;
TaskHandle_t PeriodicTaskHandler = NULL;
TaskHandle_t ConsumerTaskHandler = NULL;


QueueHandle_t EventQueue;


void Button1Task(void *pvParameters) {
		uint8_t u8_PressFlag = pdFALSE;
		uint8_t ButtonState;
    for(;;)
			{
				ButtonState = GPIO_read(BTN_1_PORT,BTN_1_PIN);  
        if (ButtonState == pdTRUE && u8_PressFlag == pdFALSE) 
					{
						const char *eventString = "Button 1 Rising Edge";
            xQueueSend(EventQueue, &eventString, portMAX_DELAY);
            u8_PressFlag = pdTRUE;
					} 
				else if (ButtonState == pdFALSE && u8_PressFlag == pdTRUE) 
					{
						const char *eventString = "Button 1 Falling Edge";
            xQueueSend(EventQueue, &eventString, portMAX_DELAY);
						u8_PressFlag = pdFALSE;
					}
				else{}					
        vTaskDelay(MIN_DELAY);
    }
}


void Button2Task(void *pvParameters) {
		uint8_t u8_PressFlag = pdFALSE;
		uint8_t ButtonState;
    for(;;)
			{
				ButtonState = GPIO_read(BTN_2_PORT,BTN_2_PIN);  
        if (ButtonState == pdTRUE && u8_PressFlag == pdFALSE) 
					{
						const char *eventString = "Button 2 Rising Edge";
            xQueueSend(EventQueue, &eventString, portMAX_DELAY);
            u8_PressFlag = pdTRUE;
					} 
				else if (ButtonState == pdFALSE && u8_PressFlag == pdTRUE) 
					{
						const char *eventString = "Button 2 Falling Edge";
            xQueueSend(EventQueue, &eventString, portMAX_DELAY);
						u8_PressFlag = pdFALSE;
					}
				else{}					
        vTaskDelay(MIN_DELAY);
    }
}


void PeriodicTask(void *pvParameters) {
    for(;;) 
				{
					const char *eventString = "Periodic Event Task";
					xQueueSend(EventQueue, &eventString, portMAX_DELAY);
					vTaskDelay(PERIODIC_TASK_DELAY);
				}
}


void ConsumerTask(void *pvParameters) {
    const char *eventString;

    for(;;) 
	{

        if (xQueueReceive(EventQueue, &eventString, portMAX_DELAY)) 
					{

            vSerialPutString (eventString, strlen(eventString));
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

	EventQueue = xQueueCreate(10, sizeof(const char *));

    /* Create Tasks here */

		xTaskCreate( Button1Task,
								 "Button1Task",
								 configMINIMAL_STACK_SIZE,
								 ( void * ) NULL,
								 1,
								 &Button1TaskHandler );
								 
								 
		xTaskCreate( Button2Task,
								 "Button2Task",
								 configMINIMAL_STACK_SIZE,
								 ( void * ) NULL,
								 2,
								 &Button2TaskHandler );	
								 
								 
		xTaskCreate( PeriodicTask,
								 "PeriodicTask",
								 configMINIMAL_STACK_SIZE,
								 ( void * ) NULL,
								 3,
								 &PeriodicTaskHandler );	
								 
								 
		xTaskCreate( ConsumerTask,
								 "ConsumerTask",
								 configMINIMAL_STACK_SIZE,
								 ( void * ) NULL,
								 4,
								 &ConsumerTaskHandler );							 

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


