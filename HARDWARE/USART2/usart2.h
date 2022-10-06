#ifndef __USART2_H
#define __USART2_H 
#include "sys.h"
#include "stm32f4xx_conf.h"
#include "stdio.h"	 
#include <string.h>
#include <stdarg.h>
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����2��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/5/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
////////////////////////////////////////////////////////////////////////////////// 	
 
#define USART2_MAX_RECV_LEN  			200  	//�����������ֽ��� 200
#define USART2_MAX_SEND_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
void USART2_SendString(u8 *DAT,u8 len);
void USART2_SendData(u8 data);
void usart2_init(u32 bound); 
uint16_t USART2_ReceiveData(USART_TypeDef* USARTx);
void USART2_IRQHandler(void);
void u2_printf(char* fmt,...);
#endif	   
















