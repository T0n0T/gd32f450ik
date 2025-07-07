.text
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

@ void *memset(void *s, int c, size_t n);
.global memset
.type memset, %function
.align 4
memset:
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
    bx lr
