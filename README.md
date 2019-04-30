# Project 1
Name: John Henkel

Files:
	MyShell:
		Executable version of the shell. To run type: ./MyShell	

	README.md:
		Contains description of the various project components

	command.h:
		Header file containing the command_t struct and the partion_tokens and execute functions as well as InputType and OutputType enums. 
		This file allows us to easily handle each of the inputted commands and any pipes or file redirection.

	history.txt:
		File containg all of the commands that have been executed by the shell. Allows us to preserve history between sections.

	main.cpp:
		Creates the shell and executes the shell loop.

	makefile:
		Contains info about how the shell is to be compiled. Used with the command 'make';

	shell.h:
		Contains function declarations for the various .cpp files as well as the maps needed to contain builtins, localvars, and aliases.

	shell_builtins.cpp:
		Contains function implementations for 'builtin' commands. Executes commands internally.

	shell_cmd_execution.cpp:
		Contains function implementation for 'external' commands. Executes commands internally and handles file redirection and piping as well as command parsing.

	shell_core.cpp:
		Contains the main shell loop and handles the execution of all commands, aliasing, variable assignment.

	shell_tab_completion.cpp:
		Contains the functions for implementing tab completion in the shell.

Unusual / interesting features:
	Added history preservation between sessions to more closely resemble bash behavior.
Approximate hours:
	25 hours
	
