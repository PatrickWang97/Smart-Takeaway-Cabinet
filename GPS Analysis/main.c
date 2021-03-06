#include "main.h"
#include "delay.h"
#include "uart.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*************	功能说明	**************



******************************************/

/*************	本地常量声明	**************/
#define Success 1U
#define Failure 0U

/*************	本地变量声明	**************/
unsigned long  Time_Cont = 0;       //定时器计数器
bit ledState = LED_OFF;

unsigned int gprsBufferCount = 0;



/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/



/******************** IO配置函数 **************************/
void	GPIO_config(void)
{
	P5M0 = 0; 		//设置准双向口
	P5M1 = 0;
}

/******************** 主函数 **************************/
void main(void)
{
	GPIO_config();
	UartInit();

	SendString("Hello");
	delay_ms(200);
	clrStruct();

	initGprs();
	while(1)
	{
		parseGpsBuffer();
		printGpsBuffer();	
	}
}

void initGprs()
{
	if (sendCommand("AT\r\n", "OK", 3000, 10) == Success);
	else errorLog(1);


//      如果输入 AT+CREG? <CR>则返回+CREG: <mode>, <stat> [ ,<lac>,<ci> ]
// 注: <mode>的值共有三个选项,分别是 0 or 1 or 2,  其中0 代表关闭网络注册结果
//            码, 1 代表当网络注册状态改变时激活网络注册结果码, 2 代表激活网
// 络注册结果码同时显示区域和小区信息.
//    <stat>的返回值共有三个,分别是 0, 1, 2,3,4,5 ,  其中 0 代表没有注册网络同时
//   模块没有找到运营商, 1代注册到了本地网络, 2 代表找到运营商但没
// 有注册网络, 3 代表注册被拒绝, 4 代表未知的数据, 5代表注册在漫游
// 状态.
//    <lac>表示所属网络区域代码,十六进制格式显示,如: “ 279C”
//    <ci>表示所属网络的小区 ID,十六进制格式显示,如: “ 0EB2”  Tech-Link T&E
	if (sendCommand("AT+CPIN?\r\n", "READY", 1000, 10) == Success);
	else errorLog(2);
	delay_ms(10);

	if (sendCommand("AT+CREG?\r\n", "CREG: 1", 1000, 10) == Success);
	else errorLog(3);
	delay_ms(10);

	if (sendCommand("AT+GPS=1\r\n", "OK\r\n", 1000, 10) == Success);
	else errorLog(4);
	delay_ms(10);
}

unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	Uart3CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		SendString("\r\n---------send AT Command:---------\r\n");
		SendString(Command);

		Uart3SendString(Command); 		//发送GPRS指令

		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			delay_ms(100);
			Time_Cont += 100;
			if (strstr(Uart3_Rec_Buf, Response) != NULL)
			{
				SendString("\r\n==========receive AT Command:==========\r\n");
				SendString(Uart3_Rec_Buf); //输出接收到的信息
				Uart3CLR_Buf();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	SendString("\r\n==========receive AT Command:==========\r\n");
	SendString(Uart3_Rec_Buf);//输出接收到的信息
	Uart3CLR_Buf();
	return Failure;
}


void soft_reset(void)	 //制造重启命令
{
   ((void (code *) (void)) 0x0000) ();
}

void errorLog(int num)
{
	SendString("ERROR");
	SendData(num+0x30);
	SendString("\r\n");
	while (1)
	{
	  	if (sendCommand("AT\r\n", "OK", 100, 10) == Success)
		{
			SendString("\r\nRESET!!!!!!\r\n");
			soft_reset();
		}
		delay_ms(200);
	}
}


void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		SendString("**************\r\n");
		SendString(Save_Data.GPS_Buffer);

		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}


		}
	}
}

void printGpsBuffer()
{
	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		
		SendString("Save_Data.UTCTime = ");
		SendString(Save_Data.UTCTime);
		SendString("\r\n");

		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;
			SendString("Save_Data.latitude = ");
			SendString(Save_Data.latitude);
			SendString("\r\n");


			SendString("Save_Data.N_S = ");
			SendString(Save_Data.N_S);
			SendString("\r\n");

			SendString("Save_Data.longitude = ");
			SendString(Save_Data.longitude);
			SendString("\r\n");

			SendString("Save_Data.E_W = ");
			SendString(Save_Data.E_W);
			SendString("\r\n");

			LED = ledState;
			ledState = ~ledState;

		}
		else
		{
			SendString("GPS DATA is not usefull!\r\n");
		}
		
	}
}


void clrStruct()
{
	Save_Data.isGetData = false;
	Save_Data.isParseData = false;
	Save_Data.isUsefull = false;
	memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
	memset(Save_Data.UTCTime, 0, UTCTime_Length);
	memset(Save_Data.latitude, 0, latitude_Length);
	memset(Save_Data.N_S, 0, N_S_Length);
	memset(Save_Data.longitude, 0, longitude_Length);
	memset(Save_Data.E_W, 0, E_W_Length);
	
}
