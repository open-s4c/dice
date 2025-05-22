/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#if defined(__APPLE__)
#define SYM(name) _##name
#define FUNC(name) ___fake_##name
.macro .size lab, other
.endm
.macro .type lab, other
.endm
.macro .sec_rodata
.section __TEXT,__text,regular,pure_instructions
.endm
#else
#define SYM(name) name
#define FUNC(name) name
.macro .sec_rodata
.section .rodata
.endm
#endif

/* Callback functions before and after executing an intercepted function. */
.extern SYM(autocept_before)
.extern SYM(autocept_after)

/*
 * @param name symbol name of intercepted function
 */
.macro add_interceptor name
        .sec_rodata
.func_\name:
        .string "\name"
        .text
        .align  4
        .global FUNC(\name)
        .type   FUNC(\name), @function
FUNC(\name):
        /* Push callee-save AND caller-save registers onto the stack. */
        prologue
        push_caller_save

        /* BEFORE_CALL: start intercept and find pointer to intercepted
         * function, return with function pointer. */
        arg_func_name \name
        call_func SYM(autocept_before)

        /* CALL: restore caller-save registers and perform real call. */
        pop_caller_save
#if defined(__APPLE__)
        call_func SYM(\name)
#else
        call_func_ptr
#endif

        /* AFTER_CALL: Save return value and finish book keeping. */
        push_retval
        arg_func_name \name
        call_func SYM(autocept_after)

        /* Restore return value from CALL and callee-save registers. */
        pop_retval
        epilogue
        return

.end_\name:
        .size \name, .- \name

#if defined(__APPLE__)
	/* Darwin interpose object */
	.section	__DATA,__interpose
	.p2align	3, 0x0
__interpose_\name:
	.quad	FUNC(\name)
	.quad	SYM(\name)
#endif
.endm

