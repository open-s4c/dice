#if !defined(__linux__)
#error "dice syscall module is only implemented for Linux"
#elif defined(__x86_64__)

    .section .note.GNU-stack,"",@progbits

    .text
    .extern __errno_location
    .extern dice_syscall_before
    .extern dice_syscall_after

    .set SYSCALL_EVENT_PC_OFFSET, 0
    .set SYSCALL_EVENT_NUMBER_OFFSET, 8
    .set SYSCALL_EVENT_ARGS_OFFSET, 16
    .set SYSCALL_EVENT_RET_OFFSET, 64
    .set SYSCALL_EVENT_FUNC_OFFSET, 80
    .set SYSCALL_EVENT_SIZE, 88
    .set SYSCALL_MD_OFFSET, SYSCALL_EVENT_SIZE
    .set SYSCALL_FRAME_SIZE, 112

    .section .rodata
.Lsyscall_name:
    .string "syscall"

    .text
    .globl syscall
    .type syscall, @function
syscall:
    .cfi_startproc

    pushq %rbx
    .cfi_def_cfa_offset 16
    .cfi_offset %rbx, -16

    subq $SYSCALL_FRAME_SIZE, %rsp
    .cfi_def_cfa_offset 128

    movq %rsp, %rbx

    movq SYSCALL_FRAME_SIZE + 8(%rsp), %rax
    movq %rax, SYSCALL_EVENT_PC_OFFSET(%rsp)
    movq %rdi, SYSCALL_EVENT_NUMBER_OFFSET(%rsp)
    movq %rsi, SYSCALL_EVENT_ARGS_OFFSET(%rsp)
    movq %rdx, SYSCALL_EVENT_ARGS_OFFSET + 8(%rsp)
    movq %rcx, SYSCALL_EVENT_ARGS_OFFSET + 16(%rsp)
    movq %r8, SYSCALL_EVENT_ARGS_OFFSET + 24(%rsp)
    movq %r9, SYSCALL_EVENT_ARGS_OFFSET + 32(%rsp)
    movq SYSCALL_FRAME_SIZE + 16(%rsp), %rax
    movq %rax, SYSCALL_EVENT_ARGS_OFFSET + 40(%rsp)
    movq $0, SYSCALL_EVENT_RET_OFFSET(%rsp)
    movq $0, SYSCALL_MD_OFFSET(%rsp)

    movq %rsp, %rdi
    leaq SYSCALL_MD_OFFSET(%rsp), %rsi
    call dice_syscall_before@PLT
    movq %rax, SYSCALL_EVENT_FUNC_OFFSET(%rbx)

    movq SYSCALL_EVENT_NUMBER_OFFSET(%rbx), %rdi
    movq SYSCALL_EVENT_ARGS_OFFSET(%rbx), %rsi
    movq SYSCALL_EVENT_ARGS_OFFSET + 8(%rbx), %rdx
    movq SYSCALL_EVENT_ARGS_OFFSET + 16(%rbx), %rcx
    movq SYSCALL_EVENT_ARGS_OFFSET + 24(%rbx), %r8
    movq SYSCALL_EVENT_ARGS_OFFSET + 32(%rbx), %r9
    subq $16, %rsp
    .cfi_def_cfa_offset 144

    movq SYSCALL_EVENT_ARGS_OFFSET + 40(%rbx), %rax
    movq %rax, (%rsp)
    xorl %eax, %eax
    call *SYSCALL_EVENT_FUNC_OFFSET(%rbx)
    addq $16, %rsp

    movq %rax, SYSCALL_EVENT_RET_OFFSET(%rbx)
    movq %rbx, %rdi
    leaq SYSCALL_MD_OFFSET(%rbx), %rsi
    call dice_syscall_after@PLT

    movq SYSCALL_EVENT_RET_OFFSET(%rbx), %rax
    addq $SYSCALL_FRAME_SIZE, %rsp
    .cfi_def_cfa_offset 16

    popq %rbx
    .cfi_restore %rbx
    .cfi_def_cfa_offset 8

    ret
    .cfi_endproc

    .size syscall, .-syscall

#elif defined(__aarch64__)

    .section .note.GNU-stack,"",@progbits

    .text
    .extern __errno_location
    .extern dice_syscall_before
    .extern dice_syscall_after

    .set SYSCALL_EVENT_PC_OFFSET, 0
    .set SYSCALL_EVENT_NUMBER_OFFSET, 8
    .set SYSCALL_EVENT_ARGS_OFFSET, 16
    .set SYSCALL_EVENT_RET_OFFSET, 64
    .set SYSCALL_EVENT_FUNC_OFFSET, 72
    .set SYSCALL_EVENT_NAME_OFFSET, 80
    .set SYSCALL_EVENT_SIZE, 88
    .set SYSCALL_MD_OFFSET, SYSCALL_EVENT_SIZE
    .set SYSCALL_FRAME_SIZE, 96

    .text
    .globl syscall
    .type syscall, %function
syscall:
    .cfi_startproc
    .cfi_def_cfa sp, 0

    stp x19, x30, [sp, #-16]!
    .cfi_def_cfa_offset 16
    .cfi_offset 19, -16          /* saved x19 */
    .cfi_offset 30, -8           /* saved LR/x30 */

    sub sp, sp, #SYSCALL_FRAME_SIZE
    .cfi_def_cfa_offset 112

    mov x19, sp

    str x30, [sp, #SYSCALL_EVENT_PC_OFFSET]
    str x0, [sp, #SYSCALL_EVENT_NUMBER_OFFSET]
    stp x1, x2, [sp, #SYSCALL_EVENT_ARGS_OFFSET]
    stp x3, x4, [sp, #SYSCALL_EVENT_ARGS_OFFSET + 16]
    stp x5, x6, [sp, #SYSCALL_EVENT_ARGS_OFFSET + 32]
    str xzr, [sp, #SYSCALL_EVENT_RET_OFFSET]

    mov x0, sp
    add x1, sp, #SYSCALL_MD_OFFSET
    bl dice_syscall_before
    str x0, [x19, #SYSCALL_EVENT_FUNC_OFFSET]

    ldr x0, [x19, #SYSCALL_EVENT_NUMBER_OFFSET]
    ldp x1, x2, [x19, #SYSCALL_EVENT_ARGS_OFFSET]
    ldp x3, x4, [x19, #SYSCALL_EVENT_ARGS_OFFSET + 16]
    ldp x5, x6, [x19, #SYSCALL_EVENT_ARGS_OFFSET + 32]
    ldr x9, [x19, #SYSCALL_EVENT_FUNC_OFFSET]
    blr x9

    str x0, [x19, #SYSCALL_EVENT_RET_OFFSET]
    mov x0, x19
    add x1, x19, #SYSCALL_MD_OFFSET
    bl dice_syscall_after

    ldr x0, [x19, #SYSCALL_EVENT_RET_OFFSET]
    add sp, sp, #SYSCALL_FRAME_SIZE
    .cfi_def_cfa_offset 16

    ldp x19, x30, [sp], #16
    .cfi_restore 19
    .cfi_restore 30
    .cfi_def_cfa_offset 0

    ret
    .cfi_endproc

    .size syscall, .-syscall

#else
#error "dice syscall module is only implemented for x86_64 and aarch64"
#endif
