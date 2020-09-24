TODO Please edit the following information in your assignment

- Name: Yilin Xu
- How many hours did it take you to complete this assignment?
  - About 20 hours.
- Did you collaborate with any other students/TAs/Professors?
- Did you use any external resources? (Cite them below)
  - https://stackoverflow.com/questions/6477500/explain-opaque-data-type-in-c
  - https://users.cs.cf.ac.uk/Dave.Marshall/C/node30.html
- (Optional) What was your favorite part of the assignment?
- (Optional) How would you improve the assignment?
  - The paint function can be simplified. 


# Logistics

For this assignment, you must login into the servers. `your_khoury_name@login.khoury.neu.edu` to complete the assignment. The reason is because the examples I will provide below are compiled strictly for our machines architecture, and this is where I will test your code.

# Assignment Strategy

* Part 0-3 are all related.
* Part 4 can be independently completed if you get stuck in parts 1-3. The motivation for part 4, is you may be overwhelmed by looking at lots of C code you have not written (i.e. Part 0-3). Part 4 is an opportunity to practice your fundamentals.

## SSH and images

Typically when we ssh, we do so from a terminal window. Since we are working with images, we will want some sort of graphical output  if you are not working locally from your machine (e.g. on an Ubuntu virtual box instance). There is a program called 'display' that you can run in order to view images.

Unfortunately, if we run `display image_name.ppm` when ssh'd into login.khoury.neu.edu we get an error.
```
-bash-4.2$ display image.ppm
display: unable to open X server `' @ error/display.c/DisplayImageCommand/431.
```

If we look at the man page for display, we get some more information about how display works, and that it needs an *X server*.
```
display(1)                         General Commands Manual                        display(1)

NAME
       display - displays an image or image sequence on any X server.
