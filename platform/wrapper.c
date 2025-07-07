#include <stdint.h>
#include <stdio.h>
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

void svc_1(void)
{
    static int test = 0;
    test++;
    printf("svc_1: %d\r\n", test);
}

const uintptr_t __attribute__((section(".gnu.linkonce.syscall_tables")))
_svc_syscall_tables[_MAX_SVC_NUM] = {
    ((uintptr_t)NULL), // SVC 0
    ((uintptr_t)svc_1),  // SVC 1
    ((uintptr_t)NULL),   // SVC 2
    ((uintptr_t)NULL),   // SVC 3
    ((uintptr_t)NULL),   // SVC 4
    ((uintptr_t)NULL),   // SVC 5
    ((uintptr_t)NULL),   // SVC 6
    ((uintptr_t)NULL),   // SVC 7
    ((uintptr_t)NULL),   // SVC 8
    ((uintptr_t)NULL),   // SVC 9
    ((uintptr_t)NULL),   // SVC 10
    ((uintptr_t)NULL),   // SVC 11
};

void _svc_handle(uint8_t svc_num)
{
    if (svc_num >= _MAX_SVC_NUM) {
        printf("invaild svc_num: %d\r\n", svc_num);
        return;
    }
    if (_svc_syscall_tables[svc_num]) {
        ((void (*)(void))_svc_syscall_tables[svc_num])();
    } else {
        printf("invaild svc_num: %d\r\n", svc_num);
    }

}