#include <stdint.h>
#include <string.h>
#include "stdio.h"
#include "gd32f4xx.h"
#include "gd32f450i_eval.h"
#include "platform.h"
#ifdef DEBUG
#include "cm_backtrace.h"
#endif

char __attribute__((section(".noinit"))) _main_stack[1024];
char __attribute__((section(".noinit"))) _interrupt_stacks[1024];

// vector
extern char _vector_start[];

// bss
extern char __bss_start[];
extern char __bss_end[];

// data
extern char __data_region_start[];
extern char __data_region_end[];
extern char __data_start[];
extern char __data_end[];

#define VECTOR_ADDRESS ((uintptr_t)_vector_start)
#define VTOR_MASK      SCB_VTOR_TBLOFF_Msk

void _arm_init_arch_hw_at_boot(void)
{
    __disable_irq();
    __set_FAULTMASK(0);
    /* Disable NVIC interrupts */
    for (uint8_t i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;
    }
    /* Clear pending NVIC interrupts */
    for (uint8_t i = 0; i < sizeof(NVIC->ICPR) / sizeof(NVIC->ICER[0]); i++) {
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }
    __enable_irq();

    __DSB();
    __ISB();
}

void _bss_zero(void)
{
    memset(__bss_start, 0, __bss_end - __bss_start);
}

void _data_copy(void)
{
    memcpy(__data_start, __data_region_start, __data_end - __data_start);
}

void _cstart(void)
{
    __set_MSP((uint32_t)&_main_stack[1024]);

    NVIC_SetPriority(PendSV_IRQn, 0xff);
    NVIC_SetPriority(SVCall_IRQn, 1);
    NVIC_SetPriority(MemoryManagement_IRQn, 0);
    NVIC_SetPriority(BusFault_IRQn, 0);
    NVIC_SetPriority(UsageFault_IRQn, 0);
    NVIC_SetPriority(DebugMonitor_IRQn, 0);
    SCB->SHCSR |=
        SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk;
    NVIC_SetPriority(SysTick_IRQn, _EXC_IRQ_DEFAULT_PRIO);

    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk | SCB_CCR_UNALIGN_TRP_Msk | SCB_CCR_STKALIGN_Msk;
    SCB->SCR = SCB_SCR_SEVONPEND_Msk;

    SCB->CFSR = SCB_CFSR_USGFAULTSR_Msk | SCB_CFSR_MEMFAULTSR_Msk | SCB_CFSR_BUSFAULTSR_Msk;
    SCB->HFSR = 0xffffffff;

    gd_eval_com_init(EVAL_COM0);
#ifdef DEBUG
    cm_backtrace_init("build/baselayer", "gd32f4xx", "1.0.0");
#endif
    printf("\r\nplatform init done\r\n");
    extern int main(void);
    main();
}

void _prep_c(void)
{
    SCB->VTOR = VECTOR_ADDRESS & VTOR_MASK;
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif
    __DSB();
    __ISB();

    _data_copy();
    _bss_zero();

    _cstart();
}

void _putchar(char ch)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
}