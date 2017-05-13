/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �ض���c��printf������usart�˿�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����STM32 F103-ָ���� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "bsp_usart.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "los_event.h"

#define     UART_FIFO_SIZE      1024

static SUARTPORT   sUartPorts = {0};
static PSUARTPORT  psUartPort = {0};
static CHAR rx_buf[UART_FIFO_SIZE]      = {0};

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO ����,������������
  * @param  ��
  * @retval ��
  */
  

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

    memset(&sUartPorts, 0x0, sizeof(SUARTPORT));

    psUartPort = &sUartPorts;
    
    psUartPort->ulSize = UART_FIFO_SIZE;
    psUartPort->pbyStart = (CHAR*)rx_buf;

    // Initialize cyclic buffer parameters.
    psUartPort->pbyHead = psUartPort->pbyStart;
    psUartPort->pbyTail = psUartPort->pbyStart;
    psUartPort->pbyEnd  = psUartPort->pbyStart + psUartPort->ulSize;

    // Initialize read/write counters
    psUartPort->ulToRead  = 0;
    psUartPort->ulToWrite = 0;

    psUartPort->ulTotal = 0;
    psUartPort->ulWasRead = 0;
    psUartPort->bConnected = TRUE;
    
	// �򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	// �����ж����ȼ�����
	NVIC_Configuration();
	
	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);	
	
	// ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

