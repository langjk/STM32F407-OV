#ifndef __UART4_H
#define __UART4_H 		
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

void uart4_init(u32 bound);
void UART4_IRQHandler(void);
void u4_printf(char* fmt,...) ;