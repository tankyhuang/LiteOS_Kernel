/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   重定向c库printf函数到usart端口
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火STM32 F103-指南者 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "bsp_usart.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "los_event.h"
#include "stm32f10x_conf.h"

#define     UART_FIFO_SIZE      1024

static SUARTPORT   sUartPorts = {0};
static PSUARTPORT  psUartPort = {0};
static CHAR rx_buf[UART_FIFO_SIZE]      = {0};

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
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
    
	// 打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	// 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);	
	
	// 使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

/*****************  发送一个字节 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
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


// 串口中断服务函数
// 把接收到的数据存在一个数组缓冲区里面，当接收到的的值等于0XFF时，把值返回
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

