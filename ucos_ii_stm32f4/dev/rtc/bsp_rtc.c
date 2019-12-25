/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RTC����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_rtc.h"
#include "stdio.h"
#include "string.h"
#include "oled.h"
 
u8 data_flag = 0;
 
/**
  * @brief  ����ʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Set(void)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	// ��ʼ��ʱ��
	RTC_TimeStructure.RTC_H12 = RTC_H12_AMorPM;
	RTC_TimeStructure.RTC_Hours = HOURS;        
	RTC_TimeStructure.RTC_Minutes = MINUTES;      
	RTC_TimeStructure.RTC_Seconds = SECONDS;      
	RTC_SetTime(RTC_Format_BINorBCD, &RTC_TimeStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	
  // ��ʼ������	
	RTC_DateStructure.RTC_WeekDay = WEEKDAY;       
	RTC_DateStructure.RTC_Date = DATE;         
	RTC_DateStructure.RTC_Month = MONTH;         
	RTC_DateStructure.RTC_Year = YEAR;        
	RTC_SetDate(RTC_Format_BINorBCD, &RTC_DateStructure);
	RTC_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
}

/**
  * @brief  ��ʾʱ�������
  * @param  ��
  * @retval ��
  */
void RTC_TimeAndDate_Show(void)
{
//	uint8_t Rtctmp=0;
	char LCDTemp[15];
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	
//	while(1)
	{
		// ��ȡ����
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
			
//		if (data_flag != RTC_DateStructure.RTC_Date)
//		{
//			Infrared_Count = 0;
//			OLED_ShowNum(90,6,Infrared_Count,3,16);
//		}
		
		// ÿ���ӡһ��
//		if(Rtctmp != RTC_TimeStructure.RTC_Seconds)
		{
							
//			// ��ӡ����
//			printf("20%02d-%02d-%02d--%02d\r\n",
//				RTC_DateStructure.RTC_Year,
//				RTC_DateStructure.RTC_Month,
//				RTC_DateStructure.RTC_Date,
//				RTC_DateStructure.RTC_WeekDay);
			
			//Һ����ʾ����
			//�Ȱ�Ҫ��ʾ��������sprintf����ת��Ϊ�ַ�����Ȼ�������Һ����ʾ������ʾ
			sprintf(LCDTemp,"20%02d-%02d-%02d", 
			RTC_DateStructure.RTC_Year,
			RTC_DateStructure.RTC_Month, 
			RTC_DateStructure.RTC_Date);		
			
			OLED_ShowString(20,2,(u8*)LCDTemp); 
				
//			// ��ӡʱ��
//			printf("%02d:%02d:%02d\r\n\r\n", 
//				RTC_TimeStructure.RTC_Hours, 
//				RTC_TimeStructure.RTC_Minutes, 
//				RTC_TimeStructure.RTC_Seconds);
				
			//Һ����ʾʱ��
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
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;		//RTCȫ���ж�
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�1λ,�����ȼ�3λ
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;			//��ռ���ȼ�0λ,�����ȼ�4λ
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;			//ʹ�ܸ�ͨ���ж�
	NVIC_Init(&NVIC_InitStruct);
	
	//RTC WAKUP
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
}

/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ������RTC_CLK�ķ�Ƶϵ��
  * @param  ��
  * @retval ��
  */
void RTC_CLK_Config(void)
{  
	RTC_InitTypeDef RTC_InitStructure;
	
	/*ʹ�� PWR ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
	PWR_BackupAccessCmd(ENABLE);

#if defined (RTC_CLOCK_SOURCE_LSI) 
  /* ʹ��LSI��ΪRTCʱ��Դ������� 
	 * Ĭ��ѡ��LSE��ΪRTC��ʱ��Դ
	 */
  /* ʹ��LSI */ 
  RCC_LSICmd(ENABLE);
  /* �ȴ�LSI�ȶ� */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  /* ѡ��LSI��ΪRTC��ʱ��Դ */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

#elif defined (RTC_CLOCK_SOURCE_LSE)

  /* ʹ��LSE */ 
  RCC_LSEConfig(RCC_LSE_ON);
   /* �ȴ�LSE�ȶ� */   
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }
  /* ѡ��LSE��ΪRTC��ʱ��Դ */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);    

#endif /* RTC_CLOCK_SOURCE_LSI */

  /* ʹ��RTCʱ�� */
  RCC_RTCCLKCmd(ENABLE);

  /* �ȴ� RTC APB �Ĵ���ͬ�� */
  RTC_WaitForSynchro();
   
/*=====================��ʼ��ͬ��/�첽Ԥ��Ƶ����ֵ======================*/
	/* ����������ʱ��ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* �����첽Ԥ��Ƶ����ֵ */
	RTC_InitStructure.RTC_AsynchPrediv = ASYNCHPREDIV;
	/* ����ͬ��Ԥ��Ƶ����ֵ */
	RTC_InitStructure.RTC_SynchPrediv = SYNCHPREDIV;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* ��RTC_InitStructure�����ݳ�ʼ��RTC�Ĵ��� */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("RTC timer init fault.\r\n");
	}	
	
	//RTC Wakeup
	RTC_WakeUpCmd(DISABLE);//�ر�WAKE UP  
  
    RTC_WakeUpClockConfig(RTC_CR_WUCKSEL_2);//����ʱ��ѡ��  
  
    RTC_SetWakeUpCounter(0);//����WAKE UP�Զ���װ�ؼĴ���  
  
    RTC_ClearITPendingBit(RTC_IT_WUT); //���RTC WAKE UP�ı�־  
    EXTI_ClearITPendingBit(EXTI_Line22);//���LINE22�ϵ��жϱ�־λ   
       
    RTC_ITConfig(RTC_IT_WUT,ENABLE);//����WAKE UP ��ʱ���ж�  
    RTC_WakeUpCmd(ENABLE);//����WAKE UP ��ʱ����  
}

