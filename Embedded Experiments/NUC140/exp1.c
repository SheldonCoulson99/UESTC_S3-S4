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

INT32U count = 0; // Initialize count
char adc_value[12] = "Times:";
/******************************************* declarations ************************************************/

static void Task0(void *pdata); // Function prototypes of tasks
static void Task1(void *pdata);
static void Task2(void *pdata);
static void Task3(void *pdata);
static void Task4(void *pdata);

static void TaskStart(void *pdata);			// Function prototypes of Startup task
static void TaskStartCreateTasks(void); // Function prototypes of creat task

/******************************************* implementations ************************************************/
/**

 * @brief
 * main()函数，应用程序入口函数。
 * @param[in]		无。
 * @return 操作成功返回0；\n
 *         出现问题返回1；
 */

void seg_display(int16_t value)
{
	int8_t digit;
	digit = value / 1000;
	close_seven_segment();
	show_seven_segment(3, digit);
	Delay(5000);

	value = value - digit * 1000;
	digit = value / 100;
	close_seven_segment();
	show_seven_segment(2, digit);
	Delay(5000);

	value = value - digit * 100;
	digit = value / 10;
	close_seven_segment();
	show_seven_segment(1, digit);
	Delay(5000);

	value = value - digit * 10;
	digit = value;
	close_seven_segment();
	show_seven_segment(0, digit);
	Delay(5000);
}

int main(void)
{
	INT8U err = err;

	UNLOCKREG();
	SYSCLK->PWRCON.XTL12M_EN = 1; //Enable 12Mhz and set HCLK->12Mhz
	SYSCLK->CLKSEL0.HCLK_S = 0;
	LOCKREG();

	UCOS_CPU_INIT();

	OSInit(); // Initialize uC/OS-II

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
	OSTaskCreate(Task3, (void *)&TaskData[3], &TaskStk[3][TASK_STK_SIZE - 1], 8);
	OSTaskCreate(Task4, (void *)&TaskData[4], &TaskStk[4][TASK_STK_SIZE - 1], 9);
}

/**
 * @brief
 * 		Task0()函数，执行一个任务。
 * @param[in]		void *。
 * @return 无P
 */
void Task0(void *pdata)
{
	INT8U i = i;
	INT8U err = err;
	INT8U err_res = err_res;
	i = *(int *)pdata;
	for (;;)
	{
/*通过 LCD 显示*/
#if LCD == 1
		clr_all_pannal();
		print_lcd(0, "Task_0:Runing");
		print_lcd(1, "Task_1:Suspended");
		print_lcd(2, "Task switched");
		++count;
		sprintf(adc_value + 6, "%d", count);
		print_lcd(3, adc_value);
#endif
		/*通过串口显示*/
#if UART == 1
		printf("Application tasks switched %d times!\n\r", count);
		printf("TASK_0 IS RUNNING......... . . . . . . ..........................\n\r");
		printf("task_1 is suspended!\n\r");
		printf("*****************************************\n\r");
#endif
		OSTimeDly(150);
		err_res = OSTaskResume(9);
		err = OSTaskSuspend(5); // suspend itself
	}
}

/**
 * @brief
 * 		Task1()函数，执行一个任务。
 * @param[in]		void *。
 * @return 无
 */
void Task1(void *pdata)
{
	INT8U i = i;
	INT8U err = err;
	INT8U err_res = err_res;
	i = *(int *)pdata;
	for (;;)
	{
		OSTimeDly(100);
		/*通过 LCD 显示*/
#if LCD == 1
		clr_all_pannal();
		print_lcd(0, "Task_0:Suspended");
		print_lcd(1, "Task_1:Running");
		print_lcd(2, "Task switched");
		++count;
		sprintf(adc_value + 6, "%d", count);
		print_lcd(3, adc_value);
#endif
		/*通过串口显示*/
#if UART == 129
		printf("Application tasks switched %d times!\n\r", count);
		printf("task_0 is suspended!\n\r");
		printf("TASK_1 IS RUNNING.................................. . ...........\n\r");
		printf("****************************************\n\r");
#endif
		OSTimeDly(120);
		err_res = OSTaskResume(5);
		err = OSTaskSuspend(6); // suspend itself
	}
}

/**
 * @brief
 * 		Task2()函数，执行一个任务。
 * @param[in]		void *。
 * @return 无
 */
void Task2(void *pdata)
{
	INT8U i = i;
	INT8U err = err;
	INT8U err_res = err_res;
	i = *(int *)pdata;
	for (;;)
	{
		OSTimeDly(100);
		/*通过 LCD 显示*/
#if LCD == 1
		clr_all_pannal();
		print_lcd(0, "Task_1:Suspended");
		print_lcd(1, "Task_2:Running");
		print_lcd(2, "Task switched");
		++count;
		sprintf(adc_value + 6, "%d", count);
		print_lcd(3, adc_value);
#endif
		/*通过串口显示*/
#if UART == 129
		printf("Application tasks switched %d times!\n\r", count);
		printf("task_0 is suspended!\n\r");
		printf("TASK_1 IS RUNNING.................................. . ...........\n\r");
		printf("****************************************\n\r");
#endif
		OSTimeDly(120);
		err_res = OSTaskResume(6); /* resume task0 */
		err = OSTaskSuspend(7);		 // suspend itself
	}
}

/**
 * @brief
 * 		Task3()函数，执行一个任务。
 * @param[in]		void *。
 * @return 无
 */
void Task3(void *pdata)
{
	INT8U i = i;
	INT8U err = err;
	INT8U err_res = err_res;

	i = *(int *)pdata;
	for (;;)
	{
		OSTimeDly(100);
		/*通过 LCD 显示*/
#if LCD == 1
		clr_all_pannal();
		print_lcd(0, "Task_2:Suspended");
		print_lcd(1, "Task_3:Running");
		print_lcd(2, "Task switched");
		++count;
		sprintf(adc_value + 6, "%d", count);
		print_lcd(3, adc_value);
#endif
		/*通过串口显示*/
#if UART == 129
		printf("Application tasks switched %d times!\n\r", count);
		printf("task_0 is suspended!\n\r");
		printf("TASK_1 IS RUNNING.................................. . ...........\n\r");
		printf("****************************************\n\r");
#endif
		OSTimeDly(120);
		err_res = OSTaskResume(7); /* resume task0 */
		err = OSTaskSuspend(8);		 // suspend itself
	}
}

/**
 * @brief
 * 		Task4()函数，执行一个任务。
 * @param[in]		void *。
 * @return 无
 */
void Task4(void *pdata)
{
	INT8U i = i;
	INT8U err = err;
	INT8U err_res = err_res;

	i = *(int *)pdata;
	for (;;)
	{
		OSTimeDly(100);
		/*通过 LCD 显示*/
#if LCD == 1
		clr_all_pannal();
		print_lcd(0, "Task_3:Suspended");
		print_lcd(1, "Task_4:Running");
		print_lcd(2, "Task switched");
		++count;
		sprintf(adc_value + 6, "%d", count);
		print_lcd(3, adc_value);
#endif
		/*通过串口显示*/
#if UART == 129
		printf("Application tasks switched %d times!\n\r", count);
		printf("task_0 is suspended!\n\r");
		printf("TASK_1 IS RUNNING.................................. . ...........\n\r");
		printf("****************************************\n\r");
#endif
		//OSTimeDly(100);
		err_res = OSTaskResume(8); /* resume task0 */
		err = OSTaskSuspend(9);		 // suspend itself
	}
}
