/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  OV7670TEST_H
#define  OV7670TEST_H

/* Includes -------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Defines --------------------------------------------------------------------*/
#define OV7670_DBG
//#define OV7670_DBG_CAMERA
#define OV7670_DBG_PULSE

/* Types ----------------------------------------------------------------------*/


/* Variables ------------------------------------------------------------------*/


/* Functions ------------------------------------------------------------------*/
void OV7670_USART_Init(void);
void ShanWai_SendCamera(u16 *camera_buf, u16 length_w, u16 length_h);
void ShanWai_SendPulse(u16 pulse_buf, u16 size);

#endif /* OV7670TEST_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
