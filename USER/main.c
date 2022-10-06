#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
//#include "usmart.h"  
#include "PWM.h"
#include "timer.h" 
#include "ov5640.h" 
#include "dcmi.h" 
#include "syn6288.h"


//STM32F407������ 
//����ͷ ʵ�� -�⺯���汾

extern u8 ov_frame;  					//֡��
extern uint8_t Key_Flag;      //������ֵ
u8 Com1SendFlag;              //����1�������ݱ��

u8 ovx_mode=0;							  //bit0:0,RGB565ģʽ;1,JPEGģʽ 

#define jpeg_buf_size 31*1024  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)


__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile u32 jpeg_data_len=0; 			    //buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				     //JPEG���ݲɼ���ɱ�־ 
										                   //0,����û�вɼ���;
										                   //1,���ݲɼ�����,���ǻ�û����;
										                   //2,�����Ѿ����������,���Կ�ʼ��һ֡����
//JPEG�ߴ�֧���б�
const u16 jpeg_img_size_tbl[][2]=
{
	160,120,	//QQVGA 
	320,240,	//QVGA  
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,800,	//WXGA 
	1440,900,	//WXGA+
	1280,1024,	//SXGA
	1600,1200,	//UXGA	
	1920,1080,	//1080P
	2048,1536,	//QXGA
	2592,1944,	//500W 
}; 


const u8*EFFECTS_TBL[7]={"Normal","Cool","Warm","B&W","Yellowish ","Inverse","Greenish"};	//7����Ч 
const u8*JPEG_SIZE_TBL[12]={"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};//JPEGͼƬ


//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
	if(ovx_mode&0X01)	//ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//ֹͣ��ǰ���� 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//�õ��˴����ݴ���ĳ���
				
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA_Cmd(DMA2_Stream1, ENABLE);			//���´���
			jpeg_data_ok=0;						//�������δ�ɼ�
		}
	}
	else
	{
		LCD_SetCursor(0,0);  
		LCD_WriteRAM_Prepare();		//��ʼд��GRAM
	}	
}


