/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
        BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR UART0.
*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Library includes. */
#include "gd32f4xx.h"

/* Demo application includes. */
#include "serial.h"
/*-----------------------------------------------------------*/
/* board defines. */
#define CONSOLE_COM           USART0
#define CONSOLE_COM_IRQ       USART0_IRQn
#define CONSOLE_COM_CLK       RCU_USART0

#define CONSOLE_COM_TX_PIN    GPIO_PIN_9
#define CONSOLE_COM_RX_PIN    GPIO_PIN_10

#define CONSOLE_COM_GPIO_PORT GPIOA
#define CONSOLE_COM_GPIO_CLK  RCU_GPIOA
#define CONSOLE_COM_AF        GPIO_AF_7

/* Misc defines. */
#define serINVALID_QUEUE ((QueueHandle_t)0)
#define serNO_BLOCK      ((TickType_t)0)
#define serTX_BLOCK_TIME (40 / portTICK_PERIOD_MS)

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/*-----------------------------------------------------------*/

/* UART interrupt handler. */
void vUARTInterruptHandler(void);

/*-----------------------------------------------------------*/

/*
 * See the serial2.h header file.
 */
xComPortHandle xSerialPortInitMinimal(unsigned long ulWantedBaud, unsigned portBASE_TYPE uxQueueLength)
{
    xComPortHandle xReturn;

    /* Create the queues used to hold Rx/Tx characters. */
    xRxedChars  = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char));
    xCharsForTx = xQueueCreate(uxQueueLength + 1, (unsigned portBASE_TYPE)sizeof(signed char));

    /* If the queue/semaphore was created correctly then setup the serial port
    hardware. */
    if ((xRxedChars != serINVALID_QUEUE) && (xCharsForTx != serINVALID_QUEUE)) {
        /* Enable USART1 clock */
        rcu_periph_clock_enable(CONSOLE_COM_GPIO_CLK);

        /* enable USART clock */
        rcu_periph_clock_enable(CONSOLE_COM_CLK);

        /* connect port to USARTx_Tx */
        gpio_af_set(CONSOLE_COM_GPIO_PORT, CONSOLE_COM_AF, CONSOLE_COM_TX_PIN);

        /* connect port to USARTx_Rx */
        gpio_af_set(CONSOLE_COM_GPIO_PORT, CONSOLE_COM_AF, CONSOLE_COM_RX_PIN);

        /* configure USART Tx as alternate function push-pull */
        gpio_mode_set(CONSOLE_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CONSOLE_COM_TX_PIN);
        gpio_output_options_set(CONSOLE_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CONSOLE_COM_TX_PIN);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(CONSOLE_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CONSOLE_COM_RX_PIN);
        gpio_output_options_set(CONSOLE_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CONSOLE_COM_RX_PIN);

        /* USART configure */
        usart_deinit(CONSOLE_COM);
        usart_baudrate_set(CONSOLE_COM, ulWantedBaud);
        usart_receive_config(CONSOLE_COM, USART_RECEIVE_ENABLE);
        usart_transmit_config(CONSOLE_COM, USART_TRANSMIT_ENABLE);
        usart_enable(CONSOLE_COM);

        /* USART interrupt configuration */
        nvic_irq_enable(CONSOLE_COM_IRQ, 0, 0);
        usart_interrupt_enable(CONSOLE_COM, USART_INT_RBNE);
        usart_interrupt_enable(CONSOLE_COM, USART_INT_TBE);
    } else {
        xReturn = (xComPortHandle)0;
    }

    /* This demo file only supports a single port but we have to return
    something to comply with the standard demo header file. */
    return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar(xComPortHandle pxPort, signed char *pcRxedChar, TickType_t xBlockTime)
{
    /* The port handle is not required as this driver only supports one port. */
    (void)pxPort;

    /* Get the next character from the buffer.  Return false if no characters
    are available, or arrive before xBlockTime expires. */
    if (xQueueReceive(xRxedChars, pcRxedChar, xBlockTime)) {
        return pdTRUE;
    } else {
        return pdFALSE;
    }
}
/*-----------------------------------------------------------*/

void vSerialPutString(xComPortHandle pxPort, const signed char *const pcString, unsigned short usStringLength)
{
    signed char *pxNext;

    /* A couple of parameters that this port does not use. */
    (void)usStringLength;
    (void)pxPort;

    /* NOTE: This implementation does not handle the queue being full as no
    block time is used! */

    /* The port handle is not required as this driver only supports UART1. */
    (void)pxPort;

    /* Send each character in the string, one at a time. */
    pxNext = (signed char *)pcString;
    while (*pxNext) {
        xSerialPutChar(pxPort, *pxNext, serNO_BLOCK);
        pxNext++;
    }
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar(xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime)
{
    signed portBASE_TYPE xReturn;

    if (xQueueSend(xCharsForTx, &cOutChar, xBlockTime) == pdPASS) {
        xReturn = pdPASS;
        usart_interrupt_enable(CONSOLE_COM, USART_INT_TBE);
    } else {
        xReturn = pdFAIL;
    }

    return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose(xComPortHandle xPort)
{
    /* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

void vUARTInterruptHandler(void)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char cChar;

    if (usart_interrupt_flag_get(CONSOLE_COM, USART_INT_FLAG_TBE) == SET) {
        /* The interrupt was caused by the THR becoming empty.  Are there any
        more characters to transmit? */
        if (xQueueReceiveFromISR(xCharsForTx, &cChar, &xHigherPriorityTaskWoken) == pdTRUE) {
            /* A character was retrieved from the queue so can be sent to the
            THR now. */
            usart_data_transmit(CONSOLE_COM, cChar);
        } else {
            usart_interrupt_disable(CONSOLE_COM, USART_INT_TBE);
        }
    }

    if (usart_interrupt_flag_get(CONSOLE_COM, USART_INT_FLAG_RBNE) == SET) {
        cChar = usart_data_receive(CONSOLE_COM);
        xQueueSendFromISR(xRxedChars, &cChar, &xHigherPriorityTaskWoken);
    }

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
