/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.7.0
  * @date    22-April-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_cfg.h"
#include "sys_cfg.h"
#include "ucos_ii.h"
#include "task_start.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                     10u

#define  APP_TASK_START_STK_SIZE                128u

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/* stm32系统初始化配置 */
    Sys_Config();
	
	/* ucos_ii初始化 */
    OSInit();
		
	/* 创建ucos_ii初始化任务 TASK_START */
	OSTaskCreateExt(APP_TASK_START,
						 (void    *)0,
						 (OS_STK  *)&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
						 (INT8U    )APP_TASK_START_PRIO,
						 (INT16U   )APP_TASK_START_PRIO,
						 (OS_STK  *)&App_TaskStartStk[0],
						 (INT32U   )APP_TASK_START_STK_SIZE,
						 (void    *)0,
						 (INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
		
		/* 开始 ucos_ii */		
    OSStart();

    return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
