#include "include.h" 
#include "flash.h"
#include "led_fc.h"
#include "usart_fc.h"
#include "ucos_ii.h"
#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_task.h"
#include "spi.h"
#include "bat.h"
#include "beep.h"
#include "dog.h"
#include "pwm_in.h"
#include "pwm_out.h"
#include "stm32f4xx_dma.h"
#include "pressure.h"
#include "nrf.h"
#include "icm20602.h"
#include "iic_vl53.h"
#include "vl6180x_i2c.h"
#include "vl53l0x.h"
#include "ms5611.h"
 /////////////////////////UCOSII������������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			20 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
//////////////////////////////////////////////////////////////////////////////
    
OS_EVENT * msg_key;			//���������¼���	  
OS_EVENT * q_msg;			//��Ϣ����

OS_FLAG_GRP * flags_key;	//�����ź�����
void * MsgGrp[256];			//��Ϣ���д洢��ַ,���֧��256����Ϣ
u8 CHE=11;
int main(void)
{ 
		NVIC_PriorityGroupConfig(NVIC_GROUP);//����ϵͳ�ж����ȼ�����2
		delay_init(168);  //��ʼ����ʱ����
		Initial_Timer_SYS();
	  POWER_INIT();
		RNG_Init();
		PWM_AUX_Out_Init(50); 	 
		PWM_Out_Init(50);
		LED_Init();								//LED���ܳ�ʼ��

    Beep_Init(0,84-1);
//------------------------Uart Init-------------------------------------
		Usart1_Init(115200);			//RadioLink  
		#if EN_DMA_UART1 
		MYDMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff1,SEND_BUF_SIZE1+2,1);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
		#endif
		Usart2_Init(115200);     //GPS
		#if EN_DMA_UART2
		MYDMA_Config(DMA1_Stream6,DMA_Channel_4,(u32)&USART2->DR,(u32)SendBuff2,SEND_BUF_SIZE2+2,1);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
		#endif			
		Usart4_Init(115200);			//Opticalflow  
		#if EN_DMA_UART4 
		MYDMA_Config(DMA1_Stream4,DMA_Channel_4,(u32)&UART4->DR,(u32)SendBuff4,SEND_BUF_SIZE4+2,0);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
		#endif
		Usart3_Init(115200);     //RaspberryPi
		#if EN_DMA_UART3
		MYDMA_Config(DMA1_Stream3,DMA_Channel_4,(u32)&USART3->DR,(u32)SendBuff3,SEND_BUF_SIZE3+2,2);//DMA2,STEAM7,CH4,����Ϊ����1,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
		#endif	
    Uart5_Init(100000);	//sbus
//	//-----------------DMA Init--------------------------
	#if EN_DMA_UART4 
		USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);       
		MYDMA_Enable(DMA1_Stream4,SEND_BUF_SIZE4+2);    
	#endif
	#if EN_DMA_UART2
		USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);       
		MYDMA_Enable(DMA1_Stream6,SEND_BUF_SIZE2+2);     	
	#endif
	#if EN_DMA_UART3
		USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);      
		MYDMA_Enable(DMA1_Stream3,SEND_BUF_SIZE3+2);  	
	#endif
	#if EN_DMA_UART1 
		USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);     
		MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE1+2);     
	#endif		
  Delay_ms(3000);
  LED_Init();								//LED���ܳ�ʼ��
 	SPI2_Init();
  Delay_ms(100);
	LIS3MDL_Init();
	Delay_ms(100);
	DS33_Init();
	Delay_ms(100);
	MS5611_Init();
  Adc_Init();
	Uart5_Init(100000);     //SBUS
	Delay_ms(100);
	W25QXX_Init();Delay_ms(100);		
	READ_PARM();Delay_ms(100);
	Nrf24l01_Init(MODEL_RX2,CHE);Delay_ms(100);
	Nrf24l01_Check();Delay_ms(100);
  I2c_Soft_Init_VL53();
	#if defined(LEG_USE_VL6)
	 VL6180X_Init();
	#endif
	#if defined(LEG_USE_SWITCH)
	 Init_Ground_Key();
	#endif
  #if defined(LEG_USE_AD)
	 Adc_Init();
	#endif
	//IWDG_Init(4,500*1);
	if(module.flash==1&&module.nrf==1&&module.acc_imu)
		Play_Music_Direct(START_BEEP);
	else
		Play_Music_Direct(MEMS_ERROR_BEEP);
	//---------------��ʼ��UCOSII--------------------------
	OSInit();  	 				
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	    
}
 

//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	u8 err;	    	    
	pdata = pdata; 	
	msg_key=OSMboxCreate((void*)0);		//������Ϣ����
	q_msg=OSQCreate(&MsgGrp[0],256);	//������Ϣ����
 	flags_key=OSFlagCreate(0,&err); 	//�����ź�����		  
	  
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������
	//ע��������ʱ��
	tmr1=OSTmrCreate(10,10,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0,"tmr1",&err);		//100msִ��һ��  cpuʹ����
	OSTmrStart(tmr1,&err);//����������ʱ��1				 	
	tmr2=OSTmrCreate(10,5,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr2_callback,0,"tmr2",&err);		//50msִ��һ��  LED&&MODE
	OSTmrStart(tmr2,&err);//����������ʱ��1				 	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	//ע���߳� 	
	 OSTaskCreate(brain_task,(void *)0,(OS_STK*)&BRAIN_TASK_STK[BRAIN_STK_SIZE-1],BRAIN_TASK_PRIO);//·���滮
	 OSTaskCreate(uart_task,(void *)0,(OS_STK*)&UART_TASK_STK[UART_STK_SIZE-1],UART_TASK_PRIO);
	 OSTaskCreate(fusion_task,(void *)0,(OS_STK*)&FUSION_TASK_STK[FUSION_STK_SIZE-1],FUSION_TASK_PRIO);
   OSTaskCreate(pose_fusion_task,(void *)0,(OS_STK*)&POSE_FUSION_TASK_STK[POSE_FUSION_STK_SIZE-1],POSE_FUSION_TASK_PRIO);
	//--
 	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}
   

//�ź�������������
void flags_task(void *pdata)
{	
	u16 flags;	
	u8 err;	    						 
	while(1)
	{
		flags=OSFlagPend(flags_key,0X001F,OS_FLAG_WAIT_SET_ANY,0,&err);//�ȴ��ź���
 		
		OSFlagPost(flags_key,0X001F,OS_FLAG_CLR,&err);//ȫ���ź�������
 	}
}
   		    

