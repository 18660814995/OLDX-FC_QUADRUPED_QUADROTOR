#include "spi.h"

void SPI2_Init(void)
{	GPIO_InitTypeDef GPIO_InitStructure;
		SPI_InitTypeDef  SPI_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//??GPIOF??
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//??GPIOF??
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//??GPIOF??
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//??GPIOF??

	 //
	/*?? SPI_NRF_SPI? SCK,MISO,MOSI??,GPIOA^5,GPIOA^6,GPIOA^7 */ //pa5  pa6  pa7
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_15|GPIO_Pin_14; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOB, &GPIO_InitStructure);//???
	/*??SPI_NRF_SPI?CE??,?SPI_NRF_SPI? CSN ??:*/   //ce pc4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //ce
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz; 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//??????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_12|GPIO_Pin_13; //csn  pa4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//??????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	

	SPI_CS(NRF2401,1);
	SPI_CS(MPU9250,1);
	SPI_CS(CS_FLASH,1);
	SPI_CS(MS5611,1);
	SPI_CS(CS_LIS,1);
	
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB3??? SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB4??? SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB5??? SPI1
 
	//?????SPI????
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);//??SPI1
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//????SPI1 
       
  	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //????? 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //??? 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //????8? 
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; //????,????? 
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; //?1?????,???????? 
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; //NSS??????? 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; //8??,9MHz 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //???? 
	SPI_InitStructure.SPI_CRCPolynomial = 7; 
	SPI_Init(SPI2, &SPI_InitStructure); 
	/* Enable SPI1 */ 
	SPI_Cmd(SPI2, ENABLE);
}

u8 Spi_RW(u8 dat) 
{ 
	/* ? SPI?????????? */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
	/* ?? SPI2??????? */ 
	SPI_I2S_SendData(SPI2, dat); 
	/* ?SPI?????????? */ 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
	/* Return the byte read from the SPI bus */ 
	return SPI_I2S_ReceiveData(SPI2); 
}


void SPI_Receive(uint8_t *pData, uint16_t Size)
{
    for(uint16_t i=0; i<Size; i++)
    {
        pData[i] = Spi_RW(0);
    }
}

void SPI_CS(u8 sel,u8 set)
{

	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
	GPIO_SetBits(GPIOC, GPIO_Pin_12);
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	delay_us(10);
switch(sel)
{
	case MPU9250:
  if(set)	
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	else
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		delay_us(10);
	break;
	case NRF2401:
	if(set)	
	GPIO_SetBits(GPIOC, GPIO_Pin_12);
	else
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
		delay_us(10);
	break;
	case CS_FLASH:
	if(set)	
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
	else
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
		delay_us(10);
	break;
	case MS5611:
	if(set)	
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	else
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
		delay_us(10);
	break;
	case CS_LIS:
	if(set)	
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	else
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		delay_us(10);
	break;
}
}	