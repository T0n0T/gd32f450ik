.text
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

/**
 *
 * @brief Fault handler installed in the fault vectors
 *
 * Entry point for the HardFault, MemManageFault, BusFault, UsageFault,
 * SecureFault and Debug Monitor exceptions.
 *
 * The function supplies the values of
 * - the MSP
 * - the PSP
 * - the EXC_RETURN value
 */

.global __hardfault
.type __hardfault, %function
.section .text._fault_section, "ax"
.align 4
__hard_fault:

.global __mpu_fault
.type __mpu_fault, %function
.section .text._fault_section, "ax"
.align 4
__mpu_fault:

.global __bus_fault
.type __bus_fault, %function
.section .text._fault_section, "ax"
.align 4
__bus_fault:

.global __usage_fault
.type __usage_fault, %function
.section .text._fault_section, "ax"
.align 4
__usage_fault:

.global __exc_spurious
.type __exc_spurious, %function
.section .text._fault_section, "ax"
.align 4
__exc_spurious:

.weak __debug_monitor
.type __debug_monitor, %function
.section .text._fault_section, "ax"
.align 4
__debug_monitor:

    mov     r0, lr                  /* get lr (EXC_RETURN) */
    tst     r0, #0x4                /* Test bit 2 of EXC_RETURN. 0=MSP, 1=PSP */
    ite     eq                      /* If EQ (bit 2 is 0), then use MSP */
    mrseq   r1, MSP                 /* If bit 2 is 0, get MSP */
    mrsne   r1, PSP                 /* If bit 2 is 1, get PSP */
    @ bl      cm_backtrace_fault
    bl .
loop:
    bl      loop
