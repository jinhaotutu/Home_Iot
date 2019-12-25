/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RTC驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_rtc.h"
#include "stdio.h"
#include "string.h"
#include "oled.h"
 
u8 data_flag = 0;
 
/**
  * @brief  设置时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Set(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// 初始化时间
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // 初始化日期	
	RTC_DateStructure.RTC_WeekDay = WEEKDAY;       
	RTC_DateStructure.RTC_Date = DATE;         
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  显示时间和日期
  * @param  无
  * @retval 无
  */
void RTC_TimeAndDate_Show(void)
{
//	uint8_t Rtctmp=0;
	char LCDTemp[15];
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	
//	while(1)
	{
		// 获取日历
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
			
//		if (data_flag != RTC_DateStructure.RTC_Date)
//		{
//			Infrared_Count = 0;
//			OLED_ShowNum(90,6,Infrared_Count,3,16);
//		}
		
		// 每秒打印一次
//		if(Rtctmp != RTC_TimeStructure.RTC_Seconds)
		{
							
//			// 打印日期
//			printf("20%02d-%02d-%02d--%02d\r\n",
//				RTC_DateStructure.RTC_Year,
//				RTC_DateStructure.RTC_Month,
//				RTC_DateStructure.RTC_Date,
//				RTC_DateStructure.RTC_WeekDay);
			
			//液晶显示日期
			//先把要显示的数据用sprintf函数转换为字符串，然后才能用液晶显示函数显示
			sprintf(LCDTemp,"20%02d-%02d-%02d", 
			RTC_DateStructure.RTC_Year,
			RTC_DateStructure.RTC_Month, 
			RTC_DateStructure.RTC_Date);		
			
			OLED_ShowString(20,2,(u8*)LCDTemp); 
				
//			// 打印时间
//			printf("%02d:%02d:%02d\r\n\r\n", 
//				RTC_TimeStructure.RTC_Hours, 
//				RTC_TimeStructure.RTC_Minutes, 
//				RTC_TimeStructure.RTC_Seconds);
				
			//液晶显示时间
			sprintf(LCDTemp,"%02d:%02d:%02d", 
			RTC_TimeStructure.RTC_Hours, 
			RTC_TimeStructure.RTC_Minutes, 
			RTC_TimeStructure.RTC_Seconds);
			
			OLED_ShowString(28,4,(u8*)LCDTemp); 
						
			(void)RTC->DR;
		}
	
//		Rtctmp = RTC_TimeStructure.RTC_Seconds;
		data_flag = RTC_DateStructure.RTC_Date;
	}	
}

void RTC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//RTC INT CONFIG
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;		//RTC全局中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//先占优先级1位,从优先级3位
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;			//先占优先级0位,从优先级4位
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//使能该通道中断
	NVIC_Init(&NVIC_InitStruct);
	
	//RTC WAKUP
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
void RTC_CLK_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/*使能 PWR 时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
	PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI) 
  /* 使用LSI作为RTC时钟源会有误差 
	 * 默认选择LSE作为RTC的时钟源
	 */
  /* 使能LSI */ 
  RCC_LSICmd(ENABLE);
  /* 等待LSI稳定 */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  /* 选择LSI做为RTC的时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE)

  /* 使能LSE */ 
  RCC_LSEConfig(RCC_LSE_ON);
   /* 等待LSE稳定 */   
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* 选择LSE做为RTC的时钟源 */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    

#endif /* RTC_CLOCK_SOURCE_LSI */

  /* 使能RTC时钟 */
  RCC_RTCCLKCmd(ENABLE);

  /* 等待 RTC APB 寄存器同步 */
  RTC_WaitForSynchro();
   
