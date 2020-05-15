# Project 2: Command Line Shell

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html <br />

Writer: Porfirio Mohabir <br />
University of San Francisco <br />
CS 326: Operting Systems

## Project Overview
Building a custom Shell. Shell Supports basic command execution


```bash
1.) Contains Built-In Function
	- CD whcih uses the chdir system Call
	- setenv to set environment variables using setenc system call
	- # (comments): strings prefixed with # will be ignored by the shell
	- history, which prints the last 100 commands entered with their command numbers
	- !! re-runs the last command that was entered
	- ! (history execution): entering !39 will re-run command number 39,or entering 
	  !"command" will run the most recent command specified
	- exit to exit the shell.
```
```bash
2.) Contains a History.c File
	- Struct of history entry that contains comand line and command ID number
	- Add to entry fucntion whcih adds a command to history data structure
	- History.c has a fucntion that makes a copy of its data structure and sorts the commands. 
	- There is a fucntion that supports the !! command
	- There is a fucntion that performs two searches for ! command
	- Printing History sotred by command ID
```
```bash
3.) Custom Shell supports Pipes and Redirection
	- Support Arbitrary nummber of pipes
	- Contains an execute pipleine function that seperately exectutes the commands
	- Checks executes commands with the > redirection to output file
```


```bash
4.) Other...
	- Tokenization
	- Custom shell also supports Scripting
	- Variabe Exspansion: denoted by the $ symbol
	- Signal Handling; "^C" does not terminate code, print a newline char,
	  print prompt only if no command is currently executing, fflush(stdout).
```

