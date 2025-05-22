/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2023-2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

/* caller save registers */
.macro prologue
	stp	x29, x30, [sp, #-16]!

	/* Currently it seems like we do no need to save the callee-save
	 * registers because they will be saved by the other functions. */
	//stp	x27, x28, [sp, #-16]!
	//stp	x25, x26, [sp, #-16]!
	//stp	x23, x24, [sp, #-16]!
	//stp	x21, x22, [sp, #-16]!
    //stp	x19, x20, [sp, #-16]!
	//stp	d14, d15, [sp, #-16]!
	//stp	d12, d13, [sp, #-16]!
	//stp	d10, d11, [sp, #-16]!
	//stp	d8,  d9,  [sp, #-16]!
.endm
.macro epilogue
	//ldp	d8,  d9,  [sp], #16
	//ldp	d10, d11, [sp], #16
	//ldp	d12, d13, [sp], #16
	//ldp	d14, d15, [sp], #16
	//ldp	x19, x20, [sp], #16
	//ldp	x21, x22, [sp], #16
	//ldp	x23, x24, [sp], #16
	//ldp	x25, x26, [sp], #16
	//ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16
.endm

/* register args */
.macro push_caller_save
	stp	x0, x1, [sp, #-16]!
	stp	x2, x3, [sp, #-16]!
	stp	x4, x5, [sp, #-16]!
	stp	x6, x7, [sp, #-16]!
.endm
.macro pop_caller_save
	ldp	x6, x7, [sp], #16
	ldp	x4, x5, [sp], #16
	ldp	x2, x3, [sp], #16
	ldp	x0, x1, [sp], #16
.endm

/* retval in x0, x1 */
.macro push_retval
	stp	x0, x1, [sp, #-16]!
.endm
.macro pop_retval
	ldp	x0, x1, [sp], #16
.endm

/* other module */
.macro arg_func_name name
	adr	x0, .func_\name
.endm
.macro call_func func
	bl	\func
	/* save return value in x8 */
	mov	x8, x0
.endm
.macro call_func_ptr
	blr x8
.endm
.macro return
	ret
.endm
