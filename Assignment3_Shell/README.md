TODO Please edit the following information in your assignment

- Name: Yilin Xu
- How many hours did it take you to complete this assignment?
  - About 30 hours.
- Did you collaborate with any other students/TAs/Professors?
- Did you use any external resources? (Cite them below)
  - https://en.wikipedia.org/wiki/Null-terminated_string
  - https://linux.die.net/man/3/execvp
  - http://man7.org/linux/man-pages/man2/waitpid.2.html
  - https://stackoverflow.com/questions/31907212/will-ctrlc-send-sigint-signals-to-both-parent-and-child-processes-in-linux
  - https://stackoverflow.com/questions/33655216/in-a-basic-shell-program-in-c-how-to-exit-all-processes-associated-with-a-paren
- (Optional) What was your favorite part of the assignment?
- (Optional) How would you improve the assignment?
  - Try to implement arrow key. I found arrow key is platform dependent. Since ncurses is not supported on the Khoury server (according to comments in simulator.c of Lab3), it seems to be time consuming to implement it from scratch. So i didn't do it this time.


# Logistics

For this assignment, you must login into the servers through `your_khoury_name@login.khoury.neu.edu` to complete the assignment. The reason is because the examples I will provide below are compiled strictly for our machines architecture, and this is where your submission will be graded.

# README Please :)

