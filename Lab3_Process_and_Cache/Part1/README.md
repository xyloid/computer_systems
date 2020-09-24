# Process API

In today's lab, we are going to explore the process API to get familiar with launching processes from within our program. Provided in this lab are several code examples showcasing several ways to launch and control processses. 

# Part 1 - fork

Remember that when we use fork(), we are creating a child processes that is identical to the parent process. The child process gets a copy of everything that the parent had, but the memory is distinct, such that modificiations in either the parent or child do not effect each other.

Let us go ahead and try a simple example with fork.

## example1.c

The following program is our version of a "hello world" to the fork command.

```c
#include <stdio.h>
#include <sys/types.h>	// Includes some new data types (e.g. pid_t)
#include <unistd.h>	// This is a new library that includes various unix system calls.

int main(){

	pid_t pid;
	int x = 1;

	pid = fork();

	if(pid==0){
		printf("child: x=%d\n",++x);
		return 0;	
	}

	// Parent
	printf("parent: x=%d\n", --x);

	return 0;
}

```

* Save and compile the code: `gcc example1.c -o example1`
* **Before running:** Predict with your partner what the result will be.
* Run the following program with: `./example1`

Let's try another example this time:

## example2.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){

	pid_t pid;
	int x = 1;

	pid = fork();

	if(pid==0){
		printf("Terminating Child, PID= x=%d\n",getpid());
		exit(0);	
	}else{
    	    // Run the parent
	    printf("Running parent forever(pretend I am a server): pid=%d\n",getpid());
    	    while(1){
    	    }
  	}

	return 0;
}
```

* Save and compile the code: `gcc example2.c -o example2`
* Run the following program: `./example2 &`
* **Discuss with your partner** What happens when you compile and run this code? 
	* (Run with `./example2 &` the ampersand returns control back to shell.

Type `ps` into the terminal. You will notice that one of the processes is labeled <defunct>, and that id will actually match our childs pid. What happened here, is that we have not reclaimed (or reaped) our children. Our child is said to be a zombie process, in that it is occupying and using resources, but not doing any useful work.

Let us get rid of our processes now. Type `kill somenumber` where somenumber is the pid (process id) of your parent and child process (Note: that killing the parent process should kill the child process as well. See the [kill](http://man7.org/linux/man-pages/man2/kill.2.html) system call.).
  
Now what if we flip the scenario where our child runs forever?

## example3.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){

	pid_t pid;
	int x = 1;

	pid = fork();

    if(pid==0){
    	printf("Running child forever now pid=%d\n",getpid());
    	while(1){
    	}
    }else{
	// Run the parrent
	printf("Terminating parent pid=%d\n",getpid());
	exit(0);
    }

	return 0;
}
```

* Save and compile the code: `gcc example3.c -o example3`
* Run the following program: `./example3 &`

We will have the same problem, and we need to run the `ps` command again(to see which processes are running), and then the `kill some_pid_number` command.

## Synchronization

There is a problem we have seen in the previous example where we did not properly enforce the order in which each process executed. So what is one potential solution to fix this problem?

The answer is to use a synchronization function called `wait`. `int wait(int *child_status)` will make the parent wait to execute after one or more child process(es) has finished.

## example4.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h> // Library with the 'wait' system call.
#include <unistd.h>

