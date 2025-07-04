.text
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global _memset
.type _memset, %function
.align 4
_memset:
    push {lr}   @ 保存返回地址
    mov r3, r0              @ 保存原始的 s (r0) 作为返回值

    cmp r2, #0              @ 检查 n 是否为 0
    beq .L_memset_end       @ 如果 n 为 0，直接结束

    and r1, r1, #0xFF       @ 确保 r1 中只有要设置的字节值 (c 的低 8 位)
    orr r1, r1, r1, lsl #8  @ 形成 16 位值
    orr r1, r1, r1, lsl #16 @ 形成 32 位值 (r1 = cccc)

.L_memset_loop_byte:
    tst r0, #3              @ 检查 r0 是否 4 字节对齐
    beq .L_memset_loop_word @ 如果对齐，跳到按字写入

    cmp r2, #0              @ 检查是否还有字节要写入
    beq .L_memset_end       @ 如果没有，结束

    strb r1, [r0], #1       @ 按字节写入，r0 递增 1
    subs r2, r2, #1         @ r2 递减 1
    b .L_memset_loop_byte   @ 继续处理字节对齐

.L_memset_loop_word:
    cmp r2, #3              @ 检查是否还有至少 4 个字节要写入
    blt .L_memset_loop_rem  @ 如果少于 4 个，跳到处理剩余字节

    str r1, [r0], #4        @ 按字写入，r0 递增 4
    subs r2, r2, #4         @ r2 递减 4
    b .L_memset_loop_word   @ 继续按字写入

.L_memset_loop_rem:
    cmp r2, #0              @ 检查是否还有剩余字节
    beq .L_memset_end       @ 如果没有，结束

    strb r1, [r0], #1       @ 按字节写入剩余部分
    subs r2, r2, #1         @ r2 递减 1
    b .L_memset_loop_rem    @ 继续处理剩余字节

.L_memset_end:
    mov r0, r3              @ 将原始的 s (r3) 移动到 r0 作为返回值
    pop {pc}    @ 恢复返回地址并返回

.global _memcpy
.type _memcpy, %function
.align 4
_memcpy:
    push {r4, lr}   @ 保存寄存器和返回地址
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
    pop {r4, pc}    @ 恢复寄存器并返回