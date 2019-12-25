/*
*********************************************************************************************************
* Filename      : task_start.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "app_cfg.h"
#include "sys_cfg.h"
#include "ucos_ii.h"
#include "stm32f4xx.h"
#include "task_led.h"
#include "task_wifi.h"
#include "task_temp.h"

#include "stdio.h"
#include "string.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define  APP_TASK_USER_GREEN_PRIO								 7u
#define  APP_TASK_USER_RED_PRIO									 8u
#define  APP_TASK_USER_STATUS_PRIO							     6u
#define  APP_TASK_USER_STK_SIZE									 256u

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
__align(8) static OS_STK   App_TaskGreenStk[APP_TASK_USER_STK_SIZE];
__align(8) static OS_STK   App_TaskRedStk[APP_TASK_USER_STK_SIZE];
__align(8) static OS_STK   App_TaskWifiStk[APP_TASK_USER_STK_SIZE];
__align(8) static OS_STK   App_TaskTempStk[APP_TASK_USER_STK_SIZE];
__align(8) static OS_STK   App_TaskStatusStk[APP_TASK_USER_STK_SIZE];

OS_EVENT *SYNC_SEM;
u8 err;

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void APP_TASK_STATUS(void *p_arg);


/***
 * 函数名称 : APP_TASK_START();
 *
 * 函数描述 : ucos_ii 初始化任务;
 *
 * 传递值	  : 无;
 *
 * 返回值   : 无;
 *
 **/
