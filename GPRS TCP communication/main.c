#include "main.h"
#include "uart.h"

//����
#define Success 1U
#define Failure 0U

//�������
unsigned long  Time_Cont = 0;       //��ʱ��������

code char phoneNumber[] = "17719228082";		//�滻����Ҫ������绰�ĺ���
code char msg[] = "ILoveMCU.taobao.com";		//��������

code char TCPServer[] = "122.228.19.57";		//TCP��������ַ
code char Port[] = "30396";						//�˿�

unsigned int count = 0;
	

//****************************************************
//������
//****************************************************
void main()
{
    Uart_Init();

	if (sendCommand("AT+RST\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog();
	delay_ms(10);

	if (sendCommand("AT\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog();
	delay_ms(10);

	if (sendCommand("AT+CPIN?\r\n", "READY", 1000, 10) == Success);
	else errorLog();
	delay_ms(10);

	if (sendCommand("AT+CREG?\r\n", "CREG: 1", 1000, 10) == Success);
	else errorLog();
	delay_ms(10);

	if (sendCommand("AT+CGATT=1\r\n", "OK\r\n", 1000, 10) == Success);
	else errorLog();
	delay_ms(10);

	if (sendCommand("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK\r\n", 1000, 10) == Success);
	else errorLog();
	delay_ms(10);


	if (sendCommand("AT+CGACT=1,1\r\n","OK\r\n", 1000, 10) == Success);
	else errorLog();
	delay_ms(10);

	

	while(1)
	{	
	 	char xdata send_buf[100] = {0};
		memset(send_buf, 0, 100);    //���
		strcpy(send_buf, "AT+CIPSTART=\"TCP\",\"");
		strcat(send_buf, TCPServer);
		strcat(send_buf, "\",");
		strcat(send_buf, Port);
		strcat(send_buf, "\r\n");
		if (sendCommand(send_buf, "CONNECT", 10000, 5) == Success);
		else errorLog();

		//��������
		if (sendCommand("AT+CIPSEND\r\n", ">", 3000, 5) == Success);
		else errorLog();

		memset(send_buf, 0, 100);    //���
		sprintf(send_buf,"ILoveMCU.taobao.com %d\r\n",count);
		count++;
		

		if (sendCommand(send_buf, send_buf, 3000, 1) == Success);
		else errorLog();
		delay_ms(100);

		memset(send_buf, 0, 100);    //���
		send_buf[0] = 0x1a;
		if (sendCommand(send_buf, send_buf, 3000, 5) == Success);
		else errorLog();
		delay_ms(100);

		if (sendCommand("AT+CIPCLOSE\r\n", "OK\r\n", 3000, 10) == Success);
		else errorLog();

		delay_ms(1000);
	}
}

void sendMessage(char *number,char *msg)
{
	xdata char send_buf[20] = {0};
	memset(send_buf, 0, 20);    //���
	strcpy(send_buf, "AT+CMGS=\"");
	strcat(send_buf, number);
	strcat(send_buf, "\"\r\n");
	if (sendCommand(send_buf, ">", 3000, 10) == Success);
	else errorLog();

	SendString(msg);

	SendData(0x1A);
}

void phone(char *number)
{
	char send_buf[20] = {0};
	memset(send_buf, 0, 20);    //���
	strcpy(send_buf, "ATD");
	strcat(send_buf, number);
	strcat(send_buf, ";\r\n");

	if (sendCommand(send_buf, "SOUNDER", 10000, 10) == Success);
	else errorLog();
}

void errorLog()
{
	while (1)
	{
	  	if (sendCommand("AT\r\n", "OK", 100, 10) == Success)
		{
			soft_reset();
		}
		delay_ms(200);
	}
}

void soft_reset(void)	 //������������
{
   ((void (code *) (void)) 0x0000) ();
}

unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		SendString(Command); 		//����GPRSָ��

		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			delay_ms(100);
			Time_Cont += 100;
			if (strstr(Rec_Buf, Response) != NULL)
			{
				
				CLR_Buf();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	
	CLR_Buf();
	return Failure;
}

//****************************************************
//MS��ʱ����(12M�����²���)
//****************************************************
void delay_ms(unsigned int n)
{
	unsigned int  i,j;
		for(i=0;i<n;i++)
			for(j=0;j<123;j++);
}