```

### XServer

The [X Windowing System](https://en.wikipedia.org/wiki/X_Window_System) is Unix's way to draw graphical elements to our screen. When we ssh into a terminal, the information we are passing back and forth between our client terminal(on our machine) and the server(a Khoury machine) is purely text. We want to open up our ssh connection, so we can also pass graphical information back and forth.

- In order to do this, when you ssh type: `ssh -Y username@login.khoury.neu.edu`
	- The -Y option enables trusted X11 forwarding.

### Mac Users

You may download [xquartz](https://www.xquartz.org) which has an X11 server running. When you perform `ssh -Y username@login.khoury.neu.edu`, the 'display canvas.ppm' will magically work!  (It's not really magic, because "There is no magic in Systems".)

### Windows Users

- You can download this shell: https://mobaxterm.mobatek.net/

### Linux/Ubuntu Users

- Unix users should be in luck, an X11 window is running. Just ssh with `ssh -Y username@login.khoury.neu.edu`

### Other

If none of those options work, then you may resort to these options below.

- Use this web interface and drag in your PPM: http://paulcuth.me.uk/netpbm-viewer/
- Download your repository locally after you commit any changes. Then view your .ppm in an image editor like GIMP(https://www.gimp.org).

## Part 0 - Makefiles

In this homework, there is what is called a ['makefile'](http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/). These are special files that have instructions on how to compile your programs. Make itself is a 'build system' that saves us time in typing out compilation commands (It also allows saves us from mistakes, making sure we type in the same compiler flags each time we run our program). Build systems may also have additional optimizations, such as only recompiling files that you have changed.

For this assignment:

* compile your program by typing in : `make` from the root directory. 

That is it! You can inspect the makefile to learn how to create your own makefiles for future assignments.

**Note:** Makefiles are particularly strict about using tabs instead of spaces to indent commands. If you make modifications and you get an error, the first thing to check is that your text editor is using tabs instead of spaces.

## Part 1 - Multiprocessing with Threads (Investigating the support code)

<img align="right" src="monet.jpg" width="200px" alt="picture">

The artists from our previous exercise are frustrated! They did not believe they got to be creative enough in how they were constrained on painting on the previous canvas. 

> Artists: "We are very frustrated!"
>
> You: You calmly explain, "We were exploiting data parallelism, you had one whole row of pixels to draw on without interruption."
>
> You: "Did you not enjoy working so efficiently(without the use of locks) without intefering with the other artists?"
>
> Artists: "We did not! This time we want to use threads, locks, ... , whatever it takes for our masterpiece!"

The artists have spoken...for this assignment you will be be working with [posix threads](https://computing.llnl.gov/tutorials/pthreads/) to help them create a masterpiece. This time, you will start working with exactly four artists: Michaelangelo, Donatello, Rapheal, and Leonardo. These artists egos have grown out of control and they want to move around and paint in more directions on the canvas. And as we know when there is a shared resource we will have to coordinate the artists much more carefully using locks!

### Prompt/Specification

Michaelangelo, Rapheal, Donatello, and Leonardo are amongst the great artists who have been commissioned to paint on a shared 256x256 canvas. The canvas itself starts with RGB values of (255,255,255) on each pixel.  Read on for more specifications:

- 4 total artists will paint on one canvas (until we add more artists in the next part).
- Each artist wants to paint as much as possible, and they each start in their own corner.
- Artists are not allowed to 'jump' to different locations. They may only paint next to pixels that are white, or pixels that are the same brush color the artist has. 
		- Michaelangelo paints in orange: rgb(255,140,0)
		- Leonardo paints in blue: rgb(0,0,255)
		- Donatello paints in Purple: rgb(128,0,128)
		- Raphael paints in red: rgb(255,0,0)
	- Because artists cannot jump around, there should be no "islands" where the artist has painted.
		- Again, to be clear, this means each color should be continuous.
- Artists only carry so much paint with them. They may attempt to 'paint' on the canvas 5000 times maximum. 
	- This also ensures the program eventually terminates.


Provided below are some snippets from the support code.

An artist struct stores its paint color and its current position where they are painting on the campus.
```c
// This is our data structure for each artist
typedef struct artist{
	int x,y;	// Store the position of where an artist is painting
	int r,g,b;	// The color an artist paints in.
}artist_t;
```

An artist moves around to adjacent and diagonal pixels. This is done by randomly selecting where the next move will be after trying to paint.
```c
const int movement[][2] ={
    {-1, 0}, // to the left 
    {-1, 1}, // to the left and down
    {0, 1}, // down
    {1, 1}, // to the right and down
    {1, 0}, // to the right
    {1, -1}, // to the right and up
    {0, -1}, // up
    {-1, -1} // to the left and up
};
```

Creating a new artist is done using malloc and populating an artists attributes. Each artist also has a separate thread id.
```c
	artist_t* Michelangelo = malloc(sizeof(artist_t));
	
	Michelangelo->x = 0;
	Michelangelo->y = 0;
	Michelangelo->r = 255;
	Michelangelo->g = 0;
	Michelangelo->b = 165;
	
	pthread_t Michelangelo_tid;
```

This is a reminder on how to launch a single thread and how to join the thread.
```c
	pthread_create(&Michelangelo_tid,NULL,(void*)paint,Michelangelo);
	pthread_join(Michelangelo_tid, NULL);		   

