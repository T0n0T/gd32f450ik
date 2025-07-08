#ifndef __SVC_H__
#define __SVC_H__

#ifdef __cplusplus
extern "C" {
#endif

// in cortex-m,only r0-r3 will be automatically push, so svc param limit to 4
#define SVCALL(number, return_type, signature)           \
    __attribute__((naked))                               \
    __attribute__((unused)) static return_type signature \
    {                                                    \
        __asm(                                           \
            "svc %0\n"                                   \
            "bx lr" : : "I"(number) : "r0");             \
    }

#ifdef __cplusplus
}
#endif

#endif