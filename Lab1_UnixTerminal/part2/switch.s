	.file	"switch.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$3, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$3, %eax
	je	.L3
	cmpl	$5, %eax
	je	.L4
	cmpl	$1, %eax
	jne	.L8
	movl	$11, -8(%rbp)
	jmp	.L6
.L3:
	movl	$13, -8(%rbp)
.L4:
	movl	$15, -8(%rbp)
	jmp	.L6
.L8:
	movl	$99, -8(%rbp)
.L6:
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
	.section	.note.GNU-stack,"",@progbits
