#include "includes.h"

/*************************** the following include files are added in order to make use of beep ******/
#include "stdio.h"
#include "NUC1xx.h"
#include "DrvGPIO.h"
#include "DrvUART.h"
#include "DrvSYS.h"

/* for the use of 7 segment displaying */
#include "Seven_Segment.h"
#define BAUDRATE 9600

/******************************************* definitions ************************************************/
#define TASK_STK_SIZE 256 // Size of each task's stacks (# of WORDs)
#define N_TASKS 5					// Number of identical tasks
#define TASK_START_ID 0		// Application tasks IDs

OS_STK TaskStk[N_TASKS][TASK_STK_SIZE]; // Tasks stacks
OS_STK TaskStartStk[TASK_STK_SIZE];			// TaskStart  task stack
INT8U TaskData[N_TASKS];								// Parameters to pass to each task
OS_EVENT *mutex;

INT32U count = 0; // Initialize count
char adc_value[12] = "Times:";
/******************************************* declarations ************************************************/

static void Task0(void *pdata); // Function prototypes of tasks
static void Task1(void *pdata); // Function prototypes of tasks
static void Task2(void *pdata); // Function prototypes of tasks

static void TaskStart(void *pdata);			// Function prototypes of Startup task
static void TaskStartCreateTasks(void); // Function prototypes of creat task

/******************************************* implementations ************************************************/


int main(void)
{
	INT8U err = err;

	UNLOCKREG();
	SYSCLK->PWRCON.XTL12M_EN = 1; //Enable 12Mhz and set HCLK->12Mhz
	SYSCLK->CLKSEL0.HCLK_S = 0;
	LOCKREG();

	UCOS_CPU_INIT();

	OSInit(); // Initialize uC/OS-II

	mutex = OSSemCreate(1);
	OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 4);

	OSStart(); // Start multitasking

	return 0;
}

/**
 * @brief
 * 		TaskStart()函数，启动任务。
 * @param[in]		void *。
 * @return 无\n
 */
void TaskStart(void *pdata)
{
	//	int32_t i;

#if OS_CRITICAL_METHOD == 3 // Allocate storage for CPU status register
	OS_CPU_SR cpu_sr;
	cpu_sr = cpu_sr; // Avoid warnings
#endif

	pdata = pdata; // Prevent compiler warning

	UCOS_TIMER_START();

	TaskStartCreateTasks(); // Create all the application tasks

	/*the following codes are added in order to make use of the buzzer */
	DrvGPIO_Open(E_GPB, 11, E_IO_OUTPUT); // initial GPIO pin GPB11 for controlling Buzzer

	DrvGPIO_ClrBit(E_GPB, 11); // GPB11 = 0 to turn on Buzzer

	Delay(1000000); // Delay to keep Buzz

	//	DrvGPIO_SetBit(E_GPB,11); // GPB11 = 1 to turn off Buzzer

	/* 	for (i=0; i<10000; i++)			// counting 0~9999
	{
	   seg_display(i);				// display value to 7-segment display
	   Delay(10000);
	}

*/

	OSTaskSuspend(OS_PRIO_SELF); // Suspend the TaskStart
}

/**
 * @brief
 * 		TaskStartCreateTasks()函数，创建多任务。
 * @param[in]		void *。
 * @return 无
 */
void TaskStartCreateTasks(void)
{
	INT8U i;

	for (i = 0; i < N_TASKS; i++) // Create tasks
	{
		TaskData[i] = i; // Each task will display its own information
	}

	//the following steps are creating tasks.
	OSTaskCreate(Task0, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 5);
	OSTaskCreate(Task1, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 6);
	OSTaskCreate(Task2, (void *)&TaskData[2], &TaskStk[2][TASK_STK_SIZE - 1], 7);
}

/**
 * @brief
 * 		Task0()函数，执行一个任务。
 * @param[in]		void *。
 * @return 无P
 */
void Task0(void *pdata)
{
	INT8U err;
	INT8U id;
	id = *(int *)pdata;
	for (;;)
	{
		printf("Task_%d waitting for an EVENT\n\r", id);
		OSTimeDly(200); /* Delay 200 clock tick */
		printf("Task_%d's EVENT CAME!\n\r", id);
		printf("Task_%d trying to GET MUTEX\n\r", id);
		OSSemPend(mutex, 0, &err); /* Acquire mutex */
		switch (err)
		{
		case OS_NO_ERR:
			printf("Task_%d GOT mutex.\n\r", id);
			break;
		default:
			printf("Task_%d CANNOT get mutex, then SUSPENDED.\n\r", id);
		}
		OSTimeDly(200); /* Delay 200 clock tick */
		printf("Task_%d RELEASE mutex\n\r", id);
		OSSemPost(mutex); /* Release mutex */
	}
}

void Task1(void *pdata)
{
	INT8U id;
	id = *(int *)pdata;
	for (;;)
	{
		printf("Task_%d waitting for an EVENT\n\r", id);
		OSTimeDly(100); /* Delay 100 clock tick*/
		printf("Task_%d's EVENT CAME!\n\r", id);
		OSTimeDly(100);
	}
}

void Task2(void *pdata)
{
	INT8U err;
	INT8U id;
	id = *(int *)pdata;
	for (;;)
	{
		printf("Task_%d trying to GET MUTEX\n\r", id);
		OSSemPend(mutex, 0, &err); /* Acquire mutex */
		switch (err)
		{
		case OS_NO_ERR:
			printf("Task_%d GOT mutex.\n\r", id);
			OSTimeDly(200); /* Delay 200 clock tick */
			break;
		default:
			printf("Task_%d CANNOT get mutex, then SUSPENDED.\n\r", id);
			OSTimeDly(200); /* Delay 200 clock tick */
			break;
		}
		printf("Task_%d RELEASE mutex\n\r", id);
		OSSemPost(mutex); /* Release mutex */
	}
}
