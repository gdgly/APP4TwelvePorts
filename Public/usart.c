/*
 * usart.c
 *
 *  Created on: 2021年1月5日
 *      Author: HHS007
 */
#include "usart.h"

void USART1_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	USART_DeInit(USART1);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //使能USART1时钟

	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10复用为USART1

	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9，PA10

	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound; //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1

	USART_Cmd(USART1, ENABLE);  //使能串口1
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //开启相关中断
	USART_ITConfig(USART1, USART_IT_ORE, ENABLE);  //开启相关中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  //串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);  //根据指定的参数初始化VIC寄存器、
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 ch;
#if SYSTEM_SUPPORT_OS  //使用UCOS操作系统
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		ch = USART_ReceiveData(USART1);  //(USART1->DR);	//读取接收到的数据
		if (USART1_Fram.InfBit.Length < (TCP_MAX_LEN - 1))
		{
			USART1_Fram.RxBuf[USART1_Fram.InfBit.Length++] = ch;
		}
		else
		{
			memset(USART1_Fram.RxBuf, 0, TCP_MAX_LEN);
			USART1_Fram.InfAll = 0;
		}
	}
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		USART1->SR; //先读SR，再读DR
		USART1->DR;

	}
	if (USART_GetITStatus(USART1, USART_IT_ORE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_ORE);
		USART_ReceiveData(USART1);
	}

#if SYSTEM_SUPPORT_OS
	OSIntExit();    	//退出中断
#endif
}