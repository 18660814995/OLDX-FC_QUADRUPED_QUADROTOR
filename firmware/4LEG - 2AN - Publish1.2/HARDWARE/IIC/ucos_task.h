#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "stm32f4xx.h"
#include "ucos_ii.h"
#include "os_cpu.h"
#include "os_cfg.h"


extern OS_TMR   * tmr1;			//�����ʱ��1
extern OS_TMR   * tmr2;			//�����ʱ��2
extern OS_TMR   * tmr3;			//�����ʱ��3
void tmr1_callback(OS_TMR *ptmr,void *p_arg); 		  	   
void tmr2_callback(OS_TMR *ptmr,void *p_arg); 	  	   
void tmr3_callback(OS_TMR *ptmr,void *p_arg); 
	
//�����������ȼ� 0->Highest     
//10->��ʼ��������ȼ�����Ϊ���


#define FUSION_TASK_PRIO       		  1 //MEMS
#define BRAIN_TASK_PRIO       			2 //MEMS
#define UART_TASK_PRIO       			  3 //MEMS
#define POSE_FUSION_TASK_PRIO       			  4 //MEMS
//-----------------------LEG�����߳�
//���������ջ��С
#define FUSION_STK_SIZE  					64*15
//�����ջ	
extern OS_STK FUSION_TASK_STK[FUSION_STK_SIZE];
//������
void fusion_task(void *pdata);

//-----------------------LEG�����߳�
//���������ջ��С
#define POSE_FUSION_STK_SIZE  					64*100
//�����ջ	
extern OS_STK POSE_FUSION_TASK_STK[POSE_FUSION_STK_SIZE];
//������
void pose_fusion_task(void *pdata);

//-----------------------LEG�����߳�
//���������ջ��С
#define BRAIN_STK_SIZE  					64*20*12
//�����ջ	
extern OS_STK BRAIN_TASK_STK[BRAIN_STK_SIZE];
//������
void brain_task(void *pdata);

//------------------------UART�߳�
//���������ջ��С
#define UART_STK_SIZE  					64*20
//�����ջ	
extern OS_STK UART_TASK_STK[UART_STK_SIZE];
//������
void uart_task(void *pdata);
//


#endif

