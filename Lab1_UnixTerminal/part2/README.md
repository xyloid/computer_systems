# x86-64 Assembly 

> "Nearing Machine Code Representation"

## Introduction/Motivation

Not so long ago (in a galaxy not so far away), programmers wrote most of their code in assembly. While programmers today primarily use higher level languages (Python, C, etc), it is not uncommon to debug the assembly of your code. These higher level languages afterall typically translate down to an assemble or assembly-like language. 

If you are interested in cybersecurity and reverse engineering, folks more frequently write and analyze assembly code. For high performance applications like [games](https://www.gamasutra.com/view/news/169946/CC_low_level_curriculum_Looking_at_optimized_assembly.php), programmers may write very optimized code using assembly to get things *just* right. If you are working in hardware or an embededd device, you might also do some assembly programming, as other languages environments are too bulky to support on a small device. Even web developers are using something called 'webassembly'. Hmm, the list is getting long here--I think the point is that learning assembly has quite some relevance! Let's dig in and get some practice.

In today's lab you are going to get some practice looking at assembly.

# Part 0 - Godbolt

I **strongly** recommend using the godbolt tool (https://godbolt.org/) to write and experiment with your C programs for this exercise.  The color mappings will help you see what is going on with the generated assembly. You **should** try using both godbolt and your compiler to generate assembly.

Here is an example of the Godbolt tool (and also shows part 4 of this lab)
<img src="./assembly.PNG">

# Part 1- Compiler Generated Assembly

Let us get some experience reading assembly code generated by the compiler (or godbolt)! It is actually kind of fun, you may learn some new instructions, and at the very least gain some intuition for what code the compiler is generating.

## Compiler generated assembly 1 - Swap

- Write a C program that swaps two integers(in the main body of code).
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as swap_int.s).
  - Use: `gcc -O0 -fno-builtin swap_int.c -S -o swap_int.s`
- Now modify your program to swap two long's.
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as swap_long.s).
  - Use: `gcc -O0 -fno-builtin swap_long.c -S -o swap_long.s`
- Compare each of the two assembly files using diff. See what changed.
- diff syntax
  - Use: `diff -y swap_int.s swap_long.s`
  
### Response/Observations

*Observations are written as the comments among the assembly codes for clarity.*

```Assembly
; Result of `diff -y swap_int.s swap_long.s`
; The formats of all code blocks in this README.md are adjusted on github.ccs.neu.edu. 
; I can not guarantee their formats on other markdown renderer.

	.file	"swap_int.c"			      |		.file	"swap_long.c"
	.text								.text
	.globl	main							.globl	main
	.type	main, @function					.type	main, @function
main:								main:
.LFB0:								.LFB0:
	.cfi_startproc							.cfi_startproc
; Push the base address of the current stack frame to the stack.
; Suffix q means quadword(64-bit).
	pushq	%rbp							pushq	%rbp
	.cfi_def_cfa_offset 16					.cfi_def_cfa_offset 16
	.cfi_offset 6, -16						.cfi_offset 6, -16
; Set the top of the current stack frame as the new base of the stack frame.
	movq	%rsp, %rbp						movq	%rsp, %rbp
	.cfi_def_cfa_register 6					.cfi_def_cfa_register 6
; Store local variables on the stack. 
; I.	The memory addressing mode is base-relative.
; II.	The direction of the stack is downward. So here the offset are negative.
; III.	The offset is 4 when the data type is int as int is 32-bit. On the other hand, 
;	the offset is 8 when the data type is long as long is 64-bit.
; IV.	The instruction has different suffix: 'l' for 32-bit int and 'q' for 64-bit long. 
	movl	$1, -4(%rbp)			      |		movq	$1, -8(%rbp)
	movl	$2, -8(%rbp)			      |		movq	$2, -16(%rbp)
; Swap the values. Int use EAX as they are 32-bit. Long use RAX as they are 64-bit. 
; It seems that MOV can not move value from an memory address to another memory address directly.
; Here, we can see the program first move a value from a memory address to EAX and then move the value
; from EAX to another memory address.
	movl	-4(%rbp), %eax			      |		movq	-8(%rbp), %rax
	movl	%eax, -12(%rbp)		      |		movq	%rax, -24(%rbp)
	movl	-8(%rbp), %eax			      |		movq	-16(%rbp), %rax
	movl	%eax, -4(%rbp)			      |		movq	%rax, -8(%rbp)
	movl	-12(%rbp), %eax		      |		movq	-24(%rbp), %rax
	movl	%eax, -8(%rbp)			      |		movq	%rax, -16(%rbp)
; Store the return value in EAX. According to 64-bit X86 calling convention, the caller can expect to
; find the return value of a subroutine in the register EAX
	movl	$0, %eax						movl	$0, %eax
; Restore previous base address of the stack frame to the base pointer.
	popq	%rbp							popq	%rbp
	.cfi_def_cfa 7, 8						.cfi_def_cfa 7, 8
	ret								ret
	.cfi_endproc							.cfi_endproc
.LFE0:								.LFE0:
	.size	main, .-main						.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)
	.section .note.GNU-stack,"",@progbits 			.section .note.GNU-stack,"",@progbits
```

