# rshell and `ls`

## Program Overview
This program was written to create a simple shell that could handle simple `execvp` commands and work similar to `bash`. Features include `execvp` commands and Connectors such as `&&`, `||`, and `;`.
`ls` is seperately created and supports the `-l`, `-a`, and `-R` flags.

## How to Use rshell
1. The program will prompt you with `username@machine$` and wait for your input.
    - `username` and `machine` will differ depending on account and machine you are using. E.g. if I was on my computer it would show up as `andy@SlowKing$`.
2. Input an execvp command (e.g. `ls`, `pwd`, or `false`)
3. Include any wanted flags (e.g. `-a`, `-l`, or `-r`)
4. Press enter and rshell will run your command and prompt you for another command.
    - rshell will not execute bad commands and instead print out an error message.
5. When finished, type `exit` into the prompt to close it.

## How to Use `ls`
1. After installing simply type `bin/ls`
2. Enter files, file paths, or flags that you want
3. When finished press `enter`

## Prerequisites
* g++ version 4.8.2

## Installation Guide
1. Clone the git repository with
    `git clone https://github.com/AndyThio/rshell.git`
2. Navigate to the folder with
    `cd rshell`
3. Use the `hw1` branch by typing
    `git checkout hw1`
4. Check you have the Prerequisites installed
5. Compile using
    `make`
6. Run the program with
    `bin/rshell`
    or run `ls` with
    `bin/ls`

## Bug/Known Issues/Limitations for rshell
* Currently a single command + flag is limited to 2048 chars (including `'\0'`)
* When connected are entered alone, such as:
    `andy@SlowKing$ &&`
    `andy@SlowKing$ &&||;;;||||&&`
 they will execute as if they were all `;`, but only when they are alone with no commands or arguements. This is done intentionally.
    * This is different from bash which returns a syntax error.
* A single `&` or `|` will be treated the same as `&&` or `||` respectively.
* A known bug is that when scripting, rshell will not be able to get your username. It will instead throw an error and display the default `unknown`.

## Bugs/Known Issues/Limitations for `ls`
* Once a bad file is entered, ls will quit.
* Files will always be printed out in two columns.
    *Unless -l is used, then files are printed out in lines with static column sizes.
* `-l` has limitations on sizes that it can print out.
    * hardlinks cannot exceed 5 digits
    * Usernames cannot exceed 12 characters
    * Groupnames cannot exceed 8 characters
    * File size cannot exceed 12 digits
* `bin/ls` can not take more than 512 arguements.
* When compiling the Makefile, it will always recompile both `rshell` and `ls.cpp` to make `ls`.
* A Known bug is `make` will not work when scripting.
