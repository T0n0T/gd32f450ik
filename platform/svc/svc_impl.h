#ifndef __SVC_IMPL_H__
#define __SVC_IMPL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

// 异常栈帧结构（ARM Cortex-M）
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
} ExceptionStackFrame;

// 系统调用处理函数宏
#define SYSCALL_HANDLER(number, return_type, name, ...)                  \
    return_type name(__VA_ARGS__);                                       \
    static void __syscall_##number##_handler(ExceptionStackFrame *frame) \
    {                                                                    \
        __syscall_impl_##number(                                         \
            frame,                                                       \
            name,                                                        \
            return_type,                                                 \
            ##__VA_ARGS__);                                              \
    }                                                                    \
    __attribute__((used)) static return_type name

// 参数提取实现宏
#define __syscall_impl_ ##number(frame, func, ret_type, ...) _Generic((ret_type){0}, \
    void: __syscall_call_void_##number(frame, func, ##__VA_ARGS__),                  \
    default: __syscall_call_ret_##number(frame, func, ret_type, ##__VA_ARGS__))

// 有返回值版本
#define __syscall_call_ret_##number(frame, func, ret_type, ...) do                                                                                 \
    {                                                                                                                       \
        ret_type result = func(                                                                                             \
            APPLY_ARGS(__VA_ARGS__, __syscall_arg3, __syscall_arg2, __syscall_arg1, __syscall_arg0)(frame, ##__VA_ARGS__)); \
        frame->r0 = (uint32_t)result;                                                                                       \
    }                                                                                                                       \
    while (0)

// 无返回值版本
#define __syscall_call_void_ ##number(frame, func, ...) \
    func(                                               \
        APPLY_ARGS(__VA_ARGS__, __syscall_arg3, __syscall_arg2, __syscall_arg1, __syscall_arg0)(frame, ##__VA_ARGS__))

// 参数提取辅助宏
#define APPLY_ARGS(_1, _2, _3, _4, NAME, ...) NAME
#define __syscall_arg0(frame)
#define __syscall_arg1(frame, t1)             (t1)(frame->r0)
#define __syscall_arg2(frame, t1, t2)         (t1)(frame->r0), (t2)(frame->r1)
#define __syscall_arg3(frame, t1, t2, t3)     (t1)(frame->r0), (t2)(frame->r1), (t3)(frame->r2)
#define __syscall_arg4(frame, t1, t2, t3, t4) (t1)(frame->r0), (t2)(frame->r1), (t3)(frame->r2), (t4)(frame->r3)

#ifdef __cplusplus
}
#endif

#endif