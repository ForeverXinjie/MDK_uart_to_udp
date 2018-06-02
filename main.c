/* Include ------------------------------------------------------------------*/
#include <stdio.h>
#include "W7500x_crg.h"
#include "W7500x_gpio.h"
#include "W7500x_uart.h"
#include "W7500x_dma.h"
#include "W7500x_wztoe.h"
#include "loopback.h"
#include "W7500x_miim.h"
#include "string.h"
#include "my_dac.h"
/* Private typedef -----------------------------------------------------------*/
#define INPUT_x			GPIOC
#define INPUT_pad		PAD_PC
#define INPUT1_pin		GPIO_Pin_12
#define INPUT2_pin		GPIO_Pin_13

#define input1	GPIO_ReadInputDataBit(INPUT_x,INPUT1_pin)
#define input2	GPIO_ReadInputDataBit(INPUT_x,INPUT2_pin)

UART_InitTypeDef UART_InitStructure;
My_DMA_InitTypeDef my_dma_InitStructure;


/* Private function prototypes -----------------------------------------------*/
void delay(__IO uint32_t milliseconds); //Notice: used ioLibray
void TimingDelay_Decrement(void);
void Network_Properties(void);
void System_Initialization(void); 
void LED_Configuration(void);
void UART0_init(uint32_t buad);
void PLL_out_set(uint8_t M,uint8_t N,uint8_t OD);
void Input_init(void);
void UartPuts_CAM(UART_TypeDef* UARTx, uint8_t *str); 
void Cam_cmdput(void);

/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
uint8_t  test_buf[4096];
uint8_t  USART_RX_BUF[2048];
uint32_t uart0_rx_cnt=0;
uint8_t  CAM_Snap[9] 		= {0x81,0x01,0x04,0x24,0x0A,0x00,0x00,0x01,0xFF};
uint8_t  CAM_RecordStar[9] 	= {0x81,0x01,0x04,0x24,0x0A,0x00,0x00,0x02,0xFF};
uint8_t  CAM_RecordStop[9] 	= {0x81,0x01,0x04,0x24,0x0A,0x00,0x00,0x03,0xFF};
uint8_t  uart_test[12] = {"Hello World!"};

int main()
{
	uint8_t  Sn_DIP[4]={192,168,1,189};
	System_Initialization(); 
	Input_init();	
	Network_Properties();
//	dma_data_struct_init();
//    dma_init();
	
	
	//Detect_Gateway
	if(Detect_Gateway()==1)
	{
		GPIO_ResetBits(GPIOC, D_OUTPUT1);	 
	}
	
	while(1)
	{  
		loopback_udps(0, test_buf, 5000);        // Socket 0 is set to the UDP mode, set the port to 5000
		
		if(USART_RX_BUF[0]!=0)
		{
			UartPuts(UART0,USART_RX_BUF);
			send_udp(0, USART_RX_BUF, uart0_rx_cnt, Sn_DIP, 59979);
			uart0_rx_cnt = 0;
			memset(USART_RX_BUF,0,2048);
		}
		
		if((input1==0))
		{
			delay(100);
			if(input1 == 0)
			{
				my_dma_config(2,&my_dma_InitStructure,(uint32_t)uart_test,(uint32_t)&UART0->DR,12);
				dma_enable(2);
			}
				while(input1==0);
		}
		
//		Cam_cmdput();
	}

}



/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void delay(__IO uint32_t milliseconds)
{
  TimingDelay = milliseconds;
  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if(TimingDelay != 0x00)
  { 
		TimingDelay--;
  }
}


void System_Initialization(void)
{
	/* External Clock */
	CRG_PLL_InputFrequencySelect(CRG_OCLK);
	/* Set Systme init */
	SystemInit();
	/* Set PLL output frequency  Fout = Fin * M/N * 1/OD  */
	PLL_out_set(6,1,0);		//Fout = 8*6=48MHz
	
	LED_Configuration();
	/* Configure UART2 */
	S_UART_Init(115200);
	/* Configure UART0 */
	UART0_init(921600);
	/* SysTick_Config */
	SysTick_Config((GetSystemClock()/1000));
	
//	dma_data_struct_init();
//	dma_init();
}

