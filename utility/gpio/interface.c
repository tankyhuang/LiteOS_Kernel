#include "interface.h"

void delay_init(void)
{
   SysTick->CTRL&=0xfffffffb;//���ƼĴ�����ѡ���ⲿʱ�Ӽ�ϵͳʱ�ӵİ˷�֮һ��HCLK/8��72M/8=9M��
}

//1us ��ʱ����
void Delay_us(u32 Nus)   
{   
SysTick->LOAD=Nus*9;          //ʱ�����    72M��Ƶ     
SysTick->CTRL|=0x01;             //��ʼ����      
while(!(SysTick->CTRL&(1<<16))); //�ȴ�ʱ�䵽��   
SysTick->CTRL=0X00000000;        //�رռ�����   
SysTick->VAL=0X00000000;         //��ռ�����        
} 

void Delayms(u32 Nms)
{
	while(Nms--)
	{
		Delay_us(1000);
	}
}

//ʹ������GPIOʱ��
void GPIOCLKInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG , ENABLE);
}

void UserLEDInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����GPIO�˿��ٶ�
	GPIO_Init(LED_GPIO , &GPIO_InitStructure);
	
	LED_SET;	
}

void ServoInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = Servo_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����GPIO�˿��ٶ�
	GPIO_Init(Servo_GPIO , &GPIO_InitStructure);
	
	Servo_SET;//Ĭ�ϸ��ߵ�λmodfied by LC 2015.09.20 12:00
}



void LEDToggle(uint16_t Led)
{
    /* ָ���ܽ������� 1��ʵ�ֶ�Ӧ��LEDָʾ��״̬ȡ��Ŀ�� */
	LED_GPIO->ODR ^= Led;
	//��Ҫ���Ч�ʣ�����ֱ�ӵ��� LEDnOBB = !LEDnOBB;
}


