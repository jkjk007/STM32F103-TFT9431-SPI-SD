#include "stm32f10x.h"
#include "usart.h"
#include "ESP8266.h"

#define BUFSIZE 255
u8 ESP8266_RX_BUF[BUFSIZE];     //���ջ���,���64���ֽ�.
//����״̬
//bit7��������ɱ�־
//bit6�����յ�0x0d
//bit5~0�����յ�����Ч�ֽ���Ŀ
u8 ESP8266_RX_STA=0;       //����״̬���

void uart2_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);  
  //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

  //Usart2 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1
  
   //USART ��ʼ������
   
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
 
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}

void USART2_IRQHandler(void)                	//����2�жϷ������
	{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		//printf("%c --> %x",Res,Res);
			printf("%c",Res);
		
//		if((USART_RX_STA&0x80)==0)//����δ���
//			{
//			if(USART_RX_STA&0x40)//���յ���0x0d
//				{
//				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
//				else USART_RX_STA|=0x80;	//��������� 
//				}
//			else //��û�յ�0X0D
//				{	
//				if(Res==0x0d)USART_RX_STA|=0x40;
//				else
//					{
//					USART_RX_BUF[USART_RX_STA&0X3F]=Res ;
//					USART_RX_STA++;
//					if(USART_RX_STA>BUFSIZE)USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
//					}		 
//				}
//			}   		 
     } 
		//printf("  In U2\r\n");
} 
	
void wifi_init(u32 band){
	// init usart2 PA2,PA3
	uart2_init(band);
	printf("Test1\r\n");
	esp_write("AT+GMR\r\n");
	printf("\tTest2\r\n");
	esp_write("AT+CWLAP\r\n");
}

int esp_write(u8 *str)
{
	while(*str!='\0'){
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		USART_SendData(USART2, *str);
//		USART1->DR = (u8) *str;  
//		while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
		str++;
	}
	return 1;
}
