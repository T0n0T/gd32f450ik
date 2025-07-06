#include "platform.h"

.text
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global _main_stack
.type _main_stack, %object

.global _vector_table
.type _vector_table, %object
.section .vectors, "ax"
.align 4
    .word _main_stack + _MAIN_STACK_SIZE

    .word __reset
    .word __nmi

    .word __hard_fault
    .word __mpu_fault
    .word __bus_fault
    .word __usage_fault
    .word 0
    .word 0
    .word 0
    .word 0
    .word __svc
    .word __debug_monitor
    .word 0
    .word __pendsv
    .word __sysclk_isr

Default_Handler:
    b  .
    .size  Default_Handler, . - Default_Handler

.macro  IRQ handler
    .weak   \handler
    .set    \handler, Default_Handler
    .endm

    IRQ __nmi
    IRQ __hard_fault
    IRQ __mpu_fault
    IRQ __bus_fault
    IRQ __usage_fault
    IRQ __svc
    IRQ __debug_monitor
    IRQ __pendsv
    IRQ __sysclk_isr