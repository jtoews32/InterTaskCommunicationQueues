# InterTaskCommunicationQueue

Respond to each tick of the timer by checking a number of sensors. Based on the result of the sensor
put something in the queue. If something is in the queue it will be pulled out and the correct
job/task will be created. The created task wtih execture and then will delete itself after running.

Components:
*	FreeRTOS		
*	FreeRTOS Semaphore
*	FreeRTOS Queue
*	Timer
*	

