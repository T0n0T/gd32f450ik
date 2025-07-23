#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// irq
#define _EXC_PRIO(pri)              (((pri) << (8 - NUM_IRQ_PRIO_BITS)) & 0xff)
#define _EXCEPTION_RESERVED_PRIO    1

#define _EXC_FAULT_PRIO             0
#define _EXC_ZERO_LATENCY_IRQS_PRIO 0
#define _EXC_SVC_PRIO               1
#define _IRQ_PRIO_OFFSET            (_EXCEPTION_RESERVED_PRIO + _EXC_SVC_PRIO)

#define _EXC_IRQ_DEFAULT_PRIO       _EXC_PRIO(_IRQ_PRIO_OFFSET)

/* Use lowest possible priority level for PendSV */
#define _EXC_PENDSV_PRIO      0xff
#define _EXC_PENDSV_PRIO_MASK _EXC_PRIO(_EXC_PENDSV_PRIO)

// stack
#define _MAIN_STACK_SIZE 2048
#define _ISR_STACK_SIZE  512
#endif