void APP_TASK_START(void *p_arg)
{
	OS_CPU_SR cpu_sr;
    (void)p_arg;
		
	/* 配置滴答定时器，此函数调用位置为第一个创建任务开头 */
    OS_CPU_SysTickInit();
		
#if OS_TASK_STAT_EN > 0u    
	OSStatInit();
#endif
	
	/* 关中断 */
	OS_ENTER_CRITICAL();
		
#if OS_SEM_EN > 0u    
	SYNC_SEM=OSSemCreate(0);
#endif
	
	/* 创建绿灯任务 */
    OSTaskCreateExt(APP_TASK_GREEN,
                   (void    *)0,
                   (OS_STK *)&App_TaskGreenStk[APP_TASK_USER_STK_SIZE - 1],
                   (INT8U   )APP_TASK_USER_GREEN_PRIO,
                   (INT16U  )APP_TASK_USER_GREEN_PRIO,
                   (OS_STK *)&App_TaskGreenStk[0],
                   (INT32U  )APP_TASK_USER_STK_SIZE,
                   (void   *)0,
                   (INT16U  )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
                   
	/* 创建红灯任务 */
    OSTaskCreateExt(APP_TASK_RED,
                   (void   *)0,
                   (OS_STK *)&App_TaskRedStk[APP_TASK_USER_STK_SIZE - 1],
                   (INT8U   )APP_TASK_USER_RED_PRIO,
                   (INT16U  )APP_TASK_USER_RED_PRIO,
                   (OS_STK *)&App_TaskRedStk[0],
                   (INT32U  )APP_TASK_USER_STK_SIZE,
                   (void   *)0,
                   (INT16U  )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
				   
	/* 创建WIFI任务 */
    OSTaskCreateExt(APP_TASK_WIFI,
                   (void   *)0,
                   (OS_STK *)&App_TaskWifiStk[APP_TASK_USER_STK_SIZE - 1],
                   (INT8U   )APP_TASK_USER_WIFI_PRIO,
                   (INT16U  )APP_TASK_USER_WIFI_PRIO,
                   (OS_STK *)&App_TaskWifiStk[0],
                   (INT32U  )APP_TASK_USER_STK_SIZE,
                   (void   *)0,
                   (INT16U  )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
				   
	/* 创建温湿度任务 */
    OSTaskCreateExt(APP_TASK_TEMP,
                   (void   *)0,
                   (OS_STK *)&App_TaskTempStk[APP_TASK_USER_STK_SIZE - 1],
                   (INT8U   )APP_TASK_USER_TEMP_PRIO,
                   (INT16U  )APP_TASK_USER_TEMP_PRIO,
                   (OS_STK *)&App_TaskTempStk[0],
                   (INT32U  )APP_TASK_USER_STK_SIZE,
                   (void   *)0,
                   (INT16U  )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
        
    /* 创建统计任务 */
    OSTaskCreateExt(APP_TASK_STATUS,
                   (void    *)0,
                   (OS_STK *)&App_TaskStatusStk[APP_TASK_USER_STK_SIZE - 1],
                   (INT8U   )APP_TASK_USER_STATUS_PRIO,
                   (INT16U  )APP_TASK_USER_STATUS_PRIO,
                   (OS_STK *)&App_TaskStatusStk[0],
                   (INT32U  )APP_TASK_USER_STK_SIZE,
                   (void   *)0,
                   (INT16U  )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
                  
	/* 删除任务自身 */
    OSTaskDel(OS_PRIO_SELF);
		
	/* 开中断 */
	OS_EXIT_CRITICAL();
}

#include "oled.h"
#include "ov7670test.h"

void APP_TASK_STATUS(void *p_arg)
{
    OS_STK_DATA stkdata;
	
	for (;;)
	{
        OSTaskStkChk(APP_TASK_USER_GREEN_PRIO,&stkdata);
//		OLED_ShowString(0,0,(u8*)"Tk_Green:"); 
//		OLED_ShowNum(72,0,stkdata.OSUsed,3,16);
//		OLED_ShowString(96,0,(u8*)"/");
//		OLED_ShowNum(104,0,stkdata.OSFree,3,16);
//        printf("\r\ngreen task free:%d;used:%d. \r\n",stkdata.OSFree,stkdata.OSUsed);
        
        OSTaskStkChk(APP_TASK_USER_RED_PRIO,&stkdata);
//		OLED_ShowString(0,2,(u8*)"Tk_Red:"); 
//		OLED_ShowNum(72,2,stkdata.OSUsed,3,16);
//		OLED_ShowString(96,2,(u8*)"/");
//		OLED_ShowNum(104,2,stkdata.OSFree,3,16);
//        printf("red task free:%d;used:%d. \r\n",stkdata.OSFree,stkdata.OSUsed);
        
        OSTaskStkChk(APP_TASK_USER_WIFI_PRIO,&stkdata);
		OLED_ShowString(0,0,(u8*)"Tk_Wifi:"); 
		OLED_ShowNum(72,0,stkdata.OSUsed,3,16);
		OLED_ShowString(96,0,(u8*)"/");
		OLED_ShowNum(104,0,stkdata.OSFree,3,16);
//        printf("wifi task free:%d;used:%d. \r\n",stkdata.OSFree,stkdata.OSUsed);
        
        OSTaskStkChk(APP_TASK_USER_TEMP_PRIO,&stkdata);
		OLED_ShowString(0,2,(u8*)"Tk_Temp:"); 
		OLED_ShowNum(72,2,stkdata.OSUsed,3,16);
		OLED_ShowString(96,2,(u8*)"/");
		OLED_ShowNum(104,2,stkdata.OSFree,3,16);
//        printf("temp task free:%d;used:%d. \r\n",stkdata.OSFree,stkdata.OSUsed);
        
        OSTaskStkChk(APP_TASK_USER_STATUS_PRIO,&stkdata);
		OLED_ShowString(0,4,(u8*)"Tk_Sta:"); 
		OLED_ShowNum(72,4,stkdata.OSUsed,3,16);
		OLED_ShowString(96,4,(u8*)"/");
		OLED_ShowNum(104,4,stkdata.OSFree,3,16);
//        printf("status task free:%d;used:%d. \r\n\r\n",stkdata.OSFree,stkdata.OSUsed);
        
		OLED_ShowString(16,6,(u8*)"CPU_Used:"); 
		OLED_ShowNum(88,6,OSCPUUsage,2,16);
		OLED_ShowString(104,6,(u8*)"%");
		
#ifdef OV7670_DBG_PULSE
		ShanWai_SendPulse(OSCPUUsage, 1);
#endif
		
//        printf(" uC/OS-II: V%ld.%ld%ld\r\n",OSVersion()/100,(OSVersion() % 100) /   10,(OSVersion() % 10));  
//        printf(" TickRate: %ld  \r\n",OS_TICKS_PER_SEC);  
//        printf(" CPU Usage: %ld%    \r\n",OSCPUUsage);  
//        printf(" CPU Speed: %ld MHz  \r\n",BSP_CPU_ClkFreq() / 1000000L);  
//        printf(" #Ticks: %ld  \r\n",OSTime);  
//        printf(" #CtxSw: %ld  \r\n",OSCtxSwCtr);  

//        OSTaskSuspend(APP_TASK_USER_STATUS_PRIO);
		OSTimeDly(100);
	}
}

