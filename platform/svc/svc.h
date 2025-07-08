#ifndef __SVC_H__
#define __SVC_H__

#ifdef __cplusplus
extern "C" {
#endif

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