//JPEG����
//JPEG����,ͨ������2���͸�����.
void jpeg_test(void)
{
	u32 i,jpgstart,jpglen; 
	u8 *p;
	u8 headok=0;
	u8 effect=0,contrast=2;
	u8 size=2;			  //Ĭ����QVGA 640*480�ߴ�
	u8 msgbuf[15];		//��Ϣ������ 

  uart_init(921600);		//��ʼ�����ڲ�����Ϊ921600   ʹͨ���ٶȸ��죬PC������ܸ����ˢ��ͼƬ��̫���Ļ������쳣
	
	LCD_Clear(WHITE);
  POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"STM32F4xx");
	LCD_ShowString(30,70,200,16,16,"OV5640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,120,200,16,16,"KEY1:Saturation"); 		//ɫ�ʱ��Ͷ�
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//�ֱ������� 
	
	
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	
	LCD_ShowString(30,180,200,16,16,msgbuf);					//��ʾ��ǰJPEG�ֱ���
	
 	//�Զ��Խ���ʼ��
	OV5640_RGB565_Mode();	//RGB565ģʽ 
	OV5640_Focus_Init(); 
	
 	OV5640_JPEG_Mode();		//JPEGģʽ
	
	OV5640_Light_Mode(0);	//�Զ�ģʽ
	OV5640_Color_Saturation(3);//ɫ�ʱ��Ͷ�0
	OV5640_Brightness(4);	//����0
	OV5640_Contrast(3);		//�Աȶ�0
	OV5640_Sharpness(33);	//�Զ����
	OV5640_Focus_Constant();//���������Խ�
	
	My_DCMI_Init();			//DCMI����
	
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA����   
	
	OV5640_OutSize_Set(4,0,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ� 
	
	DCMI_Start(); 		//��������
	
	while(1)
	{
		if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
		{  
			p=(u8*)jpeg_buf;
			
						
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //��ʾ���ڴ������� 
			
			jpglen=0;	//����jpg�ļ���СΪ0
			headok=0;	//���jpgͷ���
			for(i=0;i<jpeg_data_len*4;i++)//����0XFF,0XD8��0XFF,0XD9,��ȡjpg�ļ���С
			{
				if((p[i]==0XFF)&&(p[i+1]==0XD8))//�ҵ�FF D8
				{
					jpgstart=i;
					headok=1;	//����ҵ�jpgͷ(FF D8)
				}
				if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//�ҵ�ͷ�Ժ�,����FF D9
				{
					jpglen=i-jpgstart+2;
					break;
				}
			}
			
			if(jpglen)	//������jpeg���� 
			{
				p+=jpgstart;			//ƫ�Ƶ�0XFF,0XD8�� 
				for(i=0;i<jpglen;i++)	//��������jpg�ļ�
				{
					while((USART1->SR&0X40)==0);	//ѭ������,ֱ���������   
					USART1->DR=p[i];  
  
					if(Key_Flag)
					{
						Key_Flag = 0; //�����ֵ		
					  break;
					}
				}  
			}
			
			
			if(Key_Flag)	//�а�������,��Ҫ����
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//��ʾ�˳����ݴ���
				switch(Key_Flag)
				{				    
					case KEY0_PRES:	//�Աȶ�����
						contrast++;
						if(contrast>6)contrast=0;
						OV5640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
					  Key_Flag = 0; //�����ֵ		
						break; 
					case KEY1_PRES:	//ִ��һ���Զ��Խ�
						OV5640_Focus_Single();
					  Key_Flag = 0; //�����ֵ		
						break;
					case KEY2_PRES:	//��Ч����				 
						effect++;
						if(effect>6)effect=0;
						OV5640_Special_Effects(effect);//������Ч
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
					  Key_Flag = 0; //�����ֵ		
						break;
					case WKUP_PRES:	//JPEG����ߴ�����   
						size++;  
						if(size>11)size=0;   
						OV5640_OutSize_Set(16,4,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ�  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
					  Key_Flag = 0; //�����ֵ		
						break;
				}
				
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//��ʾ��ʾ����
				delay_ms(800); 	
			  
			}
			else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//��ʾ����������� 
			
			jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
			
		}		
	}     
} 
//RGB565����
//RGB����ֱ����ʾ��LCD����
void rgb565_test(void)
{ 

	u8 effect=0,contrast=2,fac;
	u8 scale=1;		//Ĭ����ȫ�ߴ�����
	u8 msgbuf[15];	//��Ϣ������ 
	
	
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200 
	
	LCD_Clear(WHITE);
  POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"STM32F4xx");
	LCD_ShowString(30,70,200,16,16,"OV5640 RGB565 Mode");
	
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,130,200,16,16,"KEY1:Saturation"); 		//ɫ�ʱ��Ͷ�
	LCD_ShowString(30,150,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,170,200,16,16,"KEY_UP:FullSize/Scale");	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/ȫ�ߴ�����
	
		//�Զ��Խ���ʼ��
	OV5640_RGB565_Mode();	//RGB565ģʽ 
	OV5640_Focus_Init();
	
	OV5640_Light_Mode(0);	//�Զ�ģʽ
	OV5640_Color_Saturation(3);//ɫ�ʱ��Ͷ�0
	OV5640_Brightness(4);	//����0
	OV5640_Contrast(3);		//�Աȶ�0
	OV5640_Sharpness(33);	//�Զ����
	OV5640_Focus_Constant();//���������Խ�
	
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((u32)&LCD->LCD_RAM,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA����  
 	OV5640_OutSize_Set(4,0,lcddev.width,lcddev.height); 
	DCMI_Start(); 		//��������
	
	
	while(1)
	{ 
 
		if(Key_Flag)
		{ 
			if(Key_Flag!=KEY1_PRES)DCMI_Stop(); //��KEY1����,ֹͣ��ʾ
			switch(Key_Flag)
			{				    
				case KEY0_PRES:	//�Աȶ�����
					contrast++;
					if(contrast>6)contrast=0;
					OV5640_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
				  Key_Flag = 0; //�����ֵ		
					break;
				case KEY1_PRES:	//ִ��һ���Զ��Խ�
					OV5640_Focus_Single();
				  Key_Flag = 0; //�����ֵ		
					break;
				case KEY2_PRES:	//��Ч����				 
					effect++;
					if(effect>6)effect=0;
					OV5640_Special_Effects(effect);//������Ч
					sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
				  Key_Flag = 0; //�����ֵ		
					break;
				case WKUP_PRES:	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/����	    
					scale=!scale;  
					if(scale==0)
					{
						fac=800/lcddev.height;//�õ���������
 						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*lcddev.height)/2,lcddev.width,lcddev.height); 	 
						sprintf((char*)msgbuf,"Full Size 1:1");
					}else 
					{
						OV5640_OutSize_Set(4,0,lcddev.width,lcddev.height);
 						sprintf((char*)msgbuf,"Scale");
					}
					Key_Flag = 0; //�����ֵ		
					break;
			}
			if(Key_Flag!=KEY1_PRES)	//��KEY1����
			{
				LCD_ShowString(30,50,210,16,16,msgbuf);//��ʾ��ʾ����
				delay_ms(800); 
				DCMI_Start();	//���¿�ʼ����
				Key_Flag = 0; //�����ֵ		
			}
			

				
		} 
		
		delay_ms(10);
		
		if(Com1SendFlag == 1) //��ʱ1���ӷ���һ��֡����¼
        {
            printf("frame:%dfps\r\n", ov_frame);	//��ӡ֡��
            ov_frame = 0;
            Com1SendFlag = 0; //������ͱ��
        }		
	}       
}