## Compiler generated assembly 2 - Functions

- Write a C program that swaps two integers in a **function** (You may use today's slide as a reference)
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as swap.s).
  - Use: `gcc -O0 -fno-builtin  swap.c -S -o swap.s`
- Do the instructions use memory/registers in a different way?

### Response/Observations

*Observations are written as the comments among the assembly codes for clarity.*

Registers RDI and RSI are used to store arguments.

```Assembly
	.file	"swap.c"
	.text
	.globl	swap
	.type	swap, @function
swap:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
; Move two arguments to the stack. Both of them are pointers, representing the address of
; the variables whose values are going to be swapped.
; According to System V ABI, RDI is the first argument, RSI is the second argument. 
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
; Move the address of the first variable to RAX.
	movq	-24(%rbp), %rax
; Dereference the pointer. Move the value of the first varibale to EAX (int).
	movl	(%rax), %eax
; Store the value of the first variable on the stack.
	movl	%eax, -4(%rbp)
; Move the address of the second variable to RAX.
	movq	-32(%rbp), %rax
; Dereference the pointer. Move the value of the second varibale to EDX (int).
	movl	(%rax), %edx
; Move the address of the first varibale to RAX.
	movq	-24(%rbp), %rax
; Move the value of the second variable to the address of the first variable.
	movl	%edx, (%rax)
; Move the address of the second varibale to RAX.
	movq	-32(%rbp), %rax
; Move the value of the first variable to EDX (int).
	movl	-4(%rbp), %edx
; Move the value of the first variable to the address of the second variable.
	movl	%edx, (%rax)
; Restore base pointer of the stack frame.
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	swap, .-swap
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
; Allocate some stack space, we have local variables. 
; It seems that the size of stack space here is always a multiple of 8.
; I am not sure if the size is always greater than the whole size of the local variables,
; because in array.s, the size here is smaller then 400*4, which is confusing.
; A very ineteresting phenomenon is that if the function will call another function,
; then the size is greater than the whole size of the local variables.
; This is understandable because RSP is used by callee to set new RBP. 
; After serveral hours googling, I was not able to find any resource which gives a
; clear explanation about how to determine the value of RSP here.
	subq	$16, %rsp
; Initialize local variables on the stack. These two values are going to be swapped.
	movl	$1, -4(%rbp)
	movl	$2, -8(%rbp)
; LEA(Load Effective Address), move the addresses of two variables to 
; the general purpose registers RDX and RAX. 
	leaq	-8(%rbp), %rdx
	leaq	-4(%rbp), %rax
; Move two addresses to the first and second argument registers.
	movq	%rdx, %rsi
	movq	%rax, %rdi
; Call subroutine.
	call	swap
; Return value is 0.
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
	.section	.note.GNU-stack,"",@progbits
```

## Compiler generated assembly 3 - Static Array
- Write a C program called array.c that has an array of 400 integers in the function of main.
  - Initialize some of the values to something (do not use a loop) (e.g. myArray[0]=72; myArray[70]=56; etc)
  	- Note that it is helpful to use 'weird' numbers so you can see where they jump out.
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as array.s).
  - Use: `gcc -O0 -fno-builtin  array.c -S -o array.s`
- How much are the offsets from the address?

### Response/Observations

*Write how much are the offsets from the address if any here*

*Observations are written as the comments among the assembly codes for clarity.*

