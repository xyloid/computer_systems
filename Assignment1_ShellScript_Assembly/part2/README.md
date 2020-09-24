# Introduction

For this assignent you are going to be practicing some [Assembly programming](https://en.wikipedia.org/wiki/Assembly_language) and learning other low level skills! For this assignment we are taking a small jump into the assembly. In future courses, you may write some assembly from scratch!

**Note** It is important that you work on the assignment on the Khoury servers (That is where you are testing, and your personal machine may have a different architecture!). You should be using a text editor like vim, nano, or emacs while working remotely. Practicing using these tools over time will help you streamline an optimal workflow weather you are working remotely or on your local desktop.

# Part 1 - GDB Introduction 

GDB is a debugger for the programs that we are compiling with the GCC compiler. It helps us search and step through programs and track down errors in our program. Learning how to debug errors through a debugger is an essential skill for all computer scientists!

1. Watch the following video from Greg Law: *Give me 15 minutes and I'll change your view of GDB* https://www.youtube.com/watch?v=PorfLSr3DDI
2. Next browse this tutorial to get a little more familiar with GDB: https://www.cs.cmu.edu/~gilpin/tutorial/ 
3. Then follow the walkthrough below based on the video to get some practice.
	- This will be potentially helpful for the rest of the assignment!

## GDB Walkthrough with a sample program

Enter the following program below and save it somewhere (It is the same one from the Greg Law video).
```c
#include <stdio.h>

int main(void){
        int i =0;
	printf("Hello, world\n");
        printf("i is %d\n",i);
	i++;
	printf("i is now %d\n",i);

        return 0;
};
```

Compile the program with debug symbols(The *-g* flag does this).
- `gcc hello2.c -g -o hello2`
- `gdb ./hello2`
- Then run the program by typing `r` when inside gdb.

## GDB TUI (Text User Interface)

Here are the instructions for the TUI part of the video roughly written out. Practice using them with the program above.

- Type: `start`
- Type: `list`
- You can type 'next' to move line by line.
- Then press [Ctrl+x a] (Press Ctrl and x at the same time, then press 'a' key after)
  - (Pressing [Ctrl+x a] again will bring you back to just the gdb terminal)
  - Note: If you are having trouble with Ctrl+x, you can type `gdb ./hello -tui` to launch the program.
- Typing in 'next' again moves another line (or simply hit 'enter' which repeats the previous command).
- Ctrl+L will clear the screen after the text prints to clean it up.
- Now try hitting [Ctrl+x 2]
  - You will now see multiple windows with the assembly and the source code.
  - Pressing [Ctrl+x o] will cycle between the windows.
  - Hitting [Ctrl+x 2] a couple of times will cycle through the windows to show what is in the registers.
    - Typing `tui reg` will give some options to see specific registers.
      - For example, `tui reg float` shows the floating registers.
      - For example, `tui reg general` displays the general purpose registers.
 - Hit [Ctrl+x 1] to go back to the first view and see your source code.
   - Typing in some commands like 'layout src' will bring you back to your source view.
     - Typing in 'layout asm' will show you the assembly.
- More: https://sourceware.org/gdb/onlinedocs/gdb/TUI-Commands.html#TUI-Commands
- More: https://sourceware.org/gdb/onlinedocs/gdb/TUI-Keys.html#TUI-Keys
- More resources on GDB TUI: https://sourceware.org/gdb/onlinedocs/gdb/TUI.html
  
## (Optional) GDB Python	

From the Greg Law Video, he demonstrates using an the Python programming langauge within the GDB Tool. Python is a general purpose scripting langauge--perhaps you have even used it! What is neat about Python is that the languages interpreter can be embedded into other tools (and even other programming languages!) as has been shown with GDB.

Here are some of the Python commands written out for your convenience. 
	
- Try the following within GDB:
  - Type: `python print("hello gdb")`
  - Type: `python import os`
  - Type: `python print("my pid is %d" % os.getpid()) # pid = process id`
  - If you have inserted any breakpoints:
    - (Create one with `b main`)
    - Type: `python print(gdb.breakpoints())`
    - Type: `python print(gdb.breakpoints()[0].location)`
    - With Python, you can even create breakpoints!
      - `python gdb.Breakpoint('7')`
    
- Finally typing in `quit` will exit gdb when you are done.

## Your exercise

Run gdb on the program *prog1* that is included in this repository.

It has been compiled with debugging symbols(*-g*) so you may investigate and view the source. Answer the following questions below.

## **TODO (Add your answers here)**

In a brief sentence or two.

1. What was the bug?

    Program received signal SIGSEGV, Segmentation fault. Error code is 6. The cause was a user-mode write resulting in no page being found.

2. What line did the bug occur?

    seg.c: line 15.

3. What is the fix for the bug?

    Check if there is a line of code trying to assign a value to a uninitialized pointer or if there is invalid array index. 

  Note: According to the disassembly, the function ( initialize_front() ) which caused the segmentation fault has an uninitialized local pointer on its stack frame. And then the function is trying to write a value to that pointer, which caused segmentation fault. 


## Aside: Core dump

In the Greg Law video, he begins to investigate a Core dump file. What exactly is a coredump?

Read here: https://wiki.archlinux.org/index.php/Core_dump

# Part 2 - Assembly - A first program

Let us dive into assembly by first writing a 'hello world' in assembly.

- Enter the following text into a file called *hello.s*
- ```asm
	  # Example for x86-64 processor using AT&T Style syntax
	.global _start

	.text

	_start:
		# Write a message to the screen
		mov $1, %rax
		mov $1, %rdi
		mov $message, %rsi
		mov $13, %rdx
		syscall

		# Exit the program
		mov $60, %rax
		xor %rdi, %rdi
		syscall

	message:
		.ascii "Hello, World\n"
  ```
- Save the file and then we will assemble the program using the GNU Assembler (https://en.wikipedia.org/wiki/GNU_Assembler)
  - `gcc -c hello.s`
  - This step builds an object file (.o suffix).
  - We now need to create an executable object file using our previously created hello.o file using a tool called *ld*
  - `ld hello.o -o hello`
    - Run `man ld` to learn more.
  - When this has been completed, push hello.s to your repo, and move on!

There are a few interesting things with this program.
1. First, there is a *global* symbol. The global sets up the starting point, as our program needs some entry point. You could think of '_start' as 'main' like in a C program. You can learn more here: http://web.mit.edu/gnu/doc/html/as_7.html#SEC89
2. The next directive (remember, lines that start with a '.' are directives) is .text. These are where our instructions start. https://stackoverflow.com/questions/14544068/what-are-data-and-text-in-x86
3. The next few lines are moving some values into registers. The first statement moves the immediate value $1 into register %rax. A few more lines down we see a syscall.
	  1. What is a syscall? In short, it is a call to a function built into the operating system (More here: https://www.geeksforgeeks.org/operating-system-introduction-system-call/).
	  2. To figure out which syscall it is, use this resource: https://filippo.io/linux-syscall-table/
4. Finally at the end there is a label ('message:') with a string literal (.ascii "Hello, World\n"). https://docs.oracle.com/cd/E26502_01/html/E28388/eoiyg.html

## Aside - Machine Representation of Numbers

  * Note: it may be beneficial to look at this ascii table to see how numbers and letters are represented. https://www.asciitable.com Remember, we do not have a 'text' datatype in assembly. Text instead is represented by numbers shown in the ascii table.

# Part 3 - Cycle Count Tool

You may be wondering how good are compilers at generating assembly from our C code (Similar to the way you may have a good and bad translator when communicating in different languages)? How can we be sure that our compiler is generating efficient assembly? There are afterall, many different ways to write a program that generates the same output.

Part of being a good systems programmer, is the ability to build tools that measure. In this case, you are going to write a small parser in C that counts the number of instructions and counts how many instructions are needed to run that program from top to bottom.

As an example, if an assembly program had the following code:

```asm
MOVQ a, %rax
MOVQ b, %rbx
ADDQ %rbx, %rax
IMULQ %rbx
MOVQ %rax, c
```

Your tool will report a summary of the total Instructions(i.e. ADDQ, MOVQ, IMUQ, etc.) found. Additionally, you will estimate the total cycles needed for hardware to execute this code.

```asm
ADDQ 1
MOVQ 3
IMULQ 1

Total Instructions = 5
Total Cycles = ???
```

* Your tool should read in a file name through the command line arguments for what file is being analyzed (e.g. ./tool barbones.s)
  * (hint, investigate what argc and argv are).
* barebones.s is provided to be used as an example input file for your tool.
  * Within barebones, **there are some questions for you to answer**. Modify the source file with your answers.
* You will modify a file called tool.c which you will implement your tool in. 
  * At the very least, your program should output counts for: ADD, SUB, MUL, DIV, MOV, LEA, PUSH, POP, RET.
    * i.e. For your analysis (and for the sake of simplicity), consider ADDQ, ADDB, ADDL, etc. each as an 'ADD' instruction.
* You may find https://fresh2refresh.com/c-programming/c-strings/c-strcat-function/ helpful for learning some string processing in C.
* Use the Agner Fog's instruction table (http://www.agner.org/optimize/instruction_tables.pdf) to estimate how many cycles the program will run. That is, take an instruction like MOV which typically takes 1 cycle, and sum them together and output the result.
  * Note that this is again an estimate (MOV with different data types itself takes different number of cycles! So your answer will be an approximation. I am expecting there to be some variance, even within the class--attempt something reasonable).
 
## Static Analysis 
 
Our Cycle Count Tool is what we call a 'static analysis'. That is, it uncovers information about our programs before it is running (during compile-time). Given that our tool uncovers information before the program, what is (at least) one pro of this, and one con you can think of?

Pro:
1. *Some bug of the program could be harmful to the computer. If the operation system doesn't have too much protection, then it's better to use static analysis before execution. *

2. *In static analysis, we could review all the code of the program, thus we can perform a more thorough inspection on some aspects of the code.*

Con:
1. *There could be some variables whose values will only be determined during the execution. Thus, static analysis could not check if those variables are correct.*

2. *If the program will generate a random variable, then static analysis cannot be used to check the distribution of that random variable.*

## Dyanmic Analysis

The opposite of a static analysis is a dynamic analysis. Dynamic analysis tools record and return information about programs that are in the process or have finished executing. An example of a dynamic analysis tool is [valgrind](http://valgrind.org/). What do you think a pro and con of dynamic analysis would be?

Pro:
1. *Some variables in the program can only be detetrmined during the execution, dynamic analysis can help us check if those values are correct.*

2. *The actual time complexity of the program could not be measured without any execution. So we must use dynamic analysis to gather these kind of informtion.*

Con:
1. *The program must be executed in dynamic analysis, but some bug might be harmful to the computer. We need some way to protect the computer.*

2. *In dynamic analysis, we might only have the binary file of the program. Then it's hard to find out the details of the program in this blackbox situation. Although we might able to use a disassembler, disassembly is hard to understand.*


# Rubric

- 20% Part 1 - GDB
  - Did you modify the README with the correct answers to what the bug was in prog1?
- 20% Part 2 - Assembly program
  - 10% for a correct output.
  - 10% for structure of program (no weird formatting, no wasteful instructions)
- 60% Part 3 - Cycle Count Tool
  - 10% Did you modify the comments and answer the questions in barebones.s
  - 40% Does your cycle count tool work? 
  	- i.e. no memory leaks
	- Number of instructions are correctly counted
		- (Are MoVq, MOVQ, or movq all counted?)
	- *Some* approximation of the cycles is given.
  - 10% Did you answer the questions on the static analysis and dynamic analysis?

# Resources to help

- A full 90+ minutes of GDB debugging
	- https://www.youtube.com/watch?v=713ay4bZUrw
- A handy syscall table
	- https://filippo.io/linux-syscall-table/

# Feedback Loop

(An optional task that will reinforce your learning throughout the semester)

- Continue using the Compiler Explorer as you write your C code. https://godbolt.org

