#include "sys.h"
#include "usart2.h"	
#include "stdio.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//Èç¹ûÊ¹ÓÃucos,Ôò°üÀ¨ÏÂÃæµÄÍ·ÎÄ¼ş¼´¿É.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos Ê¹ÓÃ	  
#endif
//////////////////////////////////////////////////////////////////////////////////	   
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32F407¿ª·¢°å
//´®¿Ú2³õÊ¼»¯ 
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//ĞŞ¸ÄÈÕÆÚ:2014/5/14
//°æ±¾£ºV1.0
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved
//********************************************************************************
//ĞŞ¸ÄËµÃ÷
//ÎŞ
////////////////////////////////////////////////////////////////////////////////// 	

//³õÊ¼»¯IO ´®¿Ú2
//pclk1:PCLK1Ê±ÖÓÆµÂÊ(Mhz)
//bound:²¨ÌØÂÊ ¡
//¼ÓÈëÒÔÏÂ´úÂë,Ö§³Öprintfº¯Êı,¶ø²»ĞèÒªÑ¡Ôñuse MicroLIB	  


u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//½ÓÊÕ»º³å,×î´óUART4_MAX_RECV_LEN¸ö×Ö½Ú.
u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			//·¢ËÍ»º³å,×î´óUART4_MAX_SEND_LEN×Ö½Ú

vu16 USART2_RX_STA=0; 

int fgetc(FILE *f)
{
		/* µÈ´ı´®¿ÚÊäÈëÊı¾İ */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
 
		return (int)USART_ReceiveData(USART1);
}

void USART2_SendData(u8 data)
{
	while((USART2->SR & 0X40) == 0);
	USART2->DR = data;
}

void USART2_SendString(u8 *DAT, u8 len)
{
	u8 i;
	for(i = 0; i < len; i++)
	{
		USART2_SendData(*DAT++);
	}
}

uint16_t USART2_ReceiveData(USART_TypeDef* USARTx)
{
  /* Check the parameters */
  assert_param(IS_USART_ALL_PERIPH(USARTx));
  
  /* Receive Data */
  return (uint16_t)(USARTx->DR & (uint16_t)0x01FF);
}

void USART2_IRQHandler(void)                	//´®¿Ú1ÖĞ¶Ï·şÎñ³ÌĞò
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//Èç¹ûSYSTEM_SUPPORT_OSÎªÕæ£¬ÔòĞèÒªÖ§³ÖOS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //½ÓÊÕÖĞ¶Ï(½ÓÊÕµ½µÄÊı¾İ±ØĞëÊÇ0x0d 0x0a½áÎ²)
	{
		Res =USART_ReceiveData(USART2);//(USART3->DR);	//¶ÁÈ¡½ÓÊÕµ½µÄÊı¾İ
			USART2_SendData(Res-0x30);
		if(Res <= 0x39 && Res >= 0x30){
			TIM_SetCompare1(TIM14,((Res-0x30)*10));
		}
		if((USART2_RX_STA&0x8000)==0)//½ÓÊÕÎ´Íê³É
		{
			if(USART2_RX_STA&0x4000)//½ÓÊÕµ½ÁË0x0d
			{
				if(Res!=0x0a)USART2_RX_STA=0;//½ÓÊÕ´íÎó,ÖØĞÂ¿ªÊ¼
				else USART2_RX_STA|=0x8000;	//½ÓÊÕÍê³ÉÁË 
			}
			else //»¹Ã»ÊÕµ½0X0D
			{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
				{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_MAX_RECV_LEN-1))USART2_RX_STA=0;//½ÓÊÕÊı¾İ´íÎó,ÖØĞÂ¿ªÊ¼½ÓÊÕ	  
				}		 
			}
		}   		 
  } 
}

void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//´Ë´Î·¢ËÍÊı¾İµÄ³¤¶È
	for(j=0;j<i;j++)							//Ñ­»··¢ËÍÊı¾İ
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
		USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}

void usart2_init(u32 bound)
{ 
	

	
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //Ê¹ÄÜGPIOAÊ±ÖÓ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//Ê¹ÄÜUSART2Ê±ÖÓ

	//USART1_TX   PA.2 PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2,PA3,¸´ÓÃ¹¦ÄÜ,ÉÏÀ­Êä³ö
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//¸´ÓÃ¹¦ÄÜ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//ËÙ¶È50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍì¸´ÓÃÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //ÉÏÀ­
	GPIO_Init(GPIOA,&GPIO_InitStructure); //³õÊ¼»¯PA2£¬PA3
	
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2¸´ÓÃÎªUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3¸´ÓÃÎªUSART2
  
	USART_InitStructure.USART_BaudRate = bound;//Ò»°ãÉèÖÃÎª9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
	USART_InitStructure.USART_Parity = USART_Parity_No;//ÎŞÆæÅ¼Ğ£ÑéÎ»
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	USART_InitStructure.USART_Mode =   USART_Mode_Rx | USART_Mode_Tx;	// ·¢Ä£Ê½
  USART_Init(USART2, &USART_InitStructure); //³õÊ¼»¯´®¿Ú
  USART_Cmd(USART2, ENABLE);  //Ê¹ÄÜ´®¿Ú 
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	#if EN_USART2_RX	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//¿ªÆôÏà¹ØÖĞ¶Ï

	//Usart1 NVIC ÅäÖÃ
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//ÇÀÕ¼ÓÅÏÈ¼¶0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//×ÓÓÅÏÈ¼¶0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾İÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯VIC¼Ä´æÆ÷

#endif
}






