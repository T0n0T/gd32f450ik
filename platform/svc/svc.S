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
    push {r4, r5, lr}
    tst lr, #4
    ite eq
        mrseq r5, MSP
        mrsne r5, PSP
    ldr r1, [r5, #24]                       //get pc
    ldrb r1, [r1, #-2]                      //get svc number
    
    cmp r1, #_MAX_SVC_NUM
    bhi __svc_invalid

    ldr r2, =_svc_syscall_tables            //load svc_syscall_tables
    lsls r1, r1, #2                         //calculate index from svc number
    ldr r4, [r2, r1]                        //load svc handler address
    cbz r4, __svc_invalid                   //check if svc handler address is 0

    ldr	r0, [r5, #0]                        
    ldr	r1, [r5, #4]
    ldr	r2, [r5, #8]
    ldr	r3, [r5, #12]
    blx r4
    str r0, [r5, #0]
    pop {r4, r5, lr}
    bx lr

__svc_invalid:
    mov r0, #0xFFFFFFFF
    str r0, [r5, #0]
    pop {r4, r5, lr}
    bx lr
