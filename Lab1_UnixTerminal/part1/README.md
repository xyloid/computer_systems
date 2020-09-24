# Terminal and Linked List
<img align="right" width="300px" src="https://support.rstudio.com/hc/article_attachments/115019778247/2017-08-11_13-53-02-dropdown-new-v2.png">

> "Refresh! We are now in Systems!"

We have previously done quite a bit of work with data structures such as a linked list. Now we are going to connect it with using our terminal to grab some data and warm up our systems skills.

This will also prepare us for our homework, where we are going to automate some of our lives!

# Part 0 - SSH and git pull

1. First open a terminal and ssh into the Khoury servers ``` ssh khoury_user_name_here@login.khoury.neu.edu ```
2. Navigate to your Monorepo directory(Use the `cd` command to change directory, `ls` to see files, and `pwd` to see your path)
3. When you are within your Monorepo, do a ```git pull```. This will add all of the files I have previously pushed into your repositories.

If for some reason you are having troubles, you can always create a new copy of your monorepo by running `git clone https://github.com/COURSENAME/monorepo-yourUserNameHere` in a separate directory.

# Part 1 - VIM and TMUX Round 2 (Wait--do I really have to use these tools??)

<img align="right" width="300px" src="https://joshldavis.com/img/vim/tabs.gif">

**Yes you should be using VIM and Tmux you are a systems programmer now** :)

### Student and Teacher Dialouge

*"But I love visual studio, sublime, XCode, Atom, etc."*

*That's great, but you cannot use those in a remote environment easily--so I suggest learning another skill. By all means, use both or multiple tools to write the highest quality code.*

*"Hehe, the professor will never know if I just use notepad. I'll just take a shortcut on this assignment"*

*While that's true you can take a shortcut now, it will eventually catch up to you. I promise this is a good skill to learn! This first assignment is much more lightweight, take the time now to do things properly.*

Here are some suggested resources to get started.
* (VIM+TMUX Example) https://www.youtube.com/watch?v=YD9aFIvlQYs (16:31)
* Type *man tmux* in a terminal.
* Tmux tutorial - https://www.youtube.com/watch?v=BHhA_ZKjyxo (11:23)

(This is an ungraded task--only work on this if you finish the deliverable). You will have to do a little bit of work to figure out how to open and read from files for this task.)

# Part 2 - Loading Data (Revisited!)

Our goal is going to be to bring in some data from the real world, and then load it into our C programs using some basic File I/O operations.

## First task - obtain some data

Provided within the lab is a shell script (**data.sh**) to obtain some real world data. You should type in the example shell program provided below into **data.sh** to learn some new terminal commands.. 

```shell
# Boston Red Sox win column selector (Now you know about wget, cut, tail, and output)
wget -O - http://www.mshah.io/comp/Fall18/Systems/Lab/2/baseballdata.csv | cut -d ',' -f 5 | tail -n +2 > data.txt

```

- [wget](https://linux.die.net/man/1/wget) is a program for connecting to online resources and grabbing data
- [cut](https://linux.die.net/man/1/cut) is a program for 'selecting' a column of data.
- [tail](https://linux.die.net/man/1/tail) is a program that retrieves the last *n* lines of a file (head returns the first *n* lines)
- The final `>` mark outputs the file as a .txt file.
- In between each command there is a pipe `|` which gets the output from one command, and makes it the input to the next command.

## Second Task - File I/O

Now that we have some data, let us actually connect what we have done with our script, to working with the data in our program.

Fill in fileread.c with the following example to get an introduction to basic file input/output in C.

```c
#include <stdio.h>
#include <stdlib.h>

int main(){

    // A pointer to a file
    FILE* myFile;

    // We point our file pointer to a file
    // The filepath is 'relative' to where we are
    // i.e. the './' means start searching for files here.
    // So we look for our 'data.txt' that has the Red Sox data.
    myFile = fopen("./data.txt","r");

    // We should check if we successfully opened the file next.
    // Because this is a 'file pointer' we can check if the
    // operation was successful against NULL.
    if(NULL == myFile){
        fprintf(stderr,"data.txt not found, did you run data.sh?");
        fprintf(stderr,"note: stderr argument means we write to a special 'error' output stream.");
        fprintf(stderr,"Program terminating now...");
        exit(1);
    }

    // If we have successfully opened our file, we can now read each line.
    // We will read each line, until we reach the end of the file (EOF).
    char buffer[255]; // At most, each line in file can be 255 characters
    // So we will now can each line from a file(fscanf),
    // and store it as a character string(%s) in buffer each iteration.
    while(fscanf(myFile, "%s", buffer) != EOF){
        printf("Red Sox wins are: %s\n",buffer);
    }

    // Finally, we will close our file
    fclose(myFile);

    return 0;
}
```

## More resources to help

- C File I/O can be further read about here: https://www.tutorialspoint.com/cprogramming/c_file_io.htm
- Hackerrank has an excellent series of shell tutorials here: https://www.hackerrank.com/domains/shell/bash
  - I highly recommend going through them if you are not comfortable with the shell.
- **If you accidently delete something** you can navigate to `cd .snapshot/` which will show files that have been periodically backed up for that current directory. This is yet anothe reason to make sure you are working within the CCIS systems which provide this backup service.

## Deliverable

* Complete the data.sh script to pull in Red Sox wins over time.
  * Make sure to 'add/commit/push' both the data.sh and data.txt file to the repo in this directory.
* Fill in the fileread.c program.
  * Make sure to 'add/commit/push' fileread.c file to the repo in this directory..
  
## Going Further

(Some more programs to research and try out)

1. history
2. tree
3. touch
4. awk
5. sed

- Another task would be to now go back and make a linked list out of the data we have read in. :)
