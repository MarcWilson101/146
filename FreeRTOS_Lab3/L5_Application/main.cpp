/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */
#include <stdio.h>
#include "tasks.hpp"
#include "utilities.h"
#include "io.hpp"
#include "uart0_min.h"  //uart0_puts("UH")
#include "LPC17xx.h"
#include "cstdint"
#include <stdlib.h>
#include <unistd.h>

inline bool CHECK_BIT(int var, int pos)
{
    return (bool)(var & (1 << pos));
}

void vTaskCode(void * pvParameters)
{
    char c = (char)((uint32_t)pvParameters);
    unsigned long ADGDR_Read;
    int channel = 1;
    int currentResult;
    while(1)
    {
        vTaskDelay(1000);
        ADGDR_Read = LPC_ADC->ADGDR;
        channel = (ADGDR_Read>>24) & 0x7;
        printf("Channel: %i ", channel);
        currentResult = (ADGDR_Read >> 4) & 0xFFF;
        printf("Result: %i\n", currentResult);
    }
}

int main(void)
{
    LPC_SC->PCONP |= (1 << 12);         //power up ADC
    LPC_ADC->ADCR |= (1 << 21);         //enable ADC

    LPC_SC->PCLKSEL0 |= (1 << 24);     //PCLK_peripheral = CLK/8 (00)
    LPC_SC->PCLKSEL0 |= (1 << 25); 

    LPC_PINCON->PINSEL1 |= (1 << 20);   //select p0.26 as adc (01)
    LPC_PINCON->PINSEL1 &= ~(1 << 21);

    LPC_ADC->ADCR |= (1 << 16);          //enable burst mode
    
    LPC_ADC->ADCR &= ~(0x7);             //clear other channels
    LPC_ADC->ADCR |= (1 << 3);                 //select channel 3 (p0.26)

    uart0_puts("Here?\n");
    /*
    unsigned long ADGDR_Read;
    int channel;
    int currentResult;
    while(1)
    {
        //vTaskDelay(1000);
        //while(!(LPC_ADC->ADGDR & (1 << 31)))
        //{
            //vTaskDelay(100);
            //printf("Not done yet\n");
            //wait
        //}
        ADGDR_Read = LPC_ADC->ADGDR;
        channel = (ADGDR_Read>>24) & 0x7;
        printf("Channel: %i ", channel);
        currentResult = (ADGDR_Read >> 4) & 0xFFF;
        printf("Result: %i\n", currentResult);
    }
    */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));
	xTaskCreate(vTaskCode, "vTaskCode", 512, ( void * ) 'A', 1, NULL );
    // Alias to vSchedulerStart();
    scheduler_start();
    return -1;
}
