# TODO Please edit the following information in your assignment

- Name: Yilin Xu
- How many hours did it take you to complete this assignment?
  - About 10 hours.
- Did you collaborate with any other students/TAs/Professors?
  - None
- Did you use any external resources? (Cite them below)
  - https://stackoverflow.com/questions/15376765/how-to-print-string-first-n-bytes-when-the-strings-length-is-greater-than-n
  - http://man7.org/linux/man-pages/man3/strchr.3.html
  - https://linux.die.net/man/3/clock_gettime
- (Optional) What was your favorite part of the assignment?
- (Optional) How would you improve the assignment?

# Logistics

For this assignment (and every assignment/lab), you must login into the servers through `your_khoury_name@login.khoury.neu.edu` to complete and test your work. The reason is the examples I will provide below are compiled strictly for our machines architecture, and this is a consistent architecture where your submission will be graded.

# Part 1 - Single Line Parser

For part one of this assignment, you will need to learn how to do some parsing (or more accurately--lexing a line, you do not have to check for correctness). You have previously done some parsing with the cycle count tool perhaps using functions such as [strcmp](http://www.cplusplus.com/reference/cstring/strcmp/). The little parser we are building now would be similar to the first part of building a compiler or interpeter for a programming language. For our next assignment, we are going to need to use our parser to parse commands users enter into their terminal (i.e. shell). A shell like we have been using can be thought of as an interactive interpreter of a command-line language performing the following:

1. It takes in a line of code (READ)
2. Parses the command (EVALUATES)
3. Then executes the command and its arguments (PRINT output)
4. Loops and then prints out any output. (LOOP)

We call this a [REPL interpreter](https://en.wikipedia.org/wiki/Read–eval–print_loop)

For the first part of this assignment, we are going to break a string into individual tokens (or [lexemes](https://en.wikipedia.org/wiki/Lexical_analysis#Lexeme)).

- The first token will be the command
- The second through *N* number of tokens (i.e. lexemes) will be any number of arguments.
- (Optionally) The N+1 tokens through M tokens will be another command with arguments.

For those who like a more formal definition, a grammar describing how to break each string into individual lexemes looks like the following: 

```command [args]* [ | commmand [args]* ]*```

- *command* is what you type in (e.g. 'ls')
- *args* are any number of arguments (e.g. -l)
- And there may be a pipe ('|') sending our output elsewhere.
	- The pipe ('|') command means that we are taking the previous commands output as using that as input into our next command. 
- The star(asterisk) means that any number of arguments(zero to as infinite) could follow (See [Kleene star](https://en.wikipedia.org/wiki/Kleene_star)).

For part one of this assignment, write a program (**parse.c**) that contains a function to parse a single line of input and and prints out the individual tokens. You may re-use as much as parse.c as you like for implementing your shell for future assignments.

### Example Input and Output

For example (See the line of input and the associated tokens separated onto a new line):

- *./parse "cd ."*
  - ```
    cd
    .
    ```
- *./parse "ls -l"*
  - ```
    ls
    -l
    ```
- *./parse "cat compile | head 2"*
  - ```
    cat
    compile.py
    |
    head
    2
    ```
  
  **Note:** Your program will read in from the command-line the arguments to parse (e.g. *./parse "cat compile | head 2"*) 
  
  **Hint:** Wrapping the command in quotes makes the first argument a string-- and you need to parse a single string.
  
### Additional Hints for Part 1  
  
* The function [strtok](http://www.cplusplus.com/reference/cstring/strtok/) may be useful for splitting up a line of input.
  * `strtok` works by splitting up an input based on a delimeter that indicates a new token is starting(in our example a space). 
  * `strtok` is part of the [string.h](http://www.cplusplus.com/reference/cstring/) library, so be sure to include it. 
* You may use other strategies outside of strtok however--there are many ways to parse your input.
* My expectation is Part 1 of this assignment should be relatively short and you may reused the functions for implementing the mini-shell in the next assignment!

# Part 2 - Compiler Hooks

With our linker trick from lab, we have replaced an existing function that we knew about (or could otherwise decipher if we have additional debugging information). (**Another word of caution** never distribute commercial software compiled with the debugging symbols.)

However, we have learned that compilers at some point have to touch every part of a code base. They are after all, responsible for compiling all of the code! This can make them quite powerful tools, especially if we want to instrument programs and uncover more information. The term *instrument* here means to add additional functionality to some function.

The GCC Compiler in particular has some neat compiler hooks specifically for instrumentation. These *hooks* allow us to add in our own functions and probe a programs behavior.

1. Navigate to the `trace/` folder with `cd trace/` in your terminal.
2. Within it, you will see two files `main.c` and `trace.c`. Both are quite small, investigate them now.
	- What you will see in the trace.c is some oddly named functions--two to be exact. 
	- Each of these functions has an attribute marked on them to not be instrumented (__attribute__((no_instrument_function))
). This notation is specific to the gcc compiler. 
	- Why that is the case? Well, these functions are going to be called at the start and end of every other function that is called in a program. That is, our compiler will automatically inject these functions into every other function call made (This is a similar analogy to a linker injecting some code from a shared library into our project).

1. First compile our main program in the trace folder: `gcc -finstrument-functions -g -c -o main.o main.c `
   1. We have passed an additional flag that leaves 'stubs' for our instrumentation with the `-finstrument-functions` flag.
2. Next, let us compile our trace file: ` gcc -c -o trace.o trace.c `
3. When we have both of our object files, we can then compile them into a single executable binary: `gcc main.o trace.o -o main`
4. Finally, run `./main ` and observe the output.

## Your task

1. Our goal is to monitor main.c. -- that is the file we are going to instrument.
2. Modify the trace.c program to count the total number of functions called.
	1. Your program will print out how many total functions were called at the end of programs execution
3. Next, investigate time.h.
	1. Use the time functions to calculate the total run-time of your program and print out the total elapsed time of your program when it finishes.
	2. Note, if your time returns '0', it may be because there is so little work to do, and the computer is so fast '0' is returned. Think about adding more work to some test functions, and investigating different resolution timers.
	3. (Optional) As a fun experiment, you may search or use your previous C assignments to profile how many times your functions execute and how many seconds it takes to execute each function.
4. Commit your new `trace.c` file to the repository.
5. (Optional) you can print out the address of each function calling each other (demonstrated below)

A sample of your assignment (without time shown) may look like the following.

```shell
-bash-4.2$ ./main
        Function entered 0x4005ff from 0x7fa6f884b445
        Function entered 0x4005cd from 0x40062c
Hello!
        Function 0x4005cd exited from 0x40062c
        Function entered 0x4005cd from 0x40062c
Hello!
        Function 0x4005cd exited from 0x40062c
        Function entered 0x4005cd from 0x40062c
(more)
Total Functions called = 11
(more)
```

# Rubric

Your task now, is to take these compiler hooks and do something more useful. 

1. 50% Part 1 - Single Line Parser
	1. Successful parsing of the examples shown (and several others will be tested).
2. 50% Part 2 - Completing the compiler hooks
	1. Do you successfully report the number of times each function executes?
	2. Do you successfully report how much time each of these functions takes to execute?

# More resources to help

1. Compiler Hooks https://balau82.wordpress.com/2010/10/06/trace-and-profile-function-calls-with-gcc/
2. [string.h tutorialspoint](https://www.tutorialspoint.com/c_standard_library/string_h.htm)
3. Compiler hook tracing: https://mcuoneclipse.com/2015/04/04/poor-mans-trace-free-of-charge-function-entryexit-trace-with-gnu-tools/
4. Measuring time intervals: http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html

# Going Further

(Optional) [Practical Parsing in C](http://www.drdobbs.com/cpp/practical-parsing-for-ansi-c/196603535)

# Feedback Loop