**I strongly recommend** you do most of your testing on your own [linux virtual box you have previously setup](https://www.google.com/search?rlz=1C1CHBF_enUS798US798&ei=t7GTXMb3JIOZ_QbEwZeYAg&q=setting+up+a+linux+virtualbox&oq=setting+up+a+linux+virtualbox&gs_l=psy-ab.3...7203.8384..8512...0.0..0.96.592.8......0....1..gws-wiz.......0i71j35i304i39j0i7i30j0i8i7i30j0i13i30j33i10.wdadpiXRdZ8). Executing a shell that forks indefinitely (i.e. a [fork bomb](https://en.wikipedia.org/wiki/Fork_bomb)) **slows down the servers for everyone** (and may prevent you from logging in). I will not grant individual extensions because you blocked yourself and your classmates out from a shared resource.

# Introduction

For this assignment, you will be writing your very own shell-- the mini-shell!

<img src="./shell.png" alt="An example shell"/>

[Shell's](https://en.wikipedia.org/wiki/Shell_(computing)) (or sometimes called command-line interfaces) are text based applications that serve as an intermediate between the user and the operating system. The shell is also a tool for how a user interacts with the operating system. Previously, we have written many scripts that our shell can execute by running various programs. The shell that you build with this assignment will be something you can continually expand on in the future, and perhaps distribute as your own one day! Let us take a closer look at the parts to get started.
 
# Part 1 - Signals

In your shell, you will have to implement a signal handler. We have very briefly discussed signal handlers, so make sure you are familiar with the concept.

Provided below is an example that you can use.

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> // This is new!


// Create a signal handler
void sigint_handler(int sig){
	// Ask yourself why 35?
	write(1,"Terminating through signal handler\n",35); 
	exit(0);
}

int main(){

	// Install our signal handler
	signal(SIGINT, sigint_handler);

	printf("You can only terminate by pressing Ctrl+C\n");

	while(1){
		printf("Running forever!\n");
		sleep(1);
	}

	return 0;
}
```

* The following page provides details about signals: https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_24.html
* The following article provides a nice introduction as well: https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/

# Part 2 - Mini-Shell

You are now ready to create Mini-Shell. Mini-Shell is inspired by other popular shells. The good news is you can validate the behavior of your shell against the Unix terminal to ensure it is working.

At a high level, implementing mini-shell takes the following components:
1. You will have a function that reads in commands a user types.
2. Your program will execute in an infinite loop.
3. Your program will execute child processes from the shell (the shell is the parent process) based on the command.

## The specifications for your shell

1. You should have a prompt "mini-shell>" in front of each command that is typed out.
	- (If you want to be creative you can include the machine name or other information you can derive).
2. The maximum input size(i.e. BUFFER SIZE) on a given line will be 80 characters for our shell. You can specify this as a constant global variable.
3. Implement a signal handler so when the user presses 'Ctrl+C' the text 'mini-shell terminated' is printed out.
	- Make sure that all child processes are terminated when this occurs!
4. When you launch a new child process from your shell, the child process should run in the foreground by default until it is completed.
	- Here are some examples: https://www.tutorialspoint.com/unix/unix-processes.htm
5. If the user adds the pipe '|' then the output of one process should be the input to the next process.
	- You can assume only one '|' will be used at most. e.g. `ls -l | wc`
	- Here are some examples: https://www.tutorialspoint.com/unix/unix-pipes-filters.htm
6. Your shell should have some built-in functions that you will have to implement (i.e. functions written in C code). These include 'exit' to terminate your shell, 'cd' so you may change directories, and 'help' explaining all the built-in commands available in your shell.
	1. You can test `cd` by moving up or down the directory tree.
		- (Hint the C library has commands for this)
	2. `help` should print out all of the built-in commands that you provide (including your user made one).
	3. `exit` should terminate the shell 
		- Note: If you launch another shell from within your shell, exit should only close out of the most recently launched shell.
7. You will want to think about how to store these built-in commands.
	- I will hint that using an array with commands and an array of function pointers is one possibly effective solution.
	- What is a function pointer? https://www.geeksforgeeks.org/function-pointer-in-c/
	- You can further think about what an array of function pointers looks like and why this may be appropriate.
8. If a command is not found, your shell should print out an error message, and resume execution.
	- e.g. `mini-shell>Command not found--Did you mean something else?`
9. You need to implement and document 1 addditional built-in command that you choose here. 
	1. This built-in command can be of your choosing, but should not be trivial (i.e. not just print a message or clear the screen).
		- An example might be 'history' which stores all of the previous commands a user has typed in and prints them out.
		- Another example may be 'guessinggame' which plays a little game in the terminal for guessing random numbers.
		- Another example may be when the user presses 'tab' a list of programs that can run are found.
		- You can be creative here. The constraint is that you are implementing this functionality in C, and not calling a script.
	2. (Edit the readme below with your built-in command)

-----------------------------------------------------
NAME: 

**hist** - command history

SYNOPSIS: 

Command hist is used to print a number of recent commands the user typed.
	
**hist [NUMBER]**

DESCRIPTION:

By default, **hist** will print 10 most recent commands the user typed, from the earliest one to the latest one excluding the current one, that is, the **hist** itself. The number of recent commands to be printed can be specified by the optional argument *NUMBER*. Note, *NUMBER* must be with in the range [0, `HIST_MAX-1`]. The most recent command will always be 'hist'. So it will not be printed in its own output. 
NOTES:

   - The value of `HIST_MAX` is 11. It is defined in shell.c. A small value is chosen so that the command is easier to be tested.
   - A data struct `record_t` is defined. It contains an array of `char` pointers. This array is used to implement a simplified ring buffer. Thus, the latest commands will be stored in the buffer.
   - A function `add_log` is defined. It is used to add command to the record buffer.
   - A buffer is created on the stack and then the addresses are assigned to the array of pointers in a `record_t` variable. So malloc is avoided which prevents memory leak when the mini-shell is terminated by `Ctrl+c`.

-----------------------------------------------------

## Some helpful things to ponder

1. You are running programs in your shell. The programs that are being run are found in the 'bin' directory. For example, you would find a program called 'cat' or 'ls' there, which someone wrote in C for you.
2. You can also execute programs from a relative or absolute filepath (e.g. ./minishell)
3. There is not a program called 'cd' in the 'bin' directory, which means it must be built into the actual shell.

Provided below is an example for program that forks a new process, executes it, and then resumes execution.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){

	char* myargv[16];
	myargv[0]="/bin/ls";
	myargv[1]="-F";
	myargv[2]=NULL; // Terminate the argument list
	
	if(fork()==0){
		// Executes command from child then terminates our process
		execve(myargv[0],myargv,NULL);
		printf("Child: Should never get here\n");
		exit(1);
	}else {
		wait(NULL); // This call makes the parent wait on its children.
	}

	printf("Finally, print from the parent\n");
	
	return 0;
}
```


# Rubric

3. 100% for completing Part 2 - Mini-Shell (rough rubric below)
	1. 20% 1 Built-in command of your choice (and documentation)
		1. e.g. A 'clear' or print command will not earn full marks. Do something creative! (Search the web for examples)
	2. 20% 3 built-in commands (cd, help, exit).
	3. 10% A working signal handler
	4. 30% A shell that runs forever and has the 'mini-shell>' prompt and properly parses programs.
		- You should avoid [fork bombs](https://en.wikipedia.org/wiki/Fork_bomb). As you are learning the fork command, it will be helpful to test on virtual box as opposed to the Khoury servers. :)
	5. 20% Can successfully execute processse in the foreground.
		- 10% for a working pipe.

# Resources to help

- (See links above)
- And otherwise think about commands like fork, execvp, wait, waitpid, getpid, etc. :)

### Glossary and Tips
1. Commit your code changes to the repository often, this is best practice.
2. Do not commit your .o file or your executable file to the repository, this is considered bad practice!
3. On Functionality/Style
	1. You should have comments in your code to explain what is going on, and use good style (80 characters per line maximum for example, functions that are less than 50 lines, well named variables, well named functions etc.).
4. You may use any stdio.h functions that are useful, such as getline. You are welcome to implement these functions from scratch if you like however.
	1. You will want to include 'signal.h' as well for your signal handlers.
5. Careful when parsing. Things like newline characters '\n' like to sneak in and may make it difficult for strcmp to provide accurate results. "teststring" != "teststring\n"	

# Going Further

You might consider some of the following optional features in your shell.
1. Switching processes between forground and background (fg and bg commands).
2. Handling the '|', pipe  between an arbitrary number of processes.
3. Handle tab completion or moving through the history of commands with arrowkeys (This is a good example of a built-in command).

# Feedback Loop

(An optional task that will reinforce your learning throughout the semester)

Implementing a shell is one of the more fun exercises in learning C programming (in my opinion). However, it is quite relevant, especially in the context of the web browser and other applications (video games for example, often have a real-time shell interface for debugging).

Choose one of the following 3 papers to skim, and you can add notes here with two interesting things you discovered (Note this is not graded and completely optional).

- Grocoli: https://dl.acm.org/citation.cfm?id=2442019
- Integrating Command Shell in Web Browser: https://www.usenix.org/techsessionssummary/integrating-command-shell-web-browser
- Inky: http://people.csail.mit.edu/emax/papers/uist08-inky/uist08-inky.pdf
