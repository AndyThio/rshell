# rshell

## Program Overview
This program was written to create a simple shell that could handle simple `execvp` commands and work similar to `bash`. Features include `execvp` commands and Connectors such as `&&`, `||`, and `;`.

## How to Use
1. The program will prompt you with `username@machine$` and wait for your input.
    - `username` and `machine` will differ depending on account and machine you are using. E.g. if I was on my computer it would show up as `andy@SlowKing$`.
2. Input an execvp command (e.g. `ls`, `pwd`, or `false`)
3. Include any wanted flags (e.g. `-a`, `-l`, or `-r`)
4. Press enter and rshell will run your command and prompt you for another command.
    - rshell will not execute bad commands and instead print out an error message.
5. When finished, type `exit` into the prompt to close it.

## Prerequisites
* g++ version 4.8.2

## Installation Guide
1. Clone the git repository with
    `git clone https://github.com/AndyThio/rshell.git`
2. Navigate to the folder with
    `cd rshell`
3. Use the `hw0` branch by typing
    `git checkout hw0`
4. Check you have the Prerequisites installed
5. Compile using
    `make`
6. Run the program with
    `bin/rshell`

## Bug/Known Issues/Limitations
* Currently a single command + flag is limited to 2048 chars (including `'\0'`)
* When connected are entered alone, such as:
    `andy@SlowKing$ &&`
    `andy@SlowKing$ &&||;;;||||&&`
 they will execute as if they were all `;`, but only when they are alone with no commands or arguements. This is done intentionally.
    * This is different from bash which returns a syntax error.
* A single `&` or `|` will be treated the same as `&&` or `||` respectively.
* A known bug is that when scripting, rshell will not be able to get your username. It will instead throw an error and display the default `unknown`.
* The number of hardlinks suported by `ls -l` can have no more than 5 digits.
