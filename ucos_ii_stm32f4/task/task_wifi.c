/*
*********************************************************************************************************
* Filename      : task_wifi.c
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
#include "ucos_ii.h"
#include "task_wifi.h"
#include "task_temp.h"
#include "task_start.h"

#include "stdio.h"

#include "net_device.h"
#include "onenet.h"
#include "fault.h"
#include "ov7670.h"
#include "dcmi.h"
#include "rgb2bmp.h"
#include "infrared.h"

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
extern float Temp_buf;
extern float Humi_buf;

DATA_STREAM dataStream[] = {
								{"Home_Temp", &Temp_buf, TYPE_FLOAT, 1},
								{"Home_Humi", &Humi_buf, TYPE_FLOAT, 1},
								{"Home_Infrared", &Infrared_Count, TYPE_USHORT, 1},
							};
unsigned char dataStreamCnt = sizeof(dataStream) / sizeof(dataStream[0]);

extern OS_EVENT *SYNC_SEM;
extern u8 err;
							
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/***
 * �������� : APP_TASK_GREEN();
 *
 * �������� : WIFI����;
 *
 * ����ֵ	  : ��;
 *
 * ����ֵ   : ��;
 *
 **/
void APP_TASK_WIFI(void *p_arg)
{
	/* WIFIģ��IO��ʼ������ */
    NET_DEVICE_IO_Init();
	printf("Wifi IO Init succeed.\r\n");
	
	while(1)
	{
        /************** �������� **************/
        if(faultType != FAULT_NONE)												//��������־������
		{
			printf("WARN:	Fault Process\r\n");
			Fault_Process();													//�����������
		}
		
		OSTimeDly(10);
		
		/************** ������ݷ���ƽ̨ **************/
		if(oneNetInfo.netWork == 1)
        {
            switch(oneNetInfo.sendData)
            {
                case SEND_TYPE_DATA:
					OSTaskResume(APP_TASK_USER_TEMP_PRIO);

#if OS_SEM_EN > 0u 
					OSSemPend(SYNC_SEM,0,&err);
#endif
                
                    OneNet_SendData(FORMAT_TYPE3, NULL, NULL, dataStream, dataStreamCnt);//�ϴ����ݵ�ƽ̨
					printf("\r\nOnenet Data Ready.\r\n");
                    break;
                case SEND_TYPE_HEART:
                    OneNet_SendData_Heart();		//�������
					printf("\r\nOnenet Heart Ready.\r\n");
                    break;
                case SEND_TYPE_PICTURE:
#ifdef OV7670_DBG_CAMERA
					ShanWai_SendCamera(camera_buffer+27, PIC_WIDTH, PIC_HEIGHT);
#endif
					rgb565tobmp((char *)camera_buffer, PIC_WIDTH, PIC_HEIGHT);
                    oneNetInfo.sendData = OneNet_SendData(FORMAT_TYPE2, NULL, NULL, NULL, 0);
					ov_rev_ok = 0;
					printf("\r\nOnenet Picture Ready.\r\n");
                    break;
                default:
                    break;
            }
            
            if (oneNetInfo.sendData != SEND_TYPE_OK)
            {
                if(NET_DEVICE_CheckListHead())
                {
                    //printf("Wifi Send Data Start.\r\n");
                    oneNetInfo.sendData = NET_DEVICE_SendData(NET_DEVICE_GetListHeadBuf(), NET_DEVICE_GetListHeadLen());
                    NET_DEVICE_DeleteDataSendList();
                    //printf("Wifi Send Data Ok.\r\n\r\n");
                }
            }
        }
		
        /************** ƽ̨��¼��ʼ����� **************/
        if(oneNetInfo.netWork == 0)
		{
            /*********** ��¼onenetƽ̨ ***********/
            if(!oneNetInfo.netWork && (checkInfo.NET_DEVICE_OK == DEV_OK))
			{			
				if(!NET_DEVICE_Init(oneNetInfo.protocol, oneNetInfo.ip, oneNetInfo.port))
				{
					OneNet_DevLink(oneNetInfo.devID, oneNetInfo.apiKey);
					
					if(oneNetInfo.netWork)
					{
						printf("Login in Onenet Succeed.\r\n");
					}
					else
					{
                        printf("Login in Onenet Failed.\r\n");
					}
				}
			} 
        
            /*********** ���WIFIģ��״̬ ***********/
            if(checkInfo.NET_DEVICE_OK == DEV_ERR)
            {   
                if(!NET_DEVICE_Exist())
                {
                    printf("NET Device :Ok\r\n");
                    checkInfo.NET_DEVICE_OK = DEV_OK;

					NET_DEVICE_GetWifiIP();
                }
                else
                {
                    printf("NET Device :Error\r\n");
                }
            }
        }	
	}
}
