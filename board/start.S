#include "platform.h"

.text
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

/*
 * The entry point is located at the z_arm_reset symbol, which
 * is fetched by a XIP image playing the role of a bootloader, which jumps to
 * it, not through the reset vector mechanism. Such bootloaders might want to
 * search for a __start symbol instead, so create that alias here.
 */
.section .text._reset_section, "ax"
.align 4
__start:
    /* Reset CONTROL register */
    movs.n r0, #0
    msr CONTROL, r0
    isb

    /* Clear SPLIM registers */
    movs.n r0, #0
    msr MSPLIM, r0
    msr PSPLIM, r0

    ldr r0, =_main_stack + _MAIN_STACK_SIZE
    msr msp, r0

    bl SystemInit
    /* Initialize core architecture registers and system blocks */
    bl _arm_init_arch_hw_at_boot

    /* lock interrupts: will get unlocked when switch to main task */
    movs.n r0, #_EXC_IRQ_DEFAULT_PRIO
    msr BASEPRI, r0

    /*
     * Set PSP and use it to boot without using MSP, so that it
     * gets set to _interrupt_stacks during initialization.
     */
    ldr r0, =_interrupt_stacks
    ldr r1, =_ISR_STACK_SIZE
    adds r0, r0, r1
    msr PSP, r0
    mrs r0, CONTROL
    movs r1, #2
    orrs r0, r1 /* CONTROL_SPSEL_Msk */
    msr CONTROL, r0
    /*
     * When changing the stack pointer, software must use an ISB instruction
     * immediately after the MSR instruction. This ensures that instructions
     * after the ISB instruction execute using the new stack pointer.
     */
    isb

    /*
     * 'bl' jumps the furthest of the branch instructions that are
     * supported on all platforms. So it is used when jumping to z_prep_c
     * (even though we do not intend to return).
     */
    bl _prep_c
