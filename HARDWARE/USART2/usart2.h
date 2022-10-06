#ifndef __USART2_H
#define __USART2_H 
#include "sys.h"
#include "stm32f4xx_conf.h"
#include "stdio.h"	 
#include <string.h>
#include <stdarg.h>
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//串口2初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/5/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//无
////////////////////////////////////////////////////////////////////////////////// 	
 
#define USART2_MAX_RECV_LEN  			200  	//定义最大接收字节数 200
#define USART2_MAX_SEND_LEN  			200  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
void USART2_SendString(u8 *DAT,u8 len);
void USART2_SendData(u8 data);
void usart2_init(u32 bound); 
uint16_t USART2_ReceiveData(USART_TypeDef* USARTx);
void USART2_IRQHandler(void);
void u2_printf(char* fmt,...);
#endif	   
















