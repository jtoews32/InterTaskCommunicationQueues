/* ###################################################################
**     Filename    : main.c
**     Project     : InterTaskCommunicationQueues
**     Processor   : MK64FN1M0VLQ12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-12-26, 20:14, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "FRTOS1.h"
#include "UTIL1.h"
#include "TI1.h"
#include "TimerIntLdd1.h"
#include "TU1.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "PDD_Includes.h"
#include "Init_Config.h"
#include "FRTOS1.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

static QueueHandle_t messageQueueHandle;
extern SemaphoreHandle_t timerSemaphoreHandle;
static TaskHandle_t cleanupTaskHandle = NULL;
static TaskHandle_t healthcheckTaskHandle = NULL;
static TaskHandle_t updateTaskHandle = NULL;

typedef enum {
	CLEANUP,
	HEALTHCHECK,
	UPDATE,
	FORWARD,
	LEFT,
	RIGHT,
	BACK,
	LIGHTON,
	LIGHTOFF
} MType;

typedef struct {
	MType type;
    uint16_t id;
    void *params;
} Message;

Message newMessage(MType type, uint16_t id, void *params) {
  Message retval;
  retval.id = id;
  retval.type = type;
  retval.params = params;
  return retval;
}


void cleanupTask(void *params) {
	// Do the work

    if( cleanupTaskHandle != NULL ) {
    	 FRTOS1_vTaskDelete(cleanupTaskHandle);
    }
}

void healthcheckTask(void *params) {
	// Do the work


    if( healthcheckTaskHandle != NULL ) {
    	 FRTOS1_vTaskDelete(healthcheckTaskHandle);
    }
}

void updateTask(void *params) {
	// Do the work


    if( updateTaskHandle != NULL ) {
    	 FRTOS1_vTaskDelete(updateTaskHandle);
    }
}


void controllerTask(void *params) {
	while (1) {
		if (FRTOS1_xSemaphoreTake(timerSemaphoreHandle, 1000)) {

			if( messageQueueHandle != 0 ) {
				// Can check if queue exists
			}
			/*
	Message message = newMessage(LIGHTOFF, 100, NULL);
	Message *message1 = &message;
	int test = message1->id;
	int test1 = message.id;
	int test2 = message.id;

			 */

			Message message = newMessage(CLEANUP, 0x01, NULL);
			Message *pxMessage = &message;

			if ( FRTOS1_xQueueSendToBack(messageQueueHandle, &message, 100) != pdPASS) {}

			message = newMessage(HEALTHCHECK, 0x02, NULL);
			pxMessage = &message;

			if ( FRTOS1_xQueueSendToBack(messageQueueHandle, &message, 100) != pdPASS) {}

			message = newMessage(UPDATE, 0x03, NULL);
			pxMessage = &message;

			if ( FRTOS1_xQueueSendToBack(messageQueueHandle, &pxMessage, 100) != pdPASS) {}

			// etc....
			// Add more checks on sensors to schedule jobs.
		}
	}
}

void queueConsumerTask(void *params)
{
	Message *pxRxedMessage;


	while(1) {
       if( FRTOS1_xQueueReceive( messageQueueHandle, &pxRxedMessage, ( TickType_t ) 100000 ) ) {

    	   void *messageParams = pxRxedMessage->params;

    	   switch( pxRxedMessage->type ) {
    	   case CLEANUP:

    		   // Throw the params on there
    		   if (FRTOS1_xTaskCreate(cleanupTask, (signed portCHAR *) "Cleanup Task", configMINIMAL_STACK_SIZE,
    				   &messageParams, tskIDLE_PRIORITY, &cleanupTaskHandle ) != pdPASS) {
    		 	for(;;){};
    		   }
    		   break;
    	   case HEALTHCHECK:

    		   // Throw the params on there
    		   if (FRTOS1_xTaskCreate(healthcheckTask, (signed portCHAR *) "Healthcheck Task", configMINIMAL_STACK_SIZE,
    				   &messageParams, tskIDLE_PRIORITY, &healthcheckTaskHandle  ) != pdPASS) {
    		 	for(;;){};
    		   }
    		   break;
    	   case UPDATE:

    		   // Throw the params on there
    		   if (FRTOS1_xTaskCreate(updateTask, (signed portCHAR *) "Update Task", configMINIMAL_STACK_SIZE,
    				   &messageParams, tskIDLE_PRIORITY, &updateTaskHandle ) != pdPASS) {
    		 	for(;;){};
    		   }
    		   break;
    	   default:
    		   // HASN'T BEEN DEFINED YET
    		   break;
    	   }
       }
	}
}


/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  timerSemaphoreHandle = FRTOS1_xSemaphoreCreateBinary();
  messageQueueHandle = FRTOS1_xQueueCreate( 100, sizeof(Message *) );

  if (FRTOS1_xTaskCreate(controllerTask, (signed portCHAR *) "Controller Task", configMINIMAL_STACK_SIZE,
	(void*) NULL, tskIDLE_PRIORITY, (xTaskHandle*) NULL ) != pdPASS) {
	for(;;){};
  }

  FRTOS1_vTaskStartScheduler();

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.4 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
