/**************************************************************************************************/
/**
 * @file     serial.c
 * @brief    Example serial file.
 * @version  V1.0.0
 * @date     December-2022
 * @author   Zhixin Semiconductor
 *
 * @note
 * Copyright (C) 2021-2023 Zhixin Semiconductor Ltd. All rights reserved.
 *
 **************************************************************************************************/
#include "serial.h"

/* UART configuration*/
 static const UART_Config_t uartConfig = 
{
    .baudRate = 115200,               /* UART baud rate: 9600 */
    .dataBits = UART_DATABITS_8,    /* UART data bits length: 8 bits */
    .parity = UART_PARITY_NONE,     /* UART parity: none */
    .stopBits = UART_STOPBITS_1,    /* UART stop bits length: 1 bit */
    .autoFlowControl = DISABLE,     /* UART flow control: disable */
    .oscFreq = 40000000             /* UART function clock freq: 40000000 */
};

 void Ex_BoardUartInit(void)
{
    /* Set and enable UART clock */
    CLK_ModuleSrc(CLK_UART1, CLK_SRC_OSC40M);
    CLK_SetClkDivider(CLK_UART1, CLK_DIV_1);
    SYSCTRL_EnableModule(SYSCTRL_UART1);
    
    /* Set and enable PORT clock */
    CLK_ModuleSrc(CLK_PORTC, CLK_SRC_OSC40M);
    SYSCTRL_EnableModule(SYSCTRL_PORTC);

    /* Set port pin for UART */
    PORT_PinmuxConfig(PORT_C, GPIO_6, PTC6_UART1_RX);
    PORT_PinmuxConfig(PORT_C, GPIO_7, PTC7_UART1_TX);

    /* Inital UART */
    UART_Init(UART1_ID, &uartConfig);

    /*Set printf not buffered*/
#if defined(__GNUC__) | defined(__ghs__)
    setbuf(stdout, NULL);
#endif
}

#if defined(__ICCARM__)
int fputc(int ch, FILE *f)
{
    /* Wait untill FIFO is empty */
    while(RESET == UART_GetLineStatus(UART1_ID, UART_LINESTA_TEMT));

    /* Send data */
    UART_SendByte(UART1_ID, (uint8_t)ch);
    return ch;
}

int fgetc(FILE *f)
{
    /* Wait untill FIFO is ready */
    while(RESET == UART_GetLineStatus(UART1_ID, UART_LINESTA_DR));

    return (int)UART_ReceiveByte(UART1_ID);
}

#elif defined(__ARMCC_VERSION)
__asm(".global __use_no_semihosting");

FILE __stdout;
FILE __stdin;
FILE __stderr;

int fputc(int ch, FILE *f)
{
    (void)(f);
    /* Wait untill FIFO is empty */
    while(RESET == UART_GetLineStatus(UART1_ID, UART_LINESTA_TEMT));

    /* Send data */
    UART_SendByte(UART1_ID, ch);
    return ch;
}

int fgetc(FILE *f)
{
    (void)(f);
    /* Wait untill FIFO is ready */
    while(RESET == UART_GetLineStatus(UART1_ID, UART_LINESTA_DR));
    return UART_ReceiveByte(UART1_ID);
}

void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}

#elif defined(__GNUC__) | defined(__ghs__)
int _write (int file, char *ptr, int len) 
{
    uint32_t idx = 0;
    int ch = 0;

    for(idx = 0; idx < len; idx++)
    {
        ch = *(ptr + idx);
        /* Wait untill FIFO is empty */
        while(RESET == UART_GetLineStatus(UART1_ID, UART_LINESTA_TEMT));
      
        /* Send data */
        UART_SendByte(UART1_ID, ch);
    }

    return len;
}
#endif
