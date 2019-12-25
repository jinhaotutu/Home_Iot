/**
  ******************************************************************************
  * @file    sys_delay.c 
  * @author  jinhao
  * @version V1.0.0
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
#include "stm32f4xx.h"
#include "ucos_ii.h"

/* Defines --------------------------------------------------------------------*/
#define UCOS_II

/* Variables ------------------------------------------------------------------*/

				
/* Functions ------------------------------------------------------------------*/			

//us级延时时,关闭任务调度(防止打断us级延迟)
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
	OS_ERR err; 
	OSSchedLock(&err);						//UCOSIII的方式,禁止调度，防止打断us延时
#else										//否则UCOSII
	OSSchedLock();							//UCOSII的方式,禁止调度，防止打断us延时
#endif
}

//us级延时时,恢复任务调度
void delay_osschedunlock(void)
{	
#ifdef CPU_CFG_CRITICAL_METHOD   			//使用UCOSIII
	OS_ERR err; 
	OSSchedUnlock(&err);					//UCOSIII的方式,恢复调度
#else										//否则UCOSII
	OSSchedUnlock();						//UCOSII的方式,恢复调度
#endif
}

#ifdef UCOS_II 						//如果需要支持OS.

static u8  fac_us=168/8;							//us延时倍乘数			   
static u16 fac_ms=1000/OS_TICKS_PER_SEC;		//ms延时倍乘数,在os下,代表每个节拍的ms数

//延时nus
//nus:要延时的us数.	
//nus:0~204522252(最大值即2^32/fac_us@fac_us=21)	    								   
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	delay_osschedlock();					//阻止OS调度，防止打断us延时
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
	delay_osschedunlock();					//恢复OS调度											    
}

//延时nms
//nms:要延时的ms数
//nms:0~65535
void delay_ms(u16 nms)
{	
	if(OSRunning&&OSIntNesting==0)//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
	{		 
		if(nms>=fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			OSTimeDly(nms/fac_ms);			//OS延时
		}
		nms%=fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms*1000));				//普通方式延时
}

#else

void Delay_Configuration(void)  
{  
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;     //时钟源为系统时钟168MHz  
    SysTick->LOAD = 167;                             //重载值为168-1，每1us溢出一次  
}  

void delay_ms(vu32 nTime)  
{  
    nTime *= 1000;  
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;        //使能SysTick，开始计数  
    while(nTime--){  
        while((SysTick->CTRL&0X010000) == 0);        //等待COUNTFLAG标志位置1  
    }  
    SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);     //失能SysTick，停止计数  
}  
  
void delay_us(vu32 nTime)  
{  
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  
    while(nTime--){  
        while((SysTick->CTRL&0X010000) == 0);  
    }  
    SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);  
} 
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
