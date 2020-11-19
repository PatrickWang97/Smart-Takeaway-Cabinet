#include "uart.h"


#define Uart1_Buf_Max 100               //串口数据缓存长度
u8 xdata  Rec_Buf[Uart1_Buf_Max];  //串口数据缓存
u8 point1 = 0;             //绶存指针

void Uart_Init()					  			   
{	
	SCON=0x50; //串口工作方式1，8位UART，波特率可变  
     TH2=0xFF;           
     TL2=0xFD;    //波特率:115200 晶振=11.0592MHz 
     RCAP2H=0xFF;   
     RCAP2L=0xFD; //16位自动再装入值



/*****************/
     TCLK=1;   
     RCLK=1;   
     C_T2=0;   
     EXEN2=0; //波特率发生器工作方式



/*****************/
    TR2=1 ; //定时器2开始

	EA   = 1;     //打开全局中断控制
	ES   = 1;     //串行口中断	
}


/*----------------------------
发送串口数据
----------------------------*/
void SendData(unsigned char ch)
{
    SBUF = ch;                 //写数据到UART数据寄存器
		while(TI == 0);
		TI = 0;
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}

bit Hand(unsigned char *a)                   // 串口命令识别函数
{ 
    if(strstr(Rec_Buf,a)!=NULL)
	    return 1;
	else
		return 0;
}

void CLR_Buf(void)                           // 串口缓存清理
{
	memset(Rec_Buf, 0, Uart1_Buf_Max);      //清空

    point1 = 0;                    
}

void RECEIVE_DATA(void) interrupt 4 using 1         //串口中断	  		
{    
	ES = 0;
	if (RI)
    {
      	RI = 0;                                //清除RI位
		Rec_Buf[point1] = SBUF; 
//		if (Rec_Buf[0] == 0xd9)
//			{
//				IAP_CONTR = 0x60;
//			}
			
		point1++;               
		if(point1>=Uart1_Buf_Max)          
		{
			point1 = 0;
		}           

    }
	ES =  1;

}











