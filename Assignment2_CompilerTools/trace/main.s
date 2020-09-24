	.file	"main.c"
	.text
	.section	.rodata
	.align 8
.LC0:
	.string	"Hello! msg() call %u, Let me sleep %u seconds.\n"
	.text
	.globl	msg
	.type	msg, @function
msg:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$1, -4(%rbp)
	movl	count.2970(%rip), %eax
	leal	1(%rax), %edx
	movl	%edx, count.2970(%rip)
	movl	-4(%rbp), %edx
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sleep@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	msg, .-msg
	.section	.rodata
.LC1:
	.string	"Message fron main()"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$0, -4(%rbp)
	leaq	.LC1(%rip), %rdi
	call	puts@PLT
	movl	$0, -4(%rbp)
	jmp	.L3
.L4:
	movl	$0, %eax
	call	msg
	addl	$1, -4(%rbp)
.L3:
	cmpl	$9, -4(%rbp)
	jle	.L4
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.local	count.2970
	.comm	count.2970,4,4
	.ident	"GCC: (Ubuntu 8.3.0-6ubuntu1) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