/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* 设置异步预分频器的值 */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* 设置同步预分频器的值 */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("RTC timer init fault.\r\n");
	}	
	
	//RTC Wakeup
	RTC_WakeUpCmd(DISABLE);//关闭WAKE UP  
  
    RTC_WakeUpClockConfig(RTC_CR_WUCKSEL_2);//唤醒时钟选择  
  
    RTC_SetWakeUpCounter(0);//设置WAKE UP自动重装载寄存器  
  
    RTC_ClearITPendingBit(RTC_IT_WUT); //清除RTC WAKE UP的标志  
    EXTI_ClearITPendingBit(EXTI_Line22);//清除LINE22上的中断标志位   
       
    RTC_ITConfig(RTC_IT_WUT,ENABLE);//开启WAKE UP 定时器中断  
    RTC_WakeUpCmd(ENABLE);//开启WAKE UP 定时器　  
}

/**
  * @brief  RTC配置：选择RTC时钟源，设置RTC_CLK的分频系数
  * @param  无
  * @retval 无
  */
#define LSE_STARTUP_TIMEOUT     ((uint16_t)0x05000)
void RTC_CLK_Config_Backup(void)
{  
  __IO uint16_t StartUpCounter = 0;
	FlagStatus LSEStatus = RESET;	
	RTC_InitTypeDef RTC_InitStructure;
	
	/* 使能 PWR 时钟 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
  PWR_BackupAccessCmd(ENABLE);
	
/*=========================选择RTC时钟源==============================*/
/* 默认使用LSE，如果LSE出故障则使用LSI */
  /* 使能LSE */
  RCC_LSEConfig(RCC_LSE_ON);	
	
	/* 等待LSE启动稳定，如果超时则退出 */
  do
  {
    LSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
    StartUpCounter++;
  }while((LSEStatus == RESET) && (StartUpCounter != LSE_STARTUP_TIMEOUT));
	
	
	if(LSEStatus == SET )
  {
		printf("\n\r LSE 启动成功 \r\n");
		/* 选择LSE作为RTC的时钟源 */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  }
	else
	{
		printf("\n\r LSE 故障，转为使用LSI \r\n");
		
		/* 使能LSI */	
		RCC_LSICmd(ENABLE);
		/* 等待LSI稳定 */ 
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{			
		}
		
		printf("\n\r LSI 启动成功 \r\n");
		/* 选择LSI作为RTC的时钟源 */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	}
	
  /* 使能 RTC 时钟 */
  RCC_RTCCLKCmd(ENABLE);
  /* 等待 RTC APB 寄存器同步 */
  RTC_WaitForSynchro();

/*=====================初始化同步/异步预分频器的值======================*/
	/* 驱动日历的时钟ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* 设置异步预分频器的值为127 */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	/* 设置同步预分频器的值为255 */
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* 用RTC_InitStructure的内容初始化RTC寄存器 */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("\n\r RTC 时钟初始化失败 \r\n");
	}	
}

void RTC_SET_DISPLAY(void)
{
	if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
	{
		/* 设置时间和日期 */
		RTC_TimeAndDate_Set();
	}
	else
	{
		/* 检查是否电源复位 */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n 发生电源复位....\r\n");
		}
		/* 检查是否外部复位 */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n 发生外部复位....\r\n");
		}

		printf("\r\n 不需要重新配置RTC....\r\n");
		
		/* 使能 PWR 时钟 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		/* PWR_CR:DBF置1，使能RTC、RTC备份寄存器和备份SRAM的访问 */
		PWR_BackupAccessCmd(ENABLE);
		/* 等待 RTC APB 寄存器同步 */
		RTC_WaitForSynchro();   
	} 
	
//	/* 显示时间和日期 */
//	RTC_TimeAndDate_Show();
}

void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//ALARM A中断?  
    {  
		if (oled_init_ok)
		{
			RTC_TimeAndDate_Show();
		}
        RTC_ClearFlag(RTC_FLAG_WUTF);//清除中断标志  
    }    
	
    EXTI_ClearITPendingBit(EXTI_Line22);    //清除中断线22的中断标志      
}

/**********************************END OF FILE*************************************/
