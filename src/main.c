/**************************************************************************************************/
/**
 * @file     main.c
 * @brief    Hello world example file.
 * @version  V1.0.0
 * @date     December-2022
 * @author   Zhixin Semiconductor
 *
 * @note
 * Copyright (C) 2021-2023 Zhixin Semiconductor Ltd. All rights reserved.
 *
 **************************************************************************************************/

#include "common_drv.h"
#include "wdog_drv.h"
#include "stim_drv.h"
#include "int_drv.h"
#include "clock_config.h"
#include "serial.h"

/* STIM configuration */
static const STIM_Config_t stimConfig = 
{
    .workMode = STIM_FREE_COUNT,
    /*counter clock is 40M, compare value =20000000,  period = 0.5s*/
    .compareValue = 20000000,
    .countResetMode = STIM_INCREASE_FROM_0,
    .clockSource = STIM_FUNCTION_CLOCK,
};

void Ex_LedPinsInit(void)
{
    /* Enable PORTB/PORTD module*/
    CLK_ModuleSrc(CLK_PORTB, CLK_SRC_OSC40M);
    SYSCTRL_EnableModule(SYSCTRL_PORTB);
    CLK_ModuleSrc(CLK_PORTD, CLK_SRC_OSC40M);
    SYSCTRL_EnableModule(SYSCTRL_PORTD);

    /* Enable GPIO*/
    SYSCTRL_EnableModule(SYSCTRL_GPIO);

    /* set pins as GPIO*/
    PORT_PinmuxConfig(PORT_B, GPIO_4, PTB4_GPIO);
    PORT_PinmuxConfig(PORT_B, GPIO_5, PTB5_GPIO);
    PORT_PinmuxConfig(PORT_D, GPIO_16, PTD16_GPIO);
    
    /* Set output direction for pins */
    GPIO_SetPinDir(PORT_B, GPIO_4, GPIO_OUTPUT);
    GPIO_SetPinDir(PORT_B, GPIO_5, GPIO_OUTPUT);
    GPIO_SetPinDir(PORT_D, GPIO_16, GPIO_OUTPUT);

    /* Clear output value for pins*/
    GPIO_ClearPinOutput(PORT_B, GPIO_4);
    GPIO_ClearPinOutput(PORT_B, GPIO_5);
    GPIO_ClearPinOutput(PORT_D, GPIO_16);
}

void Ex_LightBlue(void)
{
    /* set and clear output value*/
    GPIO_SetPinOutput(PORT_B, GPIO_4);
    GPIO_ClearPinOutput(PORT_B, GPIO_5);
    GPIO_ClearPinOutput(PORT_D, GPIO_16);
}

void Ex_LightRed(void)
{
    /* set and clear output value*/
    GPIO_ClearPinOutput(PORT_B, GPIO_4);
    GPIO_SetPinOutput(PORT_B, GPIO_5);
    GPIO_ClearPinOutput(PORT_D, GPIO_16);
}

void Ex_LightGreen(void)
{
    /* set and clear output value*/
    GPIO_ClearPinOutput(PORT_B, GPIO_4);
    GPIO_ClearPinOutput(PORT_B, GPIO_5);
    GPIO_SetPinOutput(PORT_D, GPIO_16);
}

static void STIM_IntCallBack(void)
{
    Ex_LightBlue();
    Ex_Print("Blue light.\n");
    COMMON_Delay(3000000);

    Ex_LightRed();
    Ex_Print("Red light.\n");
    COMMON_Delay(3000000);

    Ex_LightGreen();
    Ex_Print("Green light.\n");
    COMMON_Delay(3000000);
}

void Ex_StimInit(void)
{
    /* Configure STIM function clock*/
    CLK_ModuleSrc(CLK_STIM, CLK_SRC_OSC40M);
    CLK_SetClkDivider(CLK_STIM, CLK_DIV_1);

    /* Enable STIM module */
    SYSCTRL_EnableModule(SYSCTRL_STIM);
    
    /* Enable STIM NVIC IRQ*/
    INT_EnableIRQ(STIM_IRQn);
    
    /* Init STIM_0*/
    STIM_Init(STIM_0,&stimConfig);
    
    /* Install interrupt callback function */
    STIM_InstallCallBackFunc(STIM_0,STIM_INT,STIM_IntCallBack);
    /* Enable STIM_0 interrupt*/
    STIM_IntCmd(STIM_0, ENABLE);
    
    /*Enable STIM*/
    STIM_Enable(STIM_0);
}

int main(void)
{
    /* CLock init*/
    Ex_ClockInit();

    /* Disable wdog (wdog is enabled when default)*/
    WDOG_Disable();

    /* Serial port init*/
    Ex_BoardUartInit();

    /* Print example name*/
    Ex_Print("Hello world example.\n");

    /* Init blue/red/green led pins*/
    Ex_LedPinsInit();

    /* Init stim*/
    Ex_StimInit();

    for (;;);
}
