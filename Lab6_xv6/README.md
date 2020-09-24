# Lab 6 xv6


<img src="https://images.techhive.com/images/idge/imported/article/ctw/2009/06/04/unix_chart_775-100397147-orig.jpg" alt="Unix operating systems" width="400px" align="right"> In today's lab we are going to be exploring and working in the xv6 operating system. This is a Unix inspired operating system that is a manageable size to work with in a lab (The actual [Linux Kernel is several millions lines of code](https://unix.stackexchange.com/questions/223746/why-is-the-linux-kernel-15-million-lines-of-code)!). That being said, this is still a large piece of software, and getting experience working with mid to large sized source code you are not familiar with is an important skill!

The tasks for today's lab are:

* [x] Install an emulator
* [x] Download xv6 for github, and build an img file using 'make'
* [ ] Modify an unfamiliar piece of source code (i.e. xv6)

# Part 1 - Installation and running xv6

For today's lab, you will be running xv6 on your personal machine. Please see the [InstallationInstructions.md](./InstallationInstructions.md) if you have not yet been able to successfully install and run xv6 in the QEMU emulator for your operating system.

# Part 2 - Where do we begin? grep and exploring

## Task 1 - Grep'ing around to learn part 1

Remember the `grep` command? (`man grep`) It is a handy way to search text, and also a great way to navigate large projects. So if you have a large project like xv6, where would you first start? 

