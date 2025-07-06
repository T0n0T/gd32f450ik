#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// stack
#define _MAIN_STACK_SIZE 2048
#define _ISR_STACK_SIZE  512

// isr
#define _ISR_TABLE_SIZE       90
#define _EXC_IRQ_DEFAULT_PRIO 32

// svc
#define _MAX_SVC_NUM 12

#endif