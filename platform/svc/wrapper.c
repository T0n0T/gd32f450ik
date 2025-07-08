#include <stdint.h>
#include <stdio.h>
#include "gd32f4xx.h"
#include "platform.h"

int svc_1(uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
    printf("svc_1: args: %lu, %lu, %lu\r\n", arg0, arg1, arg2);
    return 4;
}

int svc_2(uint32_t count, ...)
{
    va_list args;
    va_start(args, count);
    printf("svc_2: args: %lu", count);
    for (uint32_t i = 0; i < count; i++) {
        printf(", %lu", va_arg(args, uint32_t));
    }
    printf("\r\n");
    va_end(args);
    return 5;
}

const uintptr_t __attribute__((section(".gnu.linkonce.syscall_tables")))
_svc_syscall_tables[_MAX_SVC_NUM] = {
    ((uintptr_t)NULL),  // SVC 0
    ((uintptr_t)svc_1), // SVC 1
    ((uintptr_t)NULL),  // SVC 2
    ((uintptr_t)NULL),  // SVC 3
    ((uintptr_t)NULL),  // SVC 4
    ((uintptr_t)NULL),  // SVC 5
    ((uintptr_t)NULL),  // SVC 6
    ((uintptr_t)NULL),  // SVC 7
    ((uintptr_t)NULL),  // SVC 8
    ((uintptr_t)NULL),  // SVC 9
    ((uintptr_t)NULL),  // SVC 10
    ((uintptr_t)NULL),  // SVC 11
};
