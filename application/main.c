#include "gd32f4xx.h"
#include "gd32f450i_eval.h"
#include <stdio.h>
#include "systick.h"

// 定义跳转地址宏，编译时传入
#ifndef JUMP_ADDRESS
#define JUMP_ADDRESS 0x08008000 // 默认跳转地址，可根据实际情况修改
#endif

void main(void)
{
    typedef void (*app_func_t)(void);
    uint32_t app_addr   = JUMP_ADDRESS;
    uint32_t stk_addr   = *((__IO uint32_t *)app_addr);
    app_func_t app_func = (app_func_t)(*((__IO uint32_t *)(app_addr + 4)));

    if ((((uint32_t)app_func & 0xff000000) != 0x08000000) || (((stk_addr & 0x2ff00000) != 0x20000000) && ((stk_addr & 0x2ff00000) != 0x24000000))) {
        printf("No legitimate application.\r\n");
        return;
    }

    printf("Jump to application running ... \r\n");
    delay_1ms(20);

    // __disable_irq();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    for (int i = 0; i < 128; i++) {
        NVIC_DisableIRQ((IRQn_Type)i);
        NVIC_ClearPendingIRQ((IRQn_Type)i);
    }

    __set_CONTROL(0);
    __set_MSP(stk_addr);

    app_func(); // Jump to application running

    printf("Qboot jump to application fail.\r\n");
    while (1) {
        // 如果跳转失败，可以在这里添加错误处理或指示
    }
}

#if defined(__GNUC__)
ssize_t _write(int fd, char *ptr, int len)
{
    int i = 0;

    /*
     * write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
     * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2) {
        return -1;
    }

    while (*ptr && (i < len)) {
        usart_data_transmit(EVAL_COM0, *(uint8_t *)ptr);
        while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
        if (*ptr == '\r') {
            usart_data_transmit(EVAL_COM0, '\n');
            while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
        }

        i++;
        ptr++;
    }

    return i;
}
#endif