# Build an executable using the following:
#
# clang barebones.s -o barebones  # clang is another compiler like gcc
#
.text
_barebones:

.data
	
.globl main

main:
					# (1) What are we setting up here?
					# Ans:
	pushq %rbp			# Push current address of the base of the current stack frame to the stack. 
	movq  %rsp, %rbp		# Set the current address of the top of the current stack frame as the new 
					# base of the stack frame. So we have a new stack frame on the old one.

					# (2) What is going on here
					# Ans:
	movq $1, %rax			# RAX stores the number of system call. 1 is write.
	movq $1, %rdi			# RDI stores the first parameter of the system call. For write, the first
					# parameter is file handle to write. Here, file handle is 1, in linux it means stdout.
	leaq .hello.str,%rsi		# RSI stores the second parameter of the system call. For write, the second
					# parameter is the address of the string to write. Command leaq move the address of
					#.hello.str to RSI. According to google, it seems that usually the third
					# parameter buffer size is stored in RDX. But here the buffer size 13 is defined in
					# .size, which is a directive parsed by the assembler. After hours of googling, I can
					# not found any explanation about the absence of RDX parameter in this code. I tried to
					# add a line like "movq $1, %rdx" here as an experiment. And the output length changed.
					# So RDX does specify the length of the buffer to be written. 
					# Finally, I managed to use GDB to monitor the register changes. I created 
					# barebones_explore.s to compare the effect of system call with and without RBX 
					# assignment. I found that if we don't set value in RBX, it has some random big number.
					# I think it should be some garbage data. Fortunately since it's larger enough for
					# the program successfully to print out all the buffer content. But I don't think this
					# kind of behavior is deterministic since we can not guarantee the garbage data always
					# larger than the buffer size. I think we should add a line like "movq $13, %rdx" to
					# make a syscall with all parameter specified.
					# A more interesting observation is: If we just use the random value in the RDX, then
					# the program prints 'Hello World!' and then the return value in RAX is -14, which is
					# an error number. On the other hand, if we set RDX value as 13, then the RAX return
					# value is 13, which means it has written 13 bytes successfully. 
					# As to why this error code(-14) is returned. The errno.h says it means bad address. I
					# read a post on StackOverflow. It says if the buffer is smaller than the buffer size,
					# Then the write function will access a memory address outside the buffer as the buffer
					# size says that piece of memory should be written. This attempt to access an invalid
					# memory address causes an Bad Address error. 
					# So, if RDX is some random large number, system call write will write all the buffer 
					# data and return -14 (Bad Address) when it trys to access the memory address right 
					# after the buffer.



					# (3) What is syscall? We did not talk about this
					# in class.
					# Ans: syscall is an instruction in x86_64. It is used to make a system call 
					# by interrupt. System calls are interfaces between operation system and user program.
					# A user program can use system call to let the kernel perform some system task for it.
					# Such tasks includes IO operations, process management, etc.
	syscall				# Which syscall is being run?
					# Ans: write (system call #1)

					# (4) What would another option be instead of 
					# using a syscall to achieve this?
					# Ans: int $0x80 (Note1: int $0x80 has different parameter register convention. 
					# Note 2: Although this instruction is available in X86_64, it belongs to 
					# X86_32. It's not a good idea to mix them.)
					
	movq	$60, %rax		# (5) We are again setting up another syscall
	movq	$0, %rdi		# What command is it?
					# Ans: exit() (system call #60), it has only one parameter status
					# which is 0 stored in RDI. 
					# _exit will terminate the caller. so the command after this system 
					# call will not be executed.
	syscall

	popq %rbp			# (Note we do not really need
					# this command here after the syscall)

.hello.str:
	.string "Hello World!\n"
	.size	.hello.str,13		# (6) Why is there a 13 here?
					# Ans:13 is the length of the string. 
					# The .string directive is used to define a string in GNU assembler.
					# The .size directive is used to tell ther assembler the size of the data 
					# that the symbol points to.
