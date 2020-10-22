# OVC
**Omelette version control**

Omelette is a CMD-based, user-friendly, and open source version control system written in pure C, used for maintaining and managing projects

---

## How it works
Omelette works similar to git, with a few changes to make things easier for the user. It uses its own diff algorithm to find
the difference between different versions of a file to store the data incrementally and in less space. Omelette brings a decent
variety of commands for the user to use. A very detailed explanation of every command can be accessed inside the program

---

## How to use
In order to get Omelette up and running, all you have to do is download the code folder and save it somewhere on your computer.
then compile the code and run the generated main.exe file.

---

## Compile the program
To compile the program, there are two approaches, either run the following line of code inside the code directory in CMD:
```
gcc main.c stash.c status.c select.c unselect.c reset.c log.c init.c help.c commit.c -o main
```
or simply use the Makefile that is located inside the code folder

---

## Last but not least
In order to have a good understanding of how Omelette works, please read the note.txt file inside the code folder and ...
*please contact me if you find any bugs :)*
