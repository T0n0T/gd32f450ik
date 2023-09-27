/*!
    \file    main.c
    \brief   USART printf demo

    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/
#include <stdio.h>
#include "gd32f450i_eval.h"
#include "systick.h"
#include "FreeRTOS.h"
#include "task.h"

extern void vUARTCommandConsoleStart(uint16_t usStackSize, UBaseType_t uxPriority);

void run_entry(void *p);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    /* init task */
    xTaskCreate(run_entry, "RUN", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);

    /* start scheduler */
    vTaskStartScheduler();
    return 0;
}

/*!
    \brief      initialize and run LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void run_entry(void *p)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    systick_config();
    // gd_eval_com_init(EVAL_COM0);
    vUARTCommandConsoleStart(512, 2);
    gd_eval_key_init(KEY_TAMPER, KEY_MODE_GPIO);

    // printf("\r\n ------------- FreeRTOS ------------- \r\n");

    while (1) {
        gd_eval_led_toggle(LED1);
        vTaskDelay(1000);
    }
}
