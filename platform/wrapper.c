#include <stdint.h>
#include "gd32f4xx.h"
#include "platform.h"

struct _isr_table_entry {
    const void *arg;
    void (*isr)(const void *);
};

typedef void (*ISR)(const void *);

void isr_wrapper(void);

const uintptr_t __attribute__((section(".gnu.linkonce.irq_vector_table")))
_irq_vector_table[_ISR_TABLE_SIZE] = {
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
    ((uintptr_t)&isr_wrapper),
};

struct _isr_table_entry _sw_isr_table[_ISR_TABLE_SIZE];

void isr_wrapper(void)
{
    int32_t irq_number = __get_IPSR();
    /* _sw_isr_table does not map the core system exceptions,
     * which take first 16 interrupt numbers, only the external
     * interrupts.
     */
    irq_number -= 16;

    struct _isr_table_entry *entry = &_sw_isr_table[irq_number];
    if (entry->isr) {
        (entry->isr)(entry->arg);
    }
}

void isr_install(unsigned int irq, void (*routine)(const void *),
                 const void *param)
{
    /* If dynamic IRQs are enabled, then the _sw_isr_table is in RAM and
     * can be modified
     */
    _sw_isr_table[irq].arg = param;
    _sw_isr_table[irq].isr = routine;
}

void svc_0(void)
{
    static int test = 0;
    test++;
}

const uintptr_t __attribute__((section(".gnu.linkonce.syscall_tables")))
_svc_syscall_tables[_ISR_TABLE_SIZE] = {
    (uintptr_t)&svc_0,    
};