
# Assignment 3 - Synchronization

In this assignment, you'll practice spawning threads and solving a race condition using a critical section.

This assignment is to be completed individually.

## Creating a Race Condition

You'll write a C++ program to demonstrate thread synchronization. Your main function should first create an empty file called *sync.txt*.  Then it will create two separate threads: *Thread-A* and *Thread-B*. Both threads will open *sync.txt* and write to it simultaneously.

*Thread-A* will write the numbers 0 through 9, a total of fifty-thousand times in nested `for` loops, then exit. In other words, print the numbers 0 through 9 over and over again, with each run on a separate line, for 50,000 lines. Here's an example of one such line:
```text
0 1 2 3 4 5 6 7 8 9 
```

Somewhat similarly, *Thread-B* will write the letters A through Z fifty-thousand times in nested `for` loops, then exit. In other words, print the letters A through Z over and over again, with each run on a separate line, for 50,000 lines. Here's an example of one such line:

```text
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
```

Do not write each line as one long string. You must write each character to the file, one by one (i.e., using multiple calls ). After writing a character to the file, write a space too, so the characters appear visually separated.

This means each thread would use a doubly-nested `for` loop. The outer loop would run 50,000 times (once per line), and the inner loop would run either 26 times (once per character) or 10 times (once per digit).

Hopefully, you understand this might create a race condition. Because *Thread-A* and *Thread-B* will both attempt to write individual characters to the same file at the same time, we might expect to see an undesirable mixing of the two types of lines, rather than each line remaining intact.

## Solving With a Critical Section

We'll solve the previous dilemma with a *critical section* in our code. You will use a mutex lock to control synchronization between the two threads, such that each line of numbers or letters will render uninterrupted, even with multiple competing writes to the file.

Try to decide where to place your *critical section* with the following criteria in mind:

1. We ***don't*** want to mix letters and numbers on a single line. Each line should only contain letters *or* numbers, but not both.

2. We ***do*** want line types to change sometimes. In other words, you might see a line of letters followed by a line of numbers. Or you may later see a line of numbers followed by a line of letters. You wouldn't want to see 100 lines of letters followed by 100 lines of numbers.

Try to make your *critical section* as small as possible while still solving the race condition for the above criteria.

### Overview of beginWriting()

You'll quickly see the following prototype in your starter code:

```cpp
void beginWriting(bool use_mutexes);
```

At minimum, this function should perform the following steps:

1. Take the incoming argument `use_mutexes` and assign its value to the global variable `G_USE_MUTEXES`.

2. Create both threads.

3. Join both threads.

### Overview of Thread Runner Functions

You will also see two prototypes for your thread runner functions:

```cpp
void* threadAEntry(void* param);
void* threadBEntry(void* param);
```

Each thread will start in a different function and produce a different type of live (per descriptions earlier in this document). However, each of these functions should have the ability to decide whether to use mutexes to create a *critical section*, or not. If the value of the global variable `G_USE_MUTEXES` is `true`, your thread runners should use mutexes. Otherwise, if the value of `G_USE_MUTEXES` is `false`, your thread runners should ***not*** use the mutexes. In doing so, you allow the caller to `beginWriting` to decide whether the output should simulate a race condition, or simulate a solved race condition.

One idea for how to do this: Avoid repeating the loop code, and simply wrap an `if` block around the calls to lock and unlock the mutex.

## Confirming Success

When the program is complete, the *sync.txt* file should generally resemble the following pattern:

1. Each individual line contains only letters *OR* numbers, but not both.

2. There should not be 100 lines of one type, followed by 100 lines of another type. Some lines should be of a different type than the last.

Here's an shortened example of what a successfully written file might look like (although yours may be slightly different):

```text
0 1 2 3 4 5 6 7 8 9
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
0 1 2 3 4 5 6 7 8 9
0 1 2 3 4 5 6 7 8 9
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
0 1 2 3 4 5 6 7 8 9
0 1 2 3 4 5 6 7 8 9
```

## Confirming Failure

As stated above, no line should have a mix of letters *and* numbers. Here is an example that shows your *critical section* isn't working, or perhaps needs to cover a larger area:

```text
A 0 1 B C 2 3 4 D E 5 F 6 7 G 8 H 9 I J K L M N O P Q R S T U V W X Y Z
```

Also, you should not see a block of 50,000 lines of any particular type in a row. We expect to see the lines switch up a bit. Here's the general idea (some lines omitted and parenthetical added for explanation), which may indicate your *critical section* covers *too much* code.

```text
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 
(letter lines repeat a total of 50,000 times)
0 1 2 3 4 5 6 7 8 9 
0 1 2 3 4 5 6 7 8 9 
0 1 2 3 4 5 6 7 8 9 
0 1 2 3 4 5 6 7 8 9 
0 1 2 3 4 5 6 7 8 9 
(number lines repeat a total of 50,000 times)
```

If you see something similar to either of the above examples, you know you need to work on your *critical section* a bit more!

## Files to Edit

This section describes which files you should edit for your submission. The easiest thing to do is check the comments inside each source file, but here is a summary.

The *main.cpp* files doesn't need to be edited. It is just a very simple interface to help you play around with your program. You can edit it if you want, or not; It won't be graded. It is only here to help you debug.

Most of your work will go into *functions.cpp* and *functions.hpp*. Do not remove existing functions from those files. You may, however, *add* new helper functions to the *functions.cpp* file, as long as you remember to add the corresponding prototype in *functions.hpp*.

The following files ***should not be edited***:

* INSTRUCTIONS.md
* CPP_Tests.cpp
* Makefile

## Compilation, Execution, Submission

As with the previous assignment, you'll push your code to Github whenever you make significant progress. You'll also compile/execute/test your program using [GNU Make](https://www.gnu.org/software/make/) (try `make help` for commands). Finally, you'll submit your code for grading to Gradescope.

## Hints and Tips

This section contains hints and tips to help you along.

### Avoiding File Clobbering

You may notice one of your threads writes a bit to the output file, which is promptly erased once the other thread opens the file for writing. One solution to this issue is to first clear the file before spawning any threads. This is done easily with an [std::ofstream](https://www.cplusplus.com/reference/fstream/ofstream/) object. Simply [open the file](https://www.cplusplus.com/reference/fstream/ofstream/ofstream/) with the truncate flag set, like so:

```cpp
std::ofstream myFile(your_filename_here, std::ios_base::trunc);
```

Once the file is closed, you should see the file exists, but is empty. You can then have each thread open the file using *append* mode, like so:

```cpp
std::ofstream myFile(your_filename_here, std::ios_base::app);
```

This allows both threads to open the file without clobbering the other's progress up to that point. Similar tactics can be used with C file handling functions.

### Turbo Charging the Race Condition

You may notice that *some runs* of your program may not appear to suffer from a race condition, even with mutexes turned off. This doesn't mean there isn't a race condition. It only means race conditions do not always or frequently rear their ugly heads. If you're having trouble passing tests because of this, it may help to introduce more lag while writing to your file.

One suggestion is to use an [std::ofstream](https://www.cplusplus.com/reference/fstream/ofstream/) object to do your file writing, and call its [flush](https://www.cplusplus.com/reference/ostream/ostream/flush/) method after every single character you write to your file.

## Copyright and Acknowledgment

Copyright 2022, Mike Peralta.

This assignment prompt was inspired by a prompt found {? somewhere ?}.

This prompt and starter code are licensed via: [Creative Commons; Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)](https://creativecommons.org/licenses/by-sa/4.0/).

![CC By-SA 4.0](images/CC_BY-SA_4.0.png "CC By-SA 4.0")






