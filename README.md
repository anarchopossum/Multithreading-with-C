[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-c66648af7eb3fe8bc4f294546bfd86ef473780cde1dea487d3c4ff354943c9ae.svg)](https://classroom.github.com/online_ide?assignment_repo_id=7985506&assignment_repo_type=AssignmentRepo)
# CECS 326 Lab 2: Concurrent Processing, Shared Memory, and Semaphores

## Assignment Description

The goal of this assignment is to become familiar with concurrent processing in Unix/Linux using [shared memory](https://man7.org/linux/man-pages/man7/shm_overview.7.html) and become familiar with both man pages and signal processing.

You will create four seperate processes that work with the same segment of shared memory in order to communicate information. This will be done as an RPG-based game with three characters: a barbarian, a wizard, and a rogue. You will also create a program that is in charge of starting the game and running all of the necessary processes. This is required to be done on a POSIX system. If you do not own one, plenty of [virtual machines](https://www.virtualbox.org/) exist for free, and are not terribly difficult to set up.

Finally, the last stage of this assignment is to become familiar with semaphores. As the last stage of the dungeon, semaphores will be used to "hold" open a door (remember the down(&mutex) and up(&mutex) from lecture) and allow your party to get the treasure inside. The "treasure" earns more than just gold for your characters, though. It earns you points!

## The Three Classes Overview

### The Barbarian

The Barbarian is the first character class that you should probably make. The Barbarian works as follows: 

When the Barbarian receives a signal (defined in **dungeon_settings.h**), the Barbarian copies the integer in the enemy's **health** field into the **attack** field. Use **dungeon_info.h** to see how the **Dungeon** struct is set up. The Dungeon will then wait an amount of time defined in dungeon_settings.h as **SECONDS_TO_ATTACK**. If the integer in **attack** matches the integer in **health**, then this will count as success.

### The Wizard

The Wizard is probably the second class that you should make. The Wizard works as follows:

When the Wizard receives a signal (defined in **dungeon_settings.h**), the Wizard reads the *Caesar Cypher* placed in the Barrier's **spell** field. The Wizard then decodes the [*Caesar Cypher*](#Caesar-Cypher "Goto Caesar Cypher"), using the first character as the key, and copies the decoded message into the Wizard's **spell** field. The Dungeon will wait an amount of time defined in dungeon_settings.h as **SECONDS_TO_GUESS_BARRIER** for the decoding process to complete. If the Wizard's **spell** field matches the decoded message after the Dungeon has finished waiting, then this will count as success.

### The Rogue

The Rogue is probably the last class that you should make. The Rogue works as follows:

When the Rogue receives a signal (defined in **dungeon_settings.h**), the Rogue will attempt to guess a float value to "pick" a lock. The Trap struct has a char for direction, and a boolean for locked. This puzzle is a little unique. The Dungeon will wait for a total amount of time defined in **dungeon_settings.h** as **SECONDS_TO_PICK**, but will check the value of the Rogue's current pick position using **TIME_BETWEEN_ROGUE_TICKS**. Notice that these two values are quite different, and that is because one of them uses [sleep](https://man7.org/linux/man-pages/man3/sleep.3.html), and the other [usleep](https://man7.org/linux/man-pages/man3/usleep.3.html). I recommend that you follow a similar example.

Every X microseconds, the Dungeon will check the field **pick** in the Rogue struct in shared memory, and will change the **direction** and **locked** fields in Trap accordingly. If the Rogue's pick needs to go up, the trap will set **direction** to 'u'. If the Rogue's pick needs to go down, the trap will set the **direction** to 'd'. If the Rogue's pick is in the right position, the dungeon will set **direction** to '-', and **locked** to false, indicating that the Rogue succeeded in picking the lock. If this occurs, it is counted as success. While this can be done through a brute force search, it can be very elegantly accomplished with a [binary search](#Binary-Search "Goto Binary Search").

## The Game and Dungeon

Please make a `game.c` file and have that be your game's launcher. This should be the code responsible for calling fork and exec. Once you have launched all of your characters, call `RunDungeon` using the pid's of the character classes that you launched. If something isn't set up right, you will likely see an appropriate error.

The Dungeon will be given to you as a .o object file. This can be easily compiled into your code by including it in your compile arguments. Example:

`gcc my_code.c dungeon.o -o my_executable -lrt -pthread`

(Remember that compile command order matters, and that order should be based on dependency. Therefore dungeon.o should probably come after all of your own source files.)

All of your source code files should include "dungeon_info.h" as one of the includes. This contains various information required to make the dungeon work.

## Shared Memory Overview

This project will require you to be familiar with [shared memory](https://man7.org/linux/man-pages/man7/shm_overview.7.html) on Posix systems. It will also require you to handle [Signals](https://man7.org/linux/man-pages/man7/signal.7.html) properly. Since all of this will be done in the C-language, I highly recommend that you brush up on your C practices.

For the purposes of this assignment, only store the **Dungeon** struct in shared memory. Not any of the other structs. If you set it up properly, adjusting one value from one process in shared memory will adjust it for all processes in shared memory.

## Semaphore Overview

After your characters have successfully arrived at the end of the dungeon,  they will have one final challenge. The treasure room door must be held open by two party members in order to let the Rogue in to get the treasure! It doesn't matter which party member holds which lever, only that both levers are held down by two separate party members long enough for the Rogue to get all of the treasure. The last four points will be for releasing the [semaphores](https://man7.org/linux/man-pages/man7/sem_overview.7.html) after the Rogue leaves the treasure room. Appropriate methods will be declared, and the names of the [semaphores](https://man7.org/linux/man-pages/man7/sem_overview.7.html) will be given in the **dungeon_info.h** file. We will be using the Named Semaphores defined in [sem_overview](https://man7.org/linux/man-pages/man7/sem_overview.7.html). Be sure to read the definitions for [sem_post](https://man7.org/linux/man-pages/man3/sem_post.3.html) and [sem_wait](https://man7.org/linux/man-pages/man3/sem_wait.3.html), remembering that in this case, "holding" the door for the Rogue here would be preventing the dungeon from accessing the room while the Rogue gets the treasure. This is the final part of this assignment that is graded, so make sure everything else works first before tackling this!

The dungeon will send a new signal, defined in **dungeon_settings.h** as `SEMAPHORE_SIGNAL`. Make sure that your classes can handle it without crashing! After your Rogue gets all four letters from the "treasure" field, copy them into the "spoils" field of the Dungeon, and release the semaphores. Note: The treasure field will only give one character at a time, and it will pause between adding additional characters. It will also not be null terminated, so make sure you plan around tackling that problem!

Once the Rogue has gotten all of the treasure, it is up to you how you want to handle re-opening the semaphores. Do you want the Wizard and Barbarian to wait until the Rogue has four characters in the `spoils` field, and then immediately release the door? Do you want the Rogue to send a signal to the Wizard and Barbarian to release the door? This part is up to you to figure out. By now, if you've gotten this far, you should have at least one or two ideas of how to tackle this.

Note: Your semaphores should be created before you call `RunDungeon`. Also, the `treasure` and `spoils` fields will be initialized to null terminators before their values are used. This can be useful to note.

# Assignment Details

## Required Reading

Please read at a *MINIMUM* the following pages. You don't need to be meticulous about your reading, but at a minimum read the information that seems important, and be familiar with the pages. Then, answer the following questions. These questions are not graded, but knowing the answers to them will help you immensely when you actually start to code this assignment:
1. [shared memory](https://man7.org/linux/man-pages/man7/shm_overview.7.html) (and its related pages, at least the first three in the description.

2. [fork()](https://man7.org/linux/man-pages/man2/fork.2.html)

3. [exec](https://man7.org/linux/man-pages/man3/exec.3.html) (This has many different functions that do effectively the same thing, but through different means. Pick your favorite.)

4. [sigaction](https://man7.org/linux/man-pages/man2/sigaction.2.html) (You may choose to use [signal](https://man7.org/linux/man-pages/man7/signal.7.html) instead, but I highly recommend the prior one.)

5. [Makefile](https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/) (You are only required to make a Makefile that uses what you learn up to the first Makefile iteration, but it is worth a read to go a bit further. You may either use multiple gcc compile commands in your first make rule, or you may create multiple make rules that run by calling your first make rule. [More on that here](https://makefiletutorial.com/#targets).)

6. [Semaphores](https://man7.org/linux/man-pages/man7/sem_overview.7.html) (Be very familiar with what posting versus waiting does.)

### questions you should answer: 
* Q1. In what order should you perform the actions to create [shared memory](https://man7.org/linux/man-pages/man7/shm_overview.7.html)? (HINT: A minimum of three functions must be used the first time you create shared memory.)

* Q2. What is the return type of mmap, and what can you do with it? (If you are unfamiliar with C/C++, you might need to [do some personal research](https://en.cppreference.com/w/c) in order to understand this data type.)

* Q3. What does [fork()](https://man7.org/linux/man-pages/man2/fork.2.html) return, and how can that information be used?

* Q4. If [exec](https://man7.org/linux/man-pages/man3/exec.3.html) works as intended, what happens to the process that calls it?

* Q5. Do all three functions for shared memory need to be called in every single process after the first? If yes, why? If no, which ones are needed, and why would you not need to call all of them?

* Q6. What does a [struct](https://en.cppreference.com/w/c/language/struct) look like in memory, and if I store a struct in shared memory, how do I access its various fields?

* Q7. How do I determine the size of a struct in bytes?

## Caesar Cypher
If you want to know a brief history of the Caesar Cypher, feel free to read the [Wikipedia page](https://en.wikipedia.org/wiki/Caesar_cipher) for a summary. The wikipedia page also offers some formulas and examples that might help reinforce your understanding.

In C, characters are represented as chars, which are typically one byte of memory. They also have a numerical value, such as 65 for 'A', 90 for 'Z', 97 for 'a', and 122 for 'z'. A full list of values can be seen here:
![ASCII Table](https://www.asciitable.com/asciifull.gif "An ASCII Table").

We can utilize this in order to both encrypt and decrypt information using a Caesar Cypher. While Caesar Cyphers are not cryptographically secure, they are a nice introduction to the idea of data obfuscation. For the purposes of this assignment, the **Barrier** struct contains a field called **spell**. This is a char array of a size determined in the **dungeon_settings.h** file. Every alphabetical character that is put into the **Barrier**'s **spell** field will use the first character in the array as the shift value. So for example, if the first character were 'T', it would represent a shift of 84.

Notice that in that example the value of 84 > 26, and thus the shift would be greater than the number of characters in the alphabet. You will have to "roll" the numbers using modulo in order to keep them within the same alphabet. Capitalization will remain consistent. If a character in the **spell** field is capitalized, it will also be capitalized in the final answer. If it is lower case, it will be lower case in the final answer. Punctuation and spaces do not need to be modified at all.

## Binary Search

For a more detailed read on Binary Search, feel free to peruse the [Wikipedia article](https://en.wikipedia.org/wiki/Binary_search_algorithm).

A Binary Search in Computer Science is an algorithm that splits a list in half, and then checks if the desired element is above or below the current position. It then splits that list in half, and repeats the previous steps until the element is found. While this is usually used to traverse an array to find a list element, this formula can also be utilized to find a floating point value. This is how we will utilize it.

Our Dungeon will pick a random value between 0 and the value **MAX_PICK_ANGLE** defined in **dungeon_settings.h**. It is then up to our Rogue to guess that value. To do this, start by picking a value halfway between 0 and **MAX_PICK_ANGLE**, and put that in the **Rogue**'s **pick** field. The dungeon will put a value in **Trap**'s **direction** field to indicate whether the position is above or below the current **pick** value. (HINT: I recommend setting the value in **direction** to something like 't' after modifying your **pick** value so that you can tell when the value has changed. Otherwise it can be difficult to tell if you need to adjust your position or not.)

When the pick is within the threshold defined by **LOCK_THRESHOLD** in **dungeon_settings.h**, the dungeon will place a '-' character in **direction**. Use this information to tell the Rogue to stop searching for new values.

## Timeline

While adherence to this timeline is not graded, you will be on-track if you meet or beat these deadlines. If you have not finished one of these deadlines by the time given, please come visit me in my office hours, or at least send me an e-mail if you're having trouble understanding the assignment. Remember: I'm here to help. Be curious, and don't wait until the last second to do this assignment. Trust me on this.

Week 1: Create your makefile, and have your `game`, `barbarian`, `wizard`, and `rogue` processes able to be compiled. They don't need to be finished yet, but they need to exist in a runnable state. The Dungeon will not run properly unless three separate processes have been started, and are running by the time the Dungeon is started.

Week 2: All of your processes should be able to access [shared memory](https://man7.org/linux/man-pages/man7/shm_overview.7.html) and interpret the data that matters to them. Ensure that you're using [fork](https://man7.org/linux/man-pages/man2/fork.2.html) and [exec](https://man7.org/linux/man-pages/man3/exec.3.html) properly. Even if all of your processes don't fully work yet, they should all be runnable, and they should exist until they are terminated.

Week 3: Your `barbarian` process should be 100% functional, and your `wizard` and `rogue` processes should work at least to some degree. Every process should be able to receive a signal without crashing, and every process should be able to do something with shared memory when they receive a signal. Please also ensure that you're cleaning up after yourself by this point. Clean up your shared memory, terminate your processes properly, etc. A field, `running` exists in the dungeon to help with this. If dungeon->running == false, all processes should be terminated. (I recommend including in all of your while loops a condition to exit if this `running` field becomes false at any point.)

Week 4: Every process should be successful in running. If you're not getting a near 100% success rate, please see me in my office hours to try and figure out what might be going wrong. A failure once in a blue moon is nothing to worry about. Finally, your semaphores should be set up by now.

## Grading scale:

### If the code is not commented, there is no makefile, there are no commits to GitHub, or there are no source files, this is an automatic zero. You must have comments, you must have a Makefile, you must commit the assignment to GitHub, and you must have at minimum four source files. Also, DO NOT compress your files into a .zip or other similar archived format. I will not grade them if they are turned in as such.

| Points | Requirement                                                                                                                                                     |
|--------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 10     | Your code compiles and runs successfully, and you have followed the rules.                                                                                      |
| 10     | You successfully created and managed shared memory                                                                                                              |
| 5      | All of your processes run concurrently, and they can all access shared memory.                                                                                  |
| 5      | Your processes do not crash upon receiving signals, or through regular use.                                                                                     |
| 10     | 1 point for every successful run of the dungeon. I will run each character twice, followed by four random runs for up to 10 points.                             |
| 20     | 4 points for holding down the semaphores correctly for up to four ticks, for 16 points. Then, you must release your semaphores to receive the last four points. |

Partial credit may be given based on degree of success for any of the above, and additional points may be deducted in rare cases of completely disregarding the point of the directions. (Bear in mind, it's okay to experiment and have odd solutions, but if you do something along the lines of just guessing random phrases for the Wizard, for example, or by using length to calculate which phrase it is, this is grounds for points being lost. As long as your solution keeps within the spirit of the assignment, you shouldn't have to worry about this.)

## A quick C refresher:

Pointers - A pointer is only an address, on its own it does not contain any information. It must be given memory in some way. This is most often seen with [malloc](https://man7.org/linux/man-pages/man3/free.3.html), or its variations. Until initialized, pointers tend to seg fault when used. The information at the end of pointers is accessed with one of the following:
 - (*myPointer)
 - myPointer[index]
 - myPointer->someValue (this is mostly seen with structs)

Arrays - Arrays in C/C++ exist in two forms. Either pre-allocated, or dynamic. You may reassign individual values within pre-allocated arrays, but if you try to assign directly to a pre-allocated array, your program will in the best case either not compile or crash, and worst case will perform undefined behavior. Dynamic memory can be reassigned to, but you risk memory leaks if you do not [free](https://man7.org/linux/man-pages/man3/free.3p.html) your memory.

Pre-allocated arrays:
 - int myArr[10];

Dynamically allocated arrays:
 - int *myArr = malloc(sizeof(int) * 10);

[printf](https://man7.org/linux/man-pages/man3/printf.3.html) - This prints to the terminal by default. It uses string substitutions with %'s to format your string. It will look something like this:

`printf("my string: %s, my int: %d, my address: %p, my char: %c", someString, someInt, somePointer, someChar);`

[C-style strings](https://man7.org/linux/man-pages/man3/string.3.html) - C is a more archaic language, and lacks some features that you might be used to, including strings. In C, a string is a char*, or char[] that ends with a literal '\0' character (null-terminator). When printing, if you manually created a char[], and funky stuff happens or you segfault after trying to print using that string, make sure that the very last element is a null-terminator ('\0') character, otherwise your program won't know where the string ends, and might even traverse your entire computer's memory looking for an end.

## Recommendations:
 - If your `Rogue` is for some reason not modifying shared memory properly, double-check that you've terminated the process, and that it hasn't crashed. Both can lead to perplexing errors.
 - If the dungeon is printing '_' characters for your wizard's spell, that means that you used an invalid character. Check your math on your caesar cypher, and make sure that you're wrapping properly and ignoring punctuation correctly.
 - For the Rogue, try setting **direction** to 't' or a similar unused character every time you set the value in **pick**, and then do not do anything while the character is still 't'.
 - If you find your program handling one signal fine, and then crashing, try setting up more information in your [sigaction](https://man7.org/linux/man-pages/man2/sigaction.2.html) before registering your signal handling. You might need to set the restart flag.
 - Do not wait to start working. Sleep clears your mental state and allows you to look at your code with a fresh mind. You will likely need to refactor this assignment two or three times at least. This takes time, and is best not left until the day before the assignment is due.
 - Remember, commit early, commit often. The deadline can sneak up on you. It's better to have almost everything turned in when the deadline passes than nothing turned in. Just do a commit every time you finish for the day and push it to GitHub and you won't have to worry about this.
 - Be good friends with sleep and usleep. These functions force your process to relinquish some time, and this might sometimes be necessary to play nicely with other processes, including the dungeon! Remember: If you're using multiple processes, and you want to let another one run, just do a quick usleep.

## Some miscellanous useful information:
### Helpful Linux/Unix terminal commands:
- [touch](https://man7.org/linux/man-pages/man1/touch.1.html) - to create your files
- [htop](https://man7.org/linux/man-pages/man1/htop.1.html) - for if you want to see if any errant processes are still running
- [kill](https://man7.org/linux/man-pages/man1/kill.1.html) - for if you find an errant process running
### Useful information if you get stuck:
- In order to compile on Unix/Linux machines, you may need to specify some compiler flags. Specifically -lrt needs to be near/at the end of your compile commands for working with shared memory.
- It's worth checking that you have included any headers that you need at the top of your source files. If you find yourself being told that you are using functions implicitly without defining them, this is probably the culprit.
- Order of function definitions matters in C. You can get around this, however, by *declaring* functions before using them. [More on that here](https://en.cppreference.com/w/c/language/functions). This is where a header file might come in handy.
- If the implementation part of this seems a bit general, and open to interpretation, that's because it is. As computer scientists and engineers, I expect you to have some level of problem solving skills and the ability to research problems to find solutions. While I have certainly given you plenty of links to get you started, this is far from all of the information you will need to know in order to get a 100% in this lab. Be curious, ask questions, hypothesize and test. That's the *science* part of Computer Science.

## Deliverables

I will require the following items for grading:

* Your `*.c` source code files
* Your *makefile*
* At least one screenshot of your executed code, in `*.png` or `*.jpg` format

Submit your files through your git repository. Your submission must follow the following rules, else *I will not grade it and you will receive a zero for the submission*:

* Do not use compression on your files
* Make sure that all significant code is *commented* with your own explanations