```Assembly
	.file	"array.c"
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
	subq	$1480, %rsp
; Since the stack grows downward, and the first items in the array has the lowest address,
; the first item's address is -1600(%rbp). The 4 bytes of the first integer are -1600(%rbp),
; -1599(%rbp), -1598(%rbp), -1597(%rbp).
	movl	$100, -1600(%rbp)
	movl	$200, -1596(%rbp)
; As state above, the last item of the array has 4 bytes: 
; -4(%rbp), -3(%rbp), -2(%rbp), -1(%rbp).
	movl	$400, -4(%rbp)
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
.section	.note.GNU-stack,"",@progbits
```

## Compiler generated assembly 4 - Dynamic Array 

- Write a C program called array2.c that has an array of 400 integers in the function of main that is dynamically allocated.
  - Initialize some of the values to something (do not use a loop) (e.g. myArray[66]=712; myArray[70]=536; etc)
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as array2.s).
  - Use: `gcc -O0 -fno-builtin  array2.c -S -o array2.s`
- Study the assembly and think about what is different from the static array.

### Response/Observations

*Write here if there is anything different from the static vs dynamic array*

*Observations are written as the comments among the assembly codes for clarity.*

```Assembly
	.file	"array2.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
; Allocate stack spaces.
	subq	$1616, %rsp
; Move the size of the array (400*4) into EDI register.
; EDI stores the first and the only argument of malloc, 
; the size of the memory to be allocated.
	movl	$1600, %edi
	call	malloc
; After malloc, RAX store the address of the allocated memory.
; Here the address is stored on the stack.(Move from RAX to a stack address.)
	movq	%rax, -8(%rbp)
; Move the address of the first item of the array to rax.
	movq	-8(%rbp), %rax
; Move 0 to the address of the first item of the array.
	movl	$0, (%rax)
; Move the address of the first item of the array to rax.
	movq	-8(%rbp), %rax
; Add offset to the address of the first array item.
; So RAX stores the adress of the last array item.
; Here, the offset is positive number, and it is added to the "base address".
; This reveals the array is on the heap rather than stack, as the heap grows upward.
	addq	$1596, %rax
	movl	$400, (%rax)
	movq	-8(%rbp), %rax
	addq	$1196, %rax
	movl	$300, (%rax)
; The local variable array is on the stack, it has negative offset as the stack grows downward.
	movl	$20, -1616(%rbp)
	movl	$2400, -20(%rbp)
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
.section	.note.GNU-stack,"",@progbits
```

## Compiler generated assembly 5 - Goto
The C programming language has a 'goto' command, search how to use it if you have not previously.

- Write a C program using the goto command and a label.
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as goto.s).
  - Use: `gcc -O0 -fno-builtin  goto.c -S -o goto.s`
- Observe what kind of jmp statement is inserted.

### Response/Observations

*Write here what kind of jmp statement is inserted*

*Observations are written as the comments among the assembly codes for clarity.*

```Assembly
	.file	"goto.c"
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
; Initialize the local variable on the stack.
	movl	$0, -4(%rbp)
; The NOP is here because there are two lines of code won't be executed
; as a goto statement will skip them.
	nop
.L2:
; Increment local variable.
	addl	$1, -4(%rbp)
; Compare local variable with 2 by subtracting local varible from 2. 
	cmpl	$2, -4(%rbp)
; Jump to .L3 if the local variable is greater than 2. 
; Note the c code is "if (a < 3)".
	jg	.L3
; If the above line doesn't jump to .L2, then jump to .L2.
	jmp	.L2
.L3:
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
.section	.note.GNU-stack,"",@progbits
```

## Compiler generated assembly 6 - For-loops
- Write a C program using a for-loop that counts to 5.
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as for.s).
  - Use: `gcc -O0 -fno-builtin  for.c -S -o for.s`
- Observe where the code goes for the condition statement (at the start or at the end?).

### Response/Observations

*Write here a note about where the code goes for the condition statement.*

*Observations are written as the comments among the assembly codes for clarity.*

