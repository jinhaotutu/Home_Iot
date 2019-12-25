/*
*********************************************************************************************************
* Filename      : sys_cfg.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "stm32f4xx.h"
#include "os_cfg.h"
#include "sys_cfg.h"
#include "sys_delay.h"

#include "stdio.h"
#include "string.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/***
 * �������� : OS_CPU_SysTickInit();
 *
 * �������� : �δ�ʱ���趨;
 *
 * ����ֵ	  : ��;
 *
 * ����ֵ   : ��;
 *
 **/
void OS_CPU_SysTickInit(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	
	/* ��ȡϵͳʱ�� */
	RCC_GetClocksFreq(&rcc_clocks);
	
	/* ���õδ�ʱ���������ֵ */
	SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC);
}

/***
 * �������� : RCC_Configuration();
 *
 * �������� : RCCϵͳʱ�ӳ�ʼ������;
 *
 * ����ֵ	: ��;
 *
 * ����ֵ   : ��;
 *
 **/
static void RCC_Configuration(void)
{
    RCC_DeInit();
		
	RCC_HSEConfig(RCC_HSE_ON);
	while (RCC_WaitForHSEStartUp() != SUCCESS);
		
	/* 
	 * PLL_N = 336
	 * PLL_M = 8
	 * PLL_P = 2
	 *
	 * PLLP = ���� * PLL_N (PLL_M * PLL_P) = 8MHz * 336 / (8 * 2) = 168MHz 
	 */
	RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08){}
		
    RCC_HCLKConfig(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div4);
	RCC_PCLK2Config(RCC_HCLK_Div2);
}

/***
 * �������� : GPIO_Configuration();
 *
 * �������� : GPIO��ʼ������;
 *
 * ����ֵ	  : ��;
 *
 * ����ֵ   : ��;
 *
 **/
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	  
   
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* �ر����е� */
	GPIO_SetBits(GPIOE,GPIO_Pin_0|GPIO_Pin_1);
	

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
}

/***
 * �������� : NVIC_Configuration();
 *
 * �������� : �жϳ�ʼ������;
 *
 * ����ֵ	: ��;
 *
 * ����ֵ   : ��;
 *
 **/
static void NVIC_Configuration(void)
{
 
}


/***
 * �������� : USART_Configuration();
 *
 * �������� : ���ڳ�ʼ������;
 *
 * ����ֵ	: ��;
 *
 * ����ֵ   : ��;
 *
 **/
static void USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitStructure.USART_BaudRate = 115200;         
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1,ENABLE);
}

/***
 * �������� : Sys_Config();
 *
 * �������� : ϵͳ��ʼ������;
 *
 * ����ֵ	  : ��;
 *
 * ����ֵ   : ��;
 *
 **/
void Sys_Config(void)
{
	RCC_Configuration();
	
    GPIO_Configuration();
	
	NVIC_Configuration();
	 
	USART_Configuration();
	
	printf("\r\n\r\n***********STM32 UCOS_II System Config!***********\r\n\r\n");
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (u8) ch);
	
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET));

	return ch;
}


int fgetc(FILE *f)
{
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET));
	
	return (USART_ReceiveData(USART1));
}
