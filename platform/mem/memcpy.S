.text
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

@ void *memcpy(void *dest, const void *src, size_t n);
.global memcpy
.type memcpy, %function
.align 4
memcpy:
    push {r4}               @ 保存寄存器和返回地址
    mov r3, r0              @ 保存原始的 dest (r0) 作为返回值

    cmp r2, #0              @ 检查 n 是否为 0
    beq .L_memcpy_end       @ 如果 n 为 0，直接结束

.L_memcpy_loop_byte:
    tst r0, #3              @ 检查 dest 是否 4 字节对齐
    beq .L_memcpy_loop_word @ 如果对齐，跳到按字复制

    tst r1, #3              @ 检查 src 是否 4 字节对齐
    beq .L_memcpy_loop_word @ 如果对齐，跳到按字复制

    cmp r2, #0              @ 检查是否还有字节要复制
    beq .L_memcpy_end       @ 如果没有，结束

    ldrb r4, [r1], #1       @ 从 src 按字节加载，r1 递增 1
    strb r4, [r0], #1       @ 存储到 dest 按字节，r0 递增 1
    subs r2, r2, #1         @ r2 递减 1
    b .L_memcpy_loop_byte   @ 继续处理字节对齐

.L_memcpy_loop_word:
    cmp r2, #3              @ 检查是否还有至少 4 个字节要复制
    blt .L_memcpy_loop_rem  @ 如果少于 4 个，跳到处理剩余字节

    ldr r4, [r1], #4        @ 从 src 按字加载，r1 递增 4
    str r4, [r0], #4        @ 存储到 dest 按字，r0 递增 4
    subs r2, r2, #4         @ r2 递减 4
    b .L_memcpy_loop_word   @ 继续按字复制

.L_memcpy_loop_rem:
    cmp r2, #0              @ 检查是否还有剩余字节
    beq .L_memcpy_end       @ 如果没有，结束

    ldrb r4, [r1], #1       @ 从 src 按字节加载剩余部分
    strb r4, [r0], #1       @ 存储到 dest 按字节剩余部分
    subs r2, r2, #1         @ r2 递减 1
    b .L_memcpy_loop_rem    @ 继续处理剩余字节

.L_memcpy_end:
    mov r0, r3              @ 将原始的 dest (r3) 移动到 r0 作为返回值
    pop {r4}
    bx lr