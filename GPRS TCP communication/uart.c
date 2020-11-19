#include "uart.h"


#define Uart1_Buf_Max 100               //�������ݻ��泤��
u8 xdata  Rec_Buf[Uart1_Buf_Max];  //�������ݻ���
u8 point1 = 0;             //練�ָ��

void Uart_Init()					  			   
{	
	SCON=0x50; //���ڹ�����ʽ1��8λUART�������ʿɱ�  
     TH2=0xFF;           
     TL2=0xFD;    //������:115200 ����=11.0592MHz 
     RCAP2H=0xFF;   
     RCAP2L=0xFD; //16λ�Զ���װ��ֵ



/*****************/
     TCLK=1;   
     RCLK=1;   
     C_T2=0;   
     EXEN2=0; //�����ʷ�����������ʽ



/*****************/
    TR2=1 ; //��ʱ��2��ʼ

	EA   = 1;     //��ȫ���жϿ���
	ES   = 1;     //���п��ж�	
}


/*----------------------------
���ʹ�������
----------------------------*/
void SendData(unsigned char ch)
{
    SBUF = ch;                 //д���ݵ�UART���ݼĴ���
		while(TI == 0);
		TI = 0;
}

/*----------------------------
�����ַ���
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //����ַ���������־
    {
        SendData(*s++);         //���͵�ǰ�ַ�
    }
}

bit Hand(unsigned char *a)                   // ��������ʶ����
{ 
    if(strstr(Rec_Buf,a)!=NULL)
	    return 1;
	else
		return 0;
}

void CLR_Buf(void)                           // ���ڻ�������
{
	memset(Rec_Buf, 0, Uart1_Buf_Max);      //���

    point1 = 0;                    
}

void RECEIVE_DATA(void) interrupt 4 using 1         //�����ж�	  		
{    
	ES = 0;
	if (RI)
    {
      	RI = 0;                                //���RIλ
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