/*****************  ����һ���ֽ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** ����8λ������ ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ����һ���ֽ����ݵ�USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* �ȴ�������� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}

BOOL
UartReadFifo( CHAR*  pbyBuffer,
              UINT32  ulNumOfBytesToRead,
              UINT32* pulNumOfBytesRead,
              UINT32   uiTimeout)
{
    volatile SUARTPORT *psUartPort;
    BOOL blRetValue = TRUE;
    UINT32 ulActuallyRead = 0;
    UINT32 ulDataSet = 0;

    psUartPort = &sUartPorts;

    // check input parameters
    if ( (pbyBuffer == 0) || (ulNumOfBytesToRead > psUartPort->ulSize))
    {
        *pulNumOfBytesRead = 0;
        return FALSE;
    }


    //OsMutexGet(&txUartReadInfoMutex[uiPort], TX_WAIT_FOREVER);

    // there is a possibility, that some thread, which is
    // UART_DEFAULT_USER, was suspended on the mutex (see above)
    // and UART user was changed before the thread was resumed,
    // hance the thread has no rights to perform the function,
    // but just get out

    //OsIsrMask( uiUartInterrupt[uiPort] );

    // check if previously received data (data that was received since last "read"
    // operation terminated) should be ignored

    // if not all of the required data is ready
    if ( ulNumOfBytesToRead > psUartPort->ulWasRead )
    {
        //UINT32 ulActualFlags;

        // init the UART structure
        psUartPort->ulToRead = ulNumOfBytesToRead;

        //OsEventFlagsSet( &evUart[uiPort],
        //                 ~UART_EVENT_READ_COMPLETE,
        //                 TX_AND );

		//if( psUartPort->bHardwareFlowControlEnabled )
		{
			// When working in flow-control mode, interrupts may be lost (when the software FIFO
			// is full, the interrupt is ignored).
			// So, just in case an interrupt was lost, try to read from the FIFO
			// explicitly once, before starting the UART interrupts.
			//IsrUartFifo( uiUartInterrupt[uiPort], uiPort );
		}

        //OsIsrUnmask( uiUartInterrupt[uiPort] );

//

        // wait untill all of the required data is ready
        //OsEventFlagsGet( &evUart[uiPort],
        //                 UART_EVENT_READ_COMPLETE,
        //                 TX_OR_CLEAR,
        //                 &ulActualFlags,
        //                 ulWaitOption );

        //OsIsrMask( uiUartInterrupt[uiPort] );
    }

    ulDataSet = MIN(ulNumOfBytesToRead, psUartPort->ulWasRead);

        // copy the read bytes to the application buffer
        // the buffer is cyclic
        if ( (psUartPort->pbyHead + ulDataSet) > psUartPort->pbyEnd )
        {
            // copy the first part, from psUartPort->pbyHead to End
            memcpy( pbyBuffer,
                    psUartPort->pbyHead,
                    psUartPort->pbyEnd - psUartPort->pbyHead );

            ulActuallyRead = (UINT32)(psUartPort->pbyEnd - psUartPort->pbyHead);
            ulDataSet     -= psUartPort->pbyEnd - psUartPort->pbyHead;
            psUartPort->pbyHead = psUartPort->pbyStart;
        }

        memcpy( (pbyBuffer+ulActuallyRead),
                psUartPort->pbyHead,
                ulDataSet );

        ulActuallyRead += ulDataSet;

        // update the head of the cyclic buffer
        if ( (psUartPort->pbyHead + ulDataSet) < psUartPort->pbyEnd )
        {
            psUartPort->pbyHead += ulDataSet;
        }
        else
        {
            psUartPort->pbyHead = psUartPort->pbyStart +
                                  ulDataSet   -
                                  (psUartPort->pbyEnd  - psUartPort->pbyHead);
        }

        // update UART structure (read section)
        psUartPort->ulWasRead -= ulActuallyRead;
        *pulNumOfBytesRead = ulActuallyRead;
        psUartPort->ulToRead   = 0;

    // all of the requred data is supposed to be ready
    if ( ulNumOfBytesToRead <= ulActuallyRead )
    {
        blRetValue = TRUE;
    }
    else
    {
        blRetValue = FALSE;
    }

    //OsIsrUnmask( uiUartInterrupt[uiPort] );
    //OsMutexPut(&txUartReadInfoMutex[uiPort]);

    return blRetValue;
}

BOOL
UartWriteBlockFifo( UINT32 uiPort )
{
    volatile SUARTPORT *psUartPort = &sUartPorts;

    while ( (psUartPort->ulToWrite > 0) )
    {
        //UART_FifoXferByte(  uiPort, *(psUartPort->pbyOutBuffer) );
        Usart_SendByte( DEBUG_USARTx, *(psUartPort->pbyOutBuffer));
        
        psUartPort->ulToWrite--;
        psUartPort->pbyOutBuffer++;
    }

    if ( psUartPort->ulToWrite == 0 )
    {
        // Notify applicaiton that all date has been transmitted.
    }

    return TRUE;
}

BOOL
UartWriteFifo( CHAR* pbyBuffer,
               UINT32 ulNumOfBytesToWrite,
               UINT32 uiTimeout)
{
    BOOL blRetValue = TRUE;
    volatile SUARTPORT *psUartPort;
    UINT32 uiPort = 0;

    if ( pbyBuffer == NULL )
    {
        // Nothing to print
        return FALSE;
    }

    psUartPort = &sUartPorts;

    //OsMutexGet(&txUartWriteInfoMutex[uiPort], TX_WAIT_FOREVER);

    // there is a possibility, that some thread, which is
    // UART_DEFAULT_USER, was suspended on the mutex (see above)
    // and UART user was changed before the thread was resumed,
    // hence the thread has no rights to perform the function,
    // but just get out

    //OsIsrMask( uiUartInterrupt[uiPort] );

    psUartPort->pbyOutBuffer = pbyBuffer;
    psUartPort->ulToWrite    = ulNumOfBytesToWrite;

    // Write first bytes
    UartWriteBlockFifo( uiPort );

    // Check if we need to wait untill the data is transmitted.
    if ( (psUartPort->ulToWrite > 0) && (uiTimeout > 0) )
    {
        blRetValue =  ( (psUartPort->ulToWrite == 0) ? TRUE : FALSE  );
    }

    //OsIsrUnmask( uiUartInterrupt[uiPort] );
    //OsMutexPut( &txUartWriteInfoMutex[uiPort] );

    return blRetValue;
}

static VOID
UART_Add2InputQueue( UINT32 uiPort, CHAR byInput )
{
    PSUARTPORT psUartPort;

    psUartPort = &sUartPorts;

    // write the byte
    *(psUartPort->pbyTail) = byInput;

    // update pointers
    UART_IncrementQueueTail( psUartPort->pbyTail,
                             psUartPort->pbyStart,
                             psUartPort->pbyEnd   );

    // update counters
    if ( (psUartPort->pbyTail - psUartPort->pbyHead == 1)  ||
         ((psUartPort->pbyTail == psUartPort->pbyStart) &&
          (psUartPort->pbyHead == (psUartPort->pbyEnd-1))  )   )
    {
        // the buffer was full and now it contains single byte only,
        // or the buffer contains single byte only (and wasn't full before)
        psUartPort->ulWasRead = 1;
    }
    else
    {
        psUartPort->ulWasRead++;
    }
}


// �����жϷ�����
// �ѽ��յ������ݴ���һ�����黺�������棬�����յ��ĵ�ֵ����0XFFʱ����ֵ����
void DEBUG_USART_IRQHandler(void)
{	
	if(USART_GetITStatus(DEBUG_USARTx,USART_IT_RXNE) != RESET )
	{	
	    CHAR ch = 0;
        //rx_buf[num] = (char)USART_ReceiveData(DEBUG_USARTx);
        ch = (char)USART_ReceiveData(DEBUG_USARTx);
        // get next byte
        UART_Add2InputQueue( 0, ch );
		//USART_SendData(DEBUG_USARTx, rx_buf[num] ); 
	}	
    return ;
}

#define event_wait 0x00000001

EVENT_CB_S  uart_event;

void UartInit(void)
{
    UINT32 uwRet;
    
    uwRet = LOS_EventInit(&uart_event);
    if(uwRet != LOS_OK)
    {
        printf("init event failed .\n");
        return ;
    }


    printf("UartInit write event .\n");

    uwRet = LOS_EventWrite(&uart_event, event_wait);
    if(uwRet != LOS_OK)
    {
        printf("event write failed .\n");
        return ;
    }

}