1. Step 1 -- find the main function.
2. If I was grep'ping around the source I would likely start with something like: `grep -ir -e "main(" .`
  - 'i' means case-insensitive. 'r' means do a recursive search 'e' is the pattern 'main(' which I am looking for, and the '.' at the end tells me where to start this search.
  - After this investigation, I find `main.c` which contains the main function. Starting from main.c is a good way to start understanding a programs [control flow](https://en.wikipedia.org/wiki/Control_flow).

## Getting familiar with xv6.

1. Launch qemu and begin to run xv6 for your respective operating system.
2. While running the emulator type `ls` and you will see all of the commands you have available.
    - (While xv6 does not have as many as we have available in Unix--it is a start!)

**Discuss with your partner:** *List five commands you observe with your partner that look familiar from Unix*

1. *ls*
2. *cat*
3. *echo*
4. *mkdir*
5. *rm*

# Part 3 - Extending xv6

Remember that applications like ls, wc, cat, are all programs that someone has written and compiled as a binary. Let us add a simple binary to our xv6 operating system. This will also give us an idea of how the build process works for xv6, and how we can extend our operating systems functionality.

- Make sure you have exited the emulator.
    - As a reminder, "Ctrl+a" then tapping 'x' will terminate the emulator.
- Within the root directory of the public-xv6 directory you will see all binaries for different programs and their corresponding .c files.
- Let's emulate the style of command by copying a previously created file: `cp echo.c mycommand.c`
    - Reminder: You are doing this outside of the emulator.

Here is the code for our command:

```c
// mycommand.c
//
// Three files are included.
// None of which are the usual
// '<stdio.h>' we are use to seeing.
// (This is because we have to use code libraries provided by xv6)
#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]){
    
    int i;
    for(i = 1; i < argc; i++){
        // Remember what the '1' is as the first argument?
        printf(1, "%s%s", argv[i], i+1 < argc ? " " : "\n");
    }
    // An exit call rather than return statement    
    exit();
}
```

1. Now in order for this to be integrated into the build process, we need to modify our 'MakeFile' file.
2. Open the 'MakeFile' in vim and modify the following:
    - Under UPROGS add: `_mycommand\` (Emulate the exact style, do not forget the slash at the end)
    - Under EXTRA add: 'mycommand.c'
    - Now type 'make' and repeat your build process you did earlier to rebuild the operating system.
      - Note that 'make' is typically smart enough to only rebuild the files that have been changed.
3. You can now do a test by launching xv6, and then typing 'ls'. 
    - You should see your command listed.
4. Now try typing `mycommand test` and you should see the same output you would as if you did `echo test`
    - (We can begin appreciating the work that has gone into our modern operating systems!)

# Part 4 Grep'ing around to learn

- Now investigate each of the header files:
    - types.h, stat.h, and user.h in vim. Take a few minutes to see what you find.

**Discuss with your partner:** 

- *In a sentence, what does types.h contain?*
    - *This file contains the aliases for some unsigned c primary data types, including `unsigned int`, `unsigned short`, and `unsigned char`.*
- *In a sentence, what does stat.h contain?*
    - *This file contains the definition of `struct stat` whose fields represent the status of a file.*
- *In a sentence, what does user.h contain? What lists of functions seem particularly interesting/useful?*
    - *It contains two lists of functions, one of which contains system calls and the other contains some c standard library functions. The c standard library functions are more useful because they are widely used in both OS and application software.*


# Part 5 - Implementing a System Call (Extending xv6 further).

(Reference Chapter 3 for more: https://pdos.csail.mit.edu/6.828/2014/xv6/book-rev8.pdf)

We have seen conceptually how system calls worked, and even used them in assembly(e.g. write) and our C code (e.g. brk). When we use these system calls, our user programs are requesting the kernel to perform some action. In order for the kernel to perform this action, a [context switch](https://en.wikipedia.org/wiki/Context_switch) occurs, the action is performed, and then the user program gets control again.

Recall at a high level what happens during a system call.

1. A trap is issued. This stops the process from running.
2. The process needs its registers saved.
3. We switch to kernel mode and perform work.
4. Restore our registers with any applied changes as we give the user program back control.

## Getting started

Let us first take a look at syscalls that are already implemented and walk through. The below is my work process and how I 'play around' when in unfamiliar codebases. The question I am trying to answer is **What does it require to make a system call?** (Walk through these steps)

- I have some hunch where these system calls live, but I grep anyway `grep -ir -e "sys" --include="*.h" .` this time looking in the header file for some hints.
- I see syscall.h pops up often and looks releveant, so I will open that file.
    - Modify syscall.h and add a syscall like:
    -  `#define SYS_mike 22`
- At this point, I might look in syscall.c the corresponding file to our header file.
    - After browsing this file I notice a few things.
      - One is that the comments at the top of the file discuss a requirement to save program counter.
      - The second is there are a few helper functions for working with the system call(e.g. syscall(void).
      - The third is there is a list of function prototypes, and a jump table with function pointers we might need to add to.
      - Let's go ahead and add to syscall.c
        - `extern int sys_mike(void) // Add this around line 106`
        - `[SYS_mike]  sys_mike, // This line in the list of function pointers around line 130`


- Now at this point, I am almost completely lost. Let's continue grep'ping around.
- Let's look at the other header files within syscall.c starting with types.h
    - types.h -- nothing too interseting in here (you already saw this). Let's move along
    - defs.h -- ah, okay some interesting stuff here. 
      - More lists of things, maybe there are other system calls here?
      - In fact next, I will add `int      mike(void); // under proc.c which falls around line 123.`
- Let us continue our tour from syscall.h and next open up param.h
    - This file shows us some more configurations of our xv6 operating system.


- Let's do some more grep'ing and see where we need to add our system call.
    - `wait` is another system call that I know of and it fell under proc.c (and it matches our functions signature). What I really want to find is its function prototype, or where it is defined.
    - I can look back at defs.h and see the form.
    - I try: `grep -ir 'int wait(.*);' .` and find users.h has definitions.
    - Surely enough when I look in user.h, I see in the comments 'system calls' (Ah--I love when folks comment their code!)
      - Add `int mike(void);` to the list.
    
- Okay, now I want to figure out where to implement 'mike' (or perhaps you named your sys_call something more creative).
    - I'm going to look to *wait* again, and try `grep -ir "wait(*.*)" .` with my goal being to find the actual implementation.
    - I get lots of hits, so I narrow this down slightly.
        - Knowing that an implementation is a .c file, I try: `grep --include="*.c" -ir 'wait(*.*)' .`
        - At this point I make an educated guess that the usertests.c file is not useful.
            - As I browse up I do see a prototype, and see it is of the form sys_wait(void) in syscall.c. I also see sysproc.c has this same *sys_mysystemcall* pattern as well.
        - One more grep just to be sure: `grep --include="*.c" -ir 'sys_wait(*.*)' .`
    - In sysproc.c add at the very bottom
      ```c
      // Our custom system call
      int sys_mike(void){
          return mike();
      }
      ```
    - In syscall.c we already did everything
        - (*sys_mysyssetmcall* pattern we saw in the functional pointer table was handy for grepping)
      
Okay, so we have done quite a bit so far, but we are not *quite* done.

- During my greps I have noticed `SYSCALL(wait)` appears quite often. After more investigation, `usys.S` is the responsible file that needs modified.
    - Add `SYSCALL(mike)` at the end near line 32.
- Finally after one more grep `grep -ir 'wait(void)' .` we see that proc.c is where we can implement our command.
    - Add at the very end of the file
    ```c
      int
    mike(void){
        // Let's just print hello
        cprintf("hello from mike syscall\n");

        return 22; // Our system call number
    }
    ```
  
  
**Final Step**

* Type: `make` and rebuild qemu (Otherwise follolwing the steps to build for your respective operating system). If all worked out well, xv6 should rebuild again.
  

# Part 6 - Testing your system call.
  
Okay, now we just need to make a program that uses our syscall. 

1. Exit the emulator and create a file called `mike.c`.
    - Think about what goes in this program, a solution is in the appendix.
2. Add `mike.c` to our MakeFile as we did before.
    - Under EXTRA add 'mike.c'
    - under UPROGS add '_mike'
3. We will need to rerun 'make' again and then launch qemu.
    - If for some reason make says it is up-to-date, run `make clean` to make sure all old files are deleted, then run make again.
4. To test our system call, run the `./mike` user program, which simply makes a system call. 
  
* Note: While it is an honor to have a syscall named 'mike', sometimes naming things "weird" will make them easier to grep for when first learning. 
* Note: If things did not work for you, our worse case scenario if something is really messed up is to download xv6 again.

## Lab Deliverable

- Fill in all of the discussion sections.
- Implement a syscall 'mike' or whatever you choose with a test program.
    - Commit 'mike.c', 'mycommand.c' and your 'MakeFile' to the repository.
    - **Do not commit** the xv6_public dirctory, it is too large! Note that if you were, you would want to run `make clean` first, and make sure not to include any large object files or .img files however (which can always be built from source, and they would otherwise use a lot of space in the repository).


## More Resources to Help

* xv6 is fairly widely used for operating systems exercises. Googling around for install trouble will likely yield a working result.
* If you want to debug xv6 while developing, you can type `make-qemu-gdb` When launching this will give you a process id that you can attach to (You will see it as tcp:12345, where 12345 is shown). Then launch gdb. Then type `target gdb remote:12345`
* More on grep: https://www.gnu.org/software/grep/manual/html_node/Regular-Expressions.html


## Going Further

- Implement a syscall that prints out process information.
- Implement any other syscalls you think may be useful!

## Appendix

```c
// mike.c
#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]){

    mike();

    exit();
}
```
