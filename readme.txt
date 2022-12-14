实验器材:
	探索者STM32F4开发板
	
实验目的:
	学习STM32F4的DCMI接口和OV5640摄像头模块的使用.
	
硬件资源:
	1,DS0(连接在PF9)
	2,串口1(波特率:115200,PA9/PA10连接在板载USB转串口芯片CH340上面)
	3,串口2（波特率：921600，PA2/PA3默认跳线帽连接到COM2_RX、COM2_TX）
	4,ALIENTEK 2.8/3.5/4.3/7寸TFTLCD模块(通过FSMC驱动,FSMC_NE4接LCD片选/A6接RS)
	5,按键KEY0(PE4)/KEY1(PE3)/KEY2(PE2)/KEY_UP(PA0,也称之为WK_UP)
	6,DCMI接口(用于驱动OV5640摄像头模块)
	7,定时器3(用于打印摄像头帧率等信息)
	8,ALIENTEK OV5640摄像头模块,连接关系为:  
		 OV5640模块 ------------ STM32开发板
		 OV_D0~D7  ------------  PE6/PE5/PB6/PC11/PC9/PC8/PC7/PC6
		 OV_SCL    ------------  PD6
		 OV_SDA    ------------  PD7
		 OV_VSYNC  ------------  PB7
		 OV_HREF   ------------  PA4
		 OV_RESET  ------------  PG15
		 OV_PCLK   ------------  PA6
		 OV_PWDN   ------------  PG9 

实验现象:
	本实验开机后，初始化摄像头模块（OV5640），如果初始化成功，则提示选择模式：RGB565模式，或者
	JPEG模式。KEY0用于选择RGB565模式，KEY1用于选择JPEG模式。
	
        当使用RGB565时，输出图像（固定为：WXGA）将经过缩放处理（完全由OV5640的DSP控制），显示在LCD
	上面。我们可以通过KEY_UP按键选择：1:1显示，即不缩放，图片不变形，但是显示区域小（液晶分辨率
	大小），或者缩放显示，即将1280*800的图像压缩到液晶分辨率尺寸显示，图片变形，但是显示了整个
	图片内容。通过KEY0按键，可以设置对比度；KEY1按键，可以自动对焦一次；KEY2按键，可以设置特效。

    	当使用JPEG模式时，图像可以设置任意尺寸（QQVGA~VGA），采集到的JPEG数据将先存放到STM32F4的内
	存里面，每当采集到一帧数据，就会关闭DMA传输，然后将采集到的数据发送到串口2（此时可以通过上位
	机软件（ATK-CAM.exe）接收，并显示图片），之后再重新启动DMA传输。我们可以通过KEY_UP设置输
	出图片的尺寸（QQVGA~VGA）。通过KEY0按键，可以设置对比度；可以启动单次自动对焦；KEY2按键，可以
	设置特效。
    
    	同时是可以通过串口1，借助USMART设置/读取OV5640的寄存器，方便大家调试。DS0指示程序运行状态，
	DS1用于指示帧中断。
   
注意事项: 
	1,4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).
	2,本例程在LCD_Init函数里面(在ILI93xx.c),用到了printf,如果不初始化串口1,将导致液晶无法显示!!  
	3,本实验需要自备ALIENTEK OV5640摄像头模块一个.
	

					正点原子@ALIENTEK
					2016-06-09
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://eboard.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					
					