```Assembly
	.file	"for.c"
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
; Initialize local variable i;
	movl	$0, -4(%rbp)
; Initialize local varibale sum. 
	movl	$0, -8(%rbp)
; The i = 0; statement in for(i=0;i<5;i++). 
	movl	$0, -4(%rbp)
; Jump to the condition statement of for loop.
	jmp	.L2
.L3:
	movl	-4(%rbp), %eax
	addl	%eax, -8(%rbp)
; The i++ statement in for(i=0;i<5;i++). 
	addl	$1, -4(%rbp)
.L2:
; The i < 5; statement in for(i=0;i<5;i++). 
	cmpl	$4, -4(%rbp)
; If local variable less or equal to 4, jump to .L3, the loop body.
; Else, loop ends.
	jle	.L3
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
.section	.note.GNU-stack,"",@progbits
```

## Compiler generated assembly 7 - Switch Statements

- Write a C program using a switch statement (Sample here)[https://www.tutorialspoint.com/cprogramming/switch_statement_in_c.htm].
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as switch.s).
  - Use: `gcc -O0 -fno-builtin switch.c -S -o switch.s`
- See what code a switch statement generates. Is it optimal?

### Response/Observations

*Write here your observations with a switch statement*

*Observations are written as the comments among the assembly codes for clarity.*

```Assembly

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
; Initialize the local variable "flag".
	movl	$3, -4(%rbp)
; Move the value of "flag" to EAX. 
	movl	-4(%rbp), %eax
; Then compare the value with each case in the switch block.
; case 3:
	cmpl	$3, %eax
; If condition of case 3 is satisfied, jump to .L3, which is the code block in case 3.
	je	.L3
; case 5:
	cmpl	$5, %eax
; If condition of case 5 is satisfied, jump to .L4, which is the code block in case 5.
	je	.L4
; case 1:
	cmpl	$1, %eax
; If condition of case 1 is not satisfied, jump to .L8, which is the default case of the switch-case.
	jne	.L8
; If the above jump doesn't happen, then condition of case 1 is satisfied.
; The code block of case 1.
; Note -8(%rbp) is the address of local variable "value". 
; Although the variable is declared at the beginning of the program. 
; The address does not appear until when it is assigned a value for the first time.
; It seems that declaring a local varible without assigning an initial value means 
; the uninitialize local variable could contain any value.
	movl	$11, -8(%rbp)
; Jump to the code after switch-case.
	jmp	.L6
.L3:
; The code block of case 3. 
	movl	$13, -8(%rbp)
; As there is no break statement between case 3 and case 5, 
; the flow of execution will continue to the code block of case 5.
.L4:
; The code block of case 5.
	movl	$15, -8(%rbp)
	jmp	.L6
.L8:
; Code block of default.
	movl	$99, -8(%rbp)
.L6:
; Code after switch-case.
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
.section	.note.GNU-stack,"",@progbits
```

## Compiler generated assembly 8 - Add Function

- Write a C program that calls an add function(long add(long a, long b).
  - Save, Compile, and Run the program to verify it works.
- Output the assembly from that program (Save it as add.s).
  - Use: `gcc -O0 -fno-builtin add.c -S -o add.s`
- Observe the outputs
- Observe arguments put into registers
- Observe where 'popq' is called.

### Response/Observations

*Write here where popq is called*

*Observations are written as the comments among the assembly codes for clarity.*

```asm
	.file	"add.c"
	.text
	.globl	add
	.type	add, @function
add:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
; Copy two arguments to the current stack frame of the callee.
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	-8(%rbp), %rdx
; Return value is stored in EAX.
	addq	%rdx, %rax
; Restore previous base pointer of the stack frame before the callee returns.
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	add, .-add
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
	subq	$32, %rsp
	movq	$10, -8(%rbp)
	movq	$11, -16(%rbp)
	movq	-16(%rbp), %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	add
	movq	%rax, -24(%rbp)
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-28)"
	.section	.note.GNU-stack,"",@progbits
```

# More resources to help

- Matt Godbolt has written a great tool to help understand assembly generated from the compiler. 
  - https://godbolt.org/
- An assembly cheat sheet from Brown
  - https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf
- MIT Cheat sheet
  - http://6.035.scripts.mit.edu/sp17/x86-64-architecture-guide.html

# Deliverable

- For part 1, add your .S files that you have generated to this repository.
  - Note this submission will be auto graded for completion (i.e. save the file names as shown).
  - Add your observations in the appropriate response/observations section for each code.

# Going Further

- (Optional) Try the objdump example to read the disassembly from your programs executables. Observe how close the output is to the compiler generated output.