uint8_t t;


int main(void)
{ 

u16 pwmval = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	
	//uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200   �˴����ٳ�ʼ�����ڣ���������ģʽ�ٳ�ʼ��������
	
	LED_Init();					//��ʼ��LED 
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ�� 

	usart2_init(9600);
	TIM3_Int_Init(10000 - 1, 84 - 1); //���ö�ʱ���Ķ�ʱƵ��Ϊ10ms  1�����ж�100��
	
	
	//SYN_FrameInfo(2, "[v7][m1][t5]�������ٵ�������");
 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"STM32F4xx");	
	LCD_ShowString(30,70,200,16,16,"OV5640 TEST");	
	LCD_ShowString(30,90,200,16,16,"mcudev.taobao.com");
	LCD_ShowString(30,110,200,16,16,"2017/12");  
	
//	while(OV5640_Init())//��ʼ��OV2640
//	{
//		LCD_ShowString(30,130,240,16,16,"OV5640 ERR");
//		delay_ms(200);
//	  LCD_Fill(30,130,239,170,WHITE);
//		delay_ms(200);
//		LED0=!LED0;
//	}
	LCD_ShowString(30,130,200,16,16,  "OV5640 OK");  
	TIM14_PWM_Init(200-1,8400-1);
	pwmval = 5 ;
	TIM_SetCompare1(TIM14,pwmval);
  
 	while(1)
	{	
		u16 i = 0;
		for(i = 0;i<180;i++){
			TIM_SetCompare1(TIM14,i);
			delay_ms(10);	 
		}
//		key=KEY_Scan(0);
		if(Key_Flag==KEY0_PRES)			//RGB565ģʽ
		{
			ovx_mode=0; 
      Key_Flag = 0; //�����ֵ			
			break;
		}
		else if(Key_Flag==KEY1_PRES)	//JPEGģʽ
		{
			ovx_mode=1;
			Key_Flag = 0; //�����ֵ
			break;
		}
		t++; 									  
		if(t==100)LCD_ShowString(30,150,230,16,16,"KEY0:RGB565  KEY1:JPEG"); //��˸��ʾ��ʾ��Ϣ
 		if(t==200)
		{	
			LCD_Fill(30,150,210,150+16,WHITE);
			t=0; 
		}
		delay_ms(5);	  
	}
	if(ovx_mode==1)jpeg_test();
	else rgb565_test(); 
}
