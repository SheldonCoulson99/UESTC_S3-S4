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
#define TASK_STK_SIZE 64 // Size of each task's stacks (# of WORDs)
#define N_TASKS 2				 // Number of identical tasks
#define TASK_START_ID 0	 // Application tasks IDs

OS_STK TaskStk1[N_TASKS][TASK_STK_SIZE]; // Tasks stacks
OS_STK TaskStk2[N_TASKS][TASK_STK_SIZE]; // Tasks stacks
OS_STK TaskConStk[TASK_STK_SIZE];				 // TaskCon task stack

OS_STK TaskStartStk[TASK_STK_SIZE]; // TaskStart  task stack

INT8U TaskData1[N_TASKS]; // Parameters to pass to each task
INT8U TaskData2[N_TASKS]; // Parameters to pass to each task
OS_EVENT *mutex;

OS_EVENT *q1, *q2;
void *Msg1[6], *Msg2[6];

INT32U count = 0; // Initialize count
char adc_value[12] = "Times:";
/******************************************* declarations ************************************************/

static void Task1(void *pdata);		// Function prototypes of tasks
static void Task2(void *pdata);		// Function prototypes of tasks
static void TaskCon(void *pdata); // Function prototypes of tasks

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

	//mutex = OSSemCreate(1);
	//mutex = OSMutexCreate(8, &err);

	q1 = OSQCreate(&Msg1[0], 6);
	q2 = OSQCreate(&Msg2[0], 6);

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
		TaskData1[i] = i; // Each task will display its own information
		OSTaskCreate(Task1, (void *)&TaskData1[i], &TaskStk1[i][TASK_STK_SIZE - 1], i + 1);
	}

	for (i = 0; i < N_TASKS; i++)
	{
		TaskData2[i] = i;
		OSTaskCreate(Task2, (void *)&TaskData2[i], &TaskStk2[i][TASK_STK_SIZE - 1], i + 4);
	}
	OSTaskCreate(TaskCon, (void *)0, &TaskConStk[TASK_STK_SIZE - 1], i + 4);
}

/**
 * @brief
 * 		Task()函数，执行任务。
 * @param[in]		void *。
 * @return 无P
 */
void Task1(void *pdata)
{
	INT8U err;
	INT8U id;
	void *mg;
	id = *(int *)pdata;
	for (;;)
	{
		OSTimeDlyHMSM(0, 0, 2, 0); /* Wait 2 second */
		mg = OSQPend(q1, 0, &err); /* apply for message */
		switch (err)
		{
		case OS_NO_ERR:
		{
			/* If it is normally, just print the string.*/
			printf("Task_%d GOT %s\n\r", id, (char *)mg);
			OSTimeDlyHMSM(0, 0, 0, 200 * (4 - id));
			break;
		}
		default:
		{
			/* If the queue is empty or has been deleted, print another string.*/
			printf("Queue1 %d is EMPTY!\n\r", id);
			OSTimeDlyHMSM(0, 0, 0, 200 * (4 - id));
			break;
		}
		}
	}
}

void Task2(void *pdata)
{
	INT8U err;
	INT8U id;
	void *mg;
	id = *(int *)pdata;
	for (;;)
	{
		OSTimeDlyHMSM(0, 0, 2, 0); /* Wait 2 second */
		mg = OSQPend(q2, 0, &err); /* apply for message */
		switch (err)
		{
		case OS_NO_ERR:
		{
			/* If it is normally, just print the string.*/
			printf("Task_%d GOT %s\n\r", id + 3, (char *)mg);
			OSTimeDlyHMSM(0, 0, 0, 200 * (4 - id));
			break;
		}
		default:
		{
			/* If the queue is empty or has been deleted, print another string.*/
			printf("Queue2 is EMPTY, %d CANNOT get message!\n\r", id + 3);
			OSTimeDlyHMSM(0, 0, 0, 200 * (4 - id));
			break;
		}
		}
	}
}

void TaskCon(void *pdata)
{
	INT8U i, j;
	INT8U err;
	INT8U note = 1; /* for flush the queue */
	INT16U del = 3; /* for delete the queue */
	OS_EVENT *q;
	OS_Q_DATA *data;
	static char *s[] = {/* queue1's message */
											"message0",
											"message1",
											"message2",
											"message3",
											"message4",
											"message5"};
	static char *t[] = {/* queue2's message */
											"messageA",
											"messageB",
											"messageC",
											"messageD",
											"messageE",
											"messageF"};
	for (;;)
	{
		printf("...........................ADD MESSAGE TO QUEUE_1..............................\n\r");
		for (i = 0; i < 6; i++)
		{
			err = OSQPostFront(q1, (void *)s[i]); /* post message to q1 LIFO*/
			switch (err)
			{
			case OS_NO_ERR:
			{
				printf("Queue1 %d add %s\n\r", i, s[i]);
				OSTimeDlyHMSM(0, 0, 0, 150);
				break;
			}
			case OS_Q_FULL:
			{
				printf("Queue1 is full, CANNOT add.\n\r");
				OSTimeDlyHMSM(0, 0, 0, 150);
				break;
			}
			default:
				break;
			}
		}
		if (del > 0)
		{
			printf("..ADD MESSAGE TO QUEUE_2..............................\n\r");
		}
		for (j = 0; j < 6; j++)
		{
			err = OSQPost(q2, (void *)t[j]); /* post message to q2 FIFO*/
			switch (err)
			{
			case OS_NO_ERR:
			{
				printf("Queue2 %d add %s\n\r", j, t[j]);
				OSTimeDlyHMSM(0, 0, 0, 150);
				break;
			}
			case OS_Q_FULL:
			{
				printf("Queue2 is full, CANNOT add. \n\r");
				OSTimeDlyHMSM(0, 0, 0, 150);
				break;
			}
			default:
				break;
			}
		}
		if (del > 0)
		{
			if (note == 1)
			{
				OSQFlush(q2);
				// printf(".ADD MESSAGE TO QUEUE_2..............................\n\r");
				printf(".CLEAR UP QUEUE_2.................................\n\r");
				note = 0;
			}
			else
			{
				note = 1;
			}
		}
		// err = OSQQuery(q2, data); /* get the information about q2*/
		// if (err == OS_NO_ERR)
		// {
		// 	printf("\n\r");
		// 	printf("Queue2'information:\n\r");
		// 	printf("NextMsg:\t%s\n\rNumMsg:\t%d\n\rQSize:\t%d\n\r", (char *)data->OSMsg, data->OSNMsgs, data->OSQSize);
		// 	printf("\n\r");
		// }
		/* print the information about q2 */
		OSTimeDlyHMSM(0, 0, 0, 500); /* Wait 500 minisecond*/
		if (del == 0)
		{
			q = OSQDel(q2, OS_DEL_ALWAYS, &err); /* delete theq2 */
			if (q == (OS_EVENT *)0)
			{
				printf("DELETE Queue2 OK!\n\r");
			}
		}
		else
		{
			del--;
			printf("DELETE Queue2 FAILED!\n\r");
		}
	}
}
