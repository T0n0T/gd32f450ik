#include "platform.h"

.text
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global __svc
.type __svc, %function
.section .text, "ax"
__svc:
    tst lr, #4
    ite eq
        mrseq r0, MSP
        mrsne r0, PSP
    ldr r1, [r0, #24]                       //get pc
    ldrb r1, [r1, #-2]                      //get svc number
    
    cmp r1, #_MAX_SVC_NUM
    bhi __svc_invalid

    ldr r3, =_svc_syscall_tables            //load svc_syscall_tables
    lsls r2, r1, #2                         //calculate index from svc number
    ldr r0, [r3, r2]                        //load svc handler address
    cbz r0, __svc_invalid                   //check if svc handler address is 0

    push {lr}
    blx r0
    pop {lr}
    bx lr

__svc_invalid:
    mov     r0, #0xFFFFFFFF
    bx      lr