/**
  * @brief  RTC���ã�ѡ��RTCʱ��Դ������RTC_CLK�ķ�Ƶϵ��
  * @param  ��
  * @retval ��
  */
#define LSE_STARTUP_TIMEOUT     ((uint16_t)0x05000)
void RTC_CLK_Config_Backup(void)
{  
  __IO uint16_t StartUpCounter = 0;
	FlagStatus LSEStatus = RESET;	
	RTC_InitTypeDef RTC_InitStructure;
	
	/* ʹ�� PWR ʱ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  /* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
  PWR_BackupAccessCmd(ENABLE);
	
/*=========================ѡ��RTCʱ��Դ==============================*/
/* Ĭ��ʹ��LSE�����LSE��������ʹ��LSI */
  /* ʹ��LSE */
  RCC_LSEConfig(RCC_LSE_ON);	
	
	/* �ȴ�LSE�����ȶ��������ʱ���˳� */
  do
  {
    LSEStatus = RCC_GetFlagStatus(RCC_FLAG_LSERDY);
    StartUpCounter++;
  }while((LSEStatus == RESET) && (StartUpCounter != LSE_STARTUP_TIMEOUT));
	
	
	if(LSEStatus == SET )
  {
		printf("\n\r LSE �����ɹ� \r\n");
		/* ѡ��LSE��ΪRTC��ʱ��Դ */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  }
	else
	{
		printf("\n\r LSE ���ϣ�תΪʹ��LSI \r\n");
		
		/* ʹ��LSI */	
		RCC_LSICmd(ENABLE);
		/* �ȴ�LSI�ȶ� */ 
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
		{			
		}
		
		printf("\n\r LSI �����ɹ� \r\n");
		/* ѡ��LSI��ΪRTC��ʱ��Դ */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	}
	
  /* ʹ�� RTC ʱ�� */
  RCC_RTCCLKCmd(ENABLE);
  /* �ȴ� RTC APB �Ĵ���ͬ�� */
  RTC_WaitForSynchro();

/*=====================��ʼ��ͬ��/�첽Ԥ��Ƶ����ֵ======================*/
	/* ����������ʱ��ck_spare = LSE/[(255+1)*(127+1)] = 1HZ */
	
	/* �����첽Ԥ��Ƶ����ֵΪ127 */
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	/* ����ͬ��Ԥ��Ƶ����ֵΪ255 */
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;	
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	/* ��RTC_InitStructure�����ݳ�ʼ��RTC�Ĵ��� */
	if (RTC_Init(&RTC_InitStructure) == ERROR)
	{
		printf("\n\r RTC ʱ�ӳ�ʼ��ʧ�� \r\n");
	}	
}

void RTC_SET_DISPLAY(void)
{
	if (RTC_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
	{
		/* ����ʱ������� */
		RTC_TimeAndDate_Set();
	}
	else
	{
		/* ����Ƿ��Դ��λ */
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			printf("\r\n ������Դ��λ....\r\n");
		}
		/* ����Ƿ��ⲿ��λ */
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			printf("\r\n �����ⲿ��λ....\r\n");
		}

		printf("\r\n ����Ҫ��������RTC....\r\n");
		
		/* ʹ�� PWR ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		/* PWR_CR:DBF��1��ʹ��RTC��RTC���ݼĴ����ͱ���SRAM�ķ��� */
		PWR_BackupAccessCmd(ENABLE);
		/* �ȴ� RTC APB �Ĵ���ͬ�� */
		RTC_WaitForSynchro();   
	} 
	
//	/* ��ʾʱ������� */
//	RTC_TimeAndDate_Show();
}

void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//ALARM A�ж�?  
    {  
		if (oled_init_ok)
		{
			RTC_TimeAndDate_Show();
		}
        RTC_ClearFlag(RTC_FLAG_WUTF);//����жϱ�־  
    }    
	
    EXTI_ClearITPendingBit(EXTI_Line22);    //����ж���22���жϱ�־      
}

/**********************************END OF FILE*************************************/