void UART0_init(uint32_t buad)
{
	UART_StructInit(&UART_InitStructure,buad);
	UART_Init(UART0,&UART_InitStructure);
	UART_ITConfig(UART0, UART_IT_FLAG_RXI,ENABLE);
	NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);
}

void Network_Properties()
{
	uint8_t MAC_ADDR[6] = {0x48, 0x53, 0x00, 0x57, 0x55, 0x00}; //MAC address 
	uint8_t SIP_ADDR[4] = {192, 168,  1,  20};                  //Source IP
	uint8_t GW_ADDR[4]  = {192, 168,  1,  1};                   //The gateway address
	uint8_t SUB_ADDR[4] = {255, 255, 255,  0}; 
	
	/* mdio Init */
	mdio_init(GPIOB, MDC, MDIO);
	
	/* PHY Link Check via gpio mdio */
	while( link() == 0x0)
	{
		delay(500);
	}
	
	/* Network Configuration */
	setSHAR(MAC_ADDR);
	setSIPR(SIP_ADDR);
	setGAR(GW_ADDR);
	setSUBR(SUB_ADDR);
}


void LED_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*GPIO Configuration for D_OUTPUT1 LED,D_OUTPUT2 LED */
	PAD_AFConfig(PAD_PC,D_OUTPUT1,PAD_AF1); //< PAD Config - D_OUTPUT1 LED used 2nd Function
	PAD_AFConfig(PAD_PC,D_OUTPUT2,PAD_AF1); //< PAD Config - D_OUTPUT2 LED used 2nd Function	 
    
	GPIO_InitStructure.GPIO_Pin = D_OUTPUT1 | D_OUTPUT2; //< Connecting GPIO_Pin_8(D_OUTPUT1),9(D_OUTPUT2)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ; ///< Set to GPIO Mode to Output Port
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, D_OUTPUT1);
	GPIO_SetBits(GPIOC, D_OUTPUT2);	 	
}

void PLL_out_set(uint8_t M,uint8_t N,uint8_t OD)
{
	uint32_t temp=0;
	temp |= M<<16;
	temp |= N<<8;
	temp |= OD;
	CRG->PLL_FCR = 0;
	CRG->PLL_FCR |= temp; 
}

void Input_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	PAD_AFConfig(INPUT_pad,INPUT1_pin,PAD_AF1); 
	PAD_AFConfig(INPUT_pad,INPUT2_pin,PAD_AF1);  
    
	GPIO_InitStructure.GPIO_Pin = INPUT1_pin | INPUT2_pin; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	GPIO_Init(INPUT_x, &GPIO_InitStructure);
	GPIO_SetBits(INPUT_x, INPUT1_pin);
	GPIO_SetBits(INPUT_x, INPUT2_pin);	
}

void UartPuts_CAM(UART_TypeDef* UARTx, uint8_t *str)
{
    uint8_t ch;

    do{
        ch = *str;
        UartPutc(UARTx, ch);
        *str++;
    }while(ch != 0xFF);
	
}

void Cam_cmdput(void)
{
	static uint8_t  input2_flag=0;
	if((input1==0)||(input2==0))
	{
		delay(100);
		if(input1 == 0)
			UartPuts_CAM(UART0,CAM_Snap);
			while(input1==0);
		if(input2 == 0)
		{
			input2_flag = !input2_flag;
			while(input2 == 0);
			if(input2_flag==1)
			{
				UartPuts_CAM(UART0,CAM_RecordStar);
				
			}
			else
			{
				UartPuts_CAM(UART0,CAM_RecordStop);
			}
		}
	}
	
}

