.text
_barebones:
.data
.globl main
main:
	pushq %rbp			
	movq  %rsp, %rbp		
	movq $114 ,%rbx
	movq $1, %rax			
	movq $1, %rdi
	leaq .hello.str,%rsi
	syscall	
	movq	$60, %rax	
	movq	$0, %rdi		
	syscall
	popq %rbp		
.hello.str:
	.string "Hello World!\n"
	.size	.hello.str,13		