int main(){

    int child_status;

    if(fork()==0){
        printf("Child runs!\n");
        exit(0); // and exits properly
    }else{
        printf("Hello from parent\n");
        wait(&child_status);

        // If we want to do something with the child_status here is an example.
        // Typically we can wait(NULL), but within our parent we may want
        // to capture how the child exited(maybe it exited with some error code).
        // e.g. change 'exit(0)' to 'exit(5)' in the block of code executed by
        // the child.
        if(WEXITSTATUS(child_status)>=0){
            printf("Child exited with the following child_status %d\n",WEXITSTATUS(child_status));
        }
        printf("Child always is terminated by this point--parent resuming\n");
    }

    printf("This always prints last\n");

    // It is important to note that our parent should always wait for all
    // of its child processes to exit before proceeding.
    // If the parent ends first(i.e. forgetting to call wait), the
    // child process is 'orphaned' and becomes a zombie process that
    // cannot be otherwise terminated.

    return 0;
}
```

* Save and compile the code: `gcc example4.c -o example4`
* Run the following program: `./example4`
* Take a moment to read about orphan and zombie processes here: 
	* http://linuxg.net/what-are-zombie-and-orphan-processes-and-how-to-kill-them/
	* Understand what we saw in example 2 and 3 we created a zombie and orphan.
	* Understand also how to find and kill any zombie and orphan processes.
	
## example5.c

### execve

Sometimes we will want to not just launch a new piece of code, but rather execute an entire program as a separate process from within our process. We have been previously doing this using our terminal. In order to execute other programs (i.e. a more specific process), we have the [execve](http://man7.org/linux/man-pages/man2/execve.2.html) command.

- The execve command: `int execve(char* filename, char* argv[], char*envp[])`
- filename: This is the script or file we are going to execute (i.e. this is equivalent to typing it in the shell)
- argv: This is our argument list (which we are familiar with). 
	- As an example: argv[0] = filename of the program we want to execute
- envp: These are any specific environment variables we want to set.
  - More information can be found here: [https://en.wikipedia.org/wiki/Environment_variable]

Let's try a simple example using execve to launch the 'ls' program from within our program.

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){

	char* myargv[16];	// We can store up to 16 arguments.
	myargv[0]="/bin/ls";	// Our first argument is the program we want to launch.
	myargv[1]="-F";		// Any additional arguments (i.e. flags) we want to make use of.
	myargv[2]=NULL; 	// Terminate the argument list--similar to how we would terminate a character string.
				// (i.e. Set the last argument to NULL if we have no more flags.)
	if(fork()==0){
		// Executes command from child then terminates our process
		// Note: There are other 'exec' functions that may be helpful.
		execve(myargv[0],myargv,NULL);
		printf("Child: Should never get here\n");
		exit(1);
	}else{
		wait(NULL); // handy synchronization function again!
		printf("This always prints last\n");
	}
	
	return 0;
}
```

* Save and compile the code: `gcc example5.c -o example5`
* Run the following program: `./example5`
* Go ahead and modify the above code and try to run some other processes like 'cat'.
* **Discuss and try with your partner** Will the code work if I change `myargv[0]="ls"`. Why do you think this is? What did you discover?
	* Some hints to the previous question
		* https://phoenixts.com/blog/environment-variables-in-linux/ 
		* https://www.cyberciti.biz/faq/linux-list-all-environment-variables-env-command/

## Your task - myprogram.c

Now that you have learned how to use fork(), exec(), and wait() you will write your own program utilizing these system calls.

### Task

- Write a small program(myprogram.c) using fork and/or execve to execute a series of at least 3 other commands/programs.
  - example5.c is a program that executes exactly 1 command, you may expand on this example in your myprogram.c
  - You are welcome to be as creative as you like!

# Part 2 - Terminal Programs

To get some practice with understand processes, run the following commands: 

- `ls proc/` (Why ls here? What if you just type `/proc`)
- top
- ps
- kill (yes, I know it is a rather aggressive command name--yikes!)

It will be further useful to read the `man` pages for each of the commands.

# Lab Deliverable

- Commit the source code for the 5 examples (example1.c example2.c example3.c example4.c example5.c).
- Commit your source code for myprogram.c

# More resources to help

- https://www.networkworld.com/article/2693548/unix-viewing-your-processes-through-the-eyes-of-proc.html

# Going Further

- Investigate the different command shells: https://en.wikipedia.org/wiki/Comparison_of_command_shells

# F.A.Q.

## Difference between Zombie Process, Orphaned Process and Daemon Process

* Zombie Process - a zombie process (same as <defunct>) is a process that has completed execution but still has an entry in the process table and this happens when the termination of the process has not been collected by the parent (example2.c)
* Orphaned Process - process whose parent process has finished or terminated, though it remains running itself. (example3.c)
* Daemon Process - Daemon is a computer program that runs as a background process, every daemon process is an orphaned process.
