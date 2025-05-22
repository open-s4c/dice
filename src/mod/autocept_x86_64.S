/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/*
 * callee-save: r12, r13, r14, r15, rbx, rsp, rbp
 * caller-save: rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11
 * arguments: rdi, rsi, rdx, rcx, r8, r9
 * return values: rax, rdx
 * stack pointer: rsp
*/

.macro prologue
        pushq  %rbx
        pushq  %r12
        pushq  %r13
        pushq  %r14
        pushq  %r15

.endm
.macro epilogue
        popq  %r15
        popq  %r14
        popq  %r13
        popq  %r12
        popq  %rbx
.endm

/* Function arguments. Arguments are passed in this order to functions. */
.macro push_caller_save
        pushq  %rdi
        pushq  %rsi
        pushq  %rdx
        pushq  %rcx
        pushq  %r8
        pushq  %r9
        pushq  %r10
        pushq  %r11
.endm
.macro pop_caller_save
        popq  %r11
        popq  %r10
        popq  %r9
        popq  %r8
        popq  %rcx
        popq  %rdx
        popq  %rsi
        popq  %rdi
.endm

/* We save %rax and %rdx in %r12 and %r13 since these are is a callee-save
 * register. If we would save it in the stack, we might need to change the frame
 * again. */
.macro push_retval
        movq  %rax, %r12
        movq  %rdx, %r13
.endm
.macro pop_retval
        movq  %r12, %rax
        movq  %r13, %rdx
.endm

/* other module */
.macro arg_func_name name
        leaq  .func_\name(%rip), %rdi
.endm

.macro call_func func
        call *\func@GOTPCREL(%rip)
.endm
.macro call_func_ptr
        call  *%rax
.endm
.macro return
        ret
.endm
