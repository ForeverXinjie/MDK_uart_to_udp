#include <stdio.h>
#include "loopback.h"
#include "wizchip_conf.h"
#include "W7500x_gpio.h"
#include "W7500x_uart.h"

uint8_t  Sn_SendOK[8]={1,1,1,1,1,1,1,1};
uint8_t  Sn_TimeOut[8]={0,0,0,0,0,0,0,0};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Read_IR(uint8_t Sn)
{
	uint8_t  tmp;
	tmp=getSn_IR(Sn);
	setSn_IR(Sn,tmp);
	if(tmp & Sn_IR_CON)
	{
		GPIO_ResetBits(GPIOC, D_OUTPUT2);	 
	}
	if(tmp & Sn_IR_DISCON)
	{
		disconnect(Sn); 
	}
	if(tmp & Sn_IR_TIMEOUT)	 
	{
		Sn_TimeOut[Sn]=1;
		close(Sn);
	}
	if(tmp & Sn_IR_SENDOK)
	{
		Sn_SendOK[Sn]=1; 
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint8_t Detect_Gateway(void)
{
	uint8_t Sn_DIP[4] = {193, 169,  1,  31}; //Set up an External purpose network address
	uint8_t  dhar[1];

	setSn_MR(0, Sn_MR_TCP); 
	setSn_CR(0, Sn_CR_OPEN); 

	delay(10);
	if(getSn_SR(0)!=SOCK_INIT)
	{    
		close(0);
		return 0;       
	}
	 
	setSn_DIPR(0,Sn_DIP);
	setSn_CR(0,Sn_CR_CONNECT);   // Initial a TCP CONNECT 
	delay(10);
	do
	{
		if (getSn_IR(0) & Sn_IR_TIMEOUT)  //No Gateway detected
		{ 	
			setSn_IR(0, Sn_IR_TIMEOUT);
			close(0);
			return 0;
		}	
		else 
		{   
			dhar[0]= WIZCHIP_READ((WZTOE_Sn_DHAR(0)+3));
			if(dhar[0]!=0xff)       //Gateway detected 
			{    
				close(0);
				return 1;
			}
		}
	}while(1); 	 
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int32_t loopback_udps(uint8_t Sn, uint8_t* buf, uint16_t Sn_PORT)
{
	uint16_t size;
	uint8_t  Sn_DIP[4];
	uint16_t Sn_DPORT;

	switch(getSn_SR(Sn))
	{
		case SOCK_UDP :
			Read_IR(Sn);
			GPIO_ResetBits(GPIOC, D_OUTPUT2);
			size = recv_udp(Sn, buf, Sn_DIP, (uint16_t*)&Sn_DPORT);
			if(size <= 0)
			{
				return size;
			}
			while((Sn_SendOK[Sn]==0)&&(Sn_TimeOut[Sn]==0))
			{
				Read_IR(Sn);
			}
			if(Sn_SendOK[Sn]==1)
			{
				Sn_SendOK[Sn]=0;
				send_udp(Sn, buf, size, Sn_DIP, Sn_DPORT);
				UartPuts(UART0,buf);
			}
			break;
				 
			case SOCK_CLOSED:
				Sn_TimeOut[Sn]=0;
				Sn_SendOK[Sn]=1;
				GPIO_SetBits(GPIOC, D_OUTPUT2);
				socket(Sn, Sn_MR_UDP, Sn_PORT);
				break;
				 
			default :
				break;
	}
	return 1;
}