```

**Investigate and run the support code provided in painters.c before proceeding**

## Part 2 - 50 More artists.

Unfortunately the work the four artists are doing is not fast enough. Lets add 50 more rookie artists to speed up the work being done. All artists however do not want their work to be painted over by others(especially our four great artists), so you will have to enforce this with some locking mechanism as well.

### Specifications (The Actual assignment)

- Add 50 more 'rookieArtists' threads that paint.
	- You may initialize their paint colors and starting positions randomly in a for-loop.
	- Artists should not have the same color (Think about what this would mean when debugging!)
- Each artist has also agreed that they will not paint over each others work (the artists do respect each other--even the rookies). 
This means you need to enforce some locking mechanism.
	- I suggest using a 'trylock' 
	- An example is provided under trylock.c
- There should be no deadlock in this program.
	- Note: Care should be taken to minimize and only put locks around code that needs to be protected. 
		- Locals in threads for example, do not need to be locked if they are not shared. 
- All of the artists should congregate (i.e. join) at the end of the program to admire their work.
- You should output an image called "canvas.ppm" at the end of the program.
	- You may reference code from previous labs/assignments to understand the PPM format.

## Part 3 - Starvation

**Discuss(in a few sentences)** how starvation may occur in the above program, and how you would possibly fix it? (You may fix it in your implementation if you like--the art may become more beautiful!)

- ***In the original code provided in the homework, the artists can only move to an adjacent white pixel**. As a result, the artist can be surrounded by his own pixels and can not move any more. In this situation, starvation happens. Most of artists can only paint relatively small area. **I have added condition statement to allow the artist to move to his own pixels**. In this way, the artist has more chance to move, and the painted area becomes much larger.*
- *Another situation is that some artists paint much quickly than the other artists. This is because we can't control which thread gets more cpu time. **As a result, an artist's start pixel could be painted by another artist**. So this artist will never get a chance to paint a pixel. The thread starts starvation from the beginning. **I have set each artist's start pixel to the artist's own color**. This guarantees each artist could paint at least one pixel. So they could say they have cooperated with our four distinguished artists. Although I have to admit, since all the threads will run without order, it's very likely that there are still some artists will only have one pixel painted.*
- *I have added an array to record the possible moves from the current pixel, it will updated if a move failed, and it will be reset if the artist moved to a new pixel. In this way, the artist will stop if there is no possible moves.*
- *Another possible way of prevent starvation is let our program schedule the execution of all threads.*

## Part 4 - Catch Up

Often times in a semester long course, a few weeks in you can get in the habit of rushing through material and not truly understanding what is going on. For part 4, you will play a little catch up.

**Honor System** Spend 1-2 hours(no more or less) sitting down and learning something from this course you skipped over and did not really understand.

Some tutorial series that look reasonable (you can search around for others): 
* C Programming
	* https://www.lynda.com/C-tutorials/Up-Running-C/173754-2.html
	* https://www.youtube.com/playlist?list=PL6gx4Cwl9DGAKIXv8Yr6nhGJ9Vlcjyymq
	* https://www.youtube.com/watch?v=DWWQEmyVplU&list=PLZ1QII7yudbdFfKY1eKV3x_bag04AMPJq
	* memory/allocation: https://www.youtube.com/watch?v=xDVC3wKjS64&t=34s and https://www.youtube.com/watch?v=_8-ht2AKyH4
* Posix Threads
	* https://computing.llnl.gov/tutorials/pthreads/
	
**Write a few sentences here:** describing what you got caught up on. If you were caught up on 100% of things, write a few sentences about what new things you sat down and learned for 1-2 hours.

- *POSIX standard does not specify the size of the stack of a thread. So in order to enhance the portablilty of the program, we can explicitly specify the stack size of the thread by `pthread_attr_setstacksize(attr, stacksize)` function. The 'attr' argument is actually a pointer to a `pthread_attr_t` variable. 'pthread_attr_t' data structure is interesting because it is **opaque**. We can not modify it by directly access its fields. Instead, we can only modify it by a group of `pthread_attr_*()` functions. This phenomenon is kind like Interface in Java. It defines operations and hides the implementations.*
- *About how to count the number of colors: In GIMP, Colors->Info->Colorcube Analysis*

## Rubric

1. 90% for part 2 - 50 More Artists 
	1. 80% for a correct implementation Part 2
		- There should be no deadlock
		- There should be no data races
		- I will know your implementation is correct if I do not see overlap of colors with your 50+ artists.
			- Additionally creativity is encouraged when you get things working. :)
2. 10% for your response to Part 3 on starvation
3. 10% for your response in part 4	

# Resources to help

- Makefile tutorial: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html


### Glossary and Tips
1. Commit your code changes to the repository often, this is best practice.
2. Do not commit your .o file or your executable file to the repository, this is considered bad practice!
3. On Functionality/Style
	1. You should have comments in your code to explain what is going on, and use good style (80 characters per line maximum for example, functions that are less than 50 lines, well named variables, well named functions etc.).

1. Do not duplicate code (i.e. you should only have 1 function in which artists 'paint' in a loop)
2. It may be useful to have some struct that holds artists current position, how many attempts/brushes they have made, and what color they paint in.
3. Write as many helper functions as you like (getPixelColor(), checkIfPixelIsMyPaintColor(), etc.)
4. Make sure you join all of your threads
5. When passing arguments you should allocate memory
	1. See some examples here: https://courses.engr.illinois.edu/cs241/fa2010/ppt/10-pthread-examples.pdf

# Feedback Loop

(An optional task that will reinforce your learning throughout the semester)

- Check out [The Little Book of Semaphores](https://greenteapress.com/wp/semaphores/) by Allen Downey.
