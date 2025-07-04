#include <stdint.h>
#include <stddef.h>
#include "gd32f4xx.h"
#include "platform.h"

char _main_stacks[1024];
char _interrupt_stacks[1024];

extern char _vector_start[];
extern char _vector_end[];

// bss
extern char __bss_start[];
extern char __bss_end[];

// data
extern char __data_start[];
extern char __data_end[];

void *_memset(void *s, int c, size_t n);
void *_memcpy(void *dest, const void *src, size_t n);

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
    _memset(__bss_start, 0, __bss_end - __bss_start);
}

void _data_copy(void)
{
    _memcpy(__data_start, __data_end, __data_end - __data_start);
}

void _arm_interrupt_init(void)
{
    _arm_init_arch_hw_at_boot();
}

void _prep_c(void)
{
    SCB->VTOR = VECTOR_ADDRESS & VTOR_MASK;
    __DSB();
    __ISB();

    _bss_zero();
    _data_copy();
    _arm_interrupt_init();

    extern int main(void);
    main();
    //CODE_UNREACHABLE
}