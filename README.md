# BashEmulator

A basic emulator of the bash shell. The emulator supports several 'built-in' commands:
* ls
* cd
* pwd
* alias
* unalias
* history
* exit

If the user enters a command that is not present in the list of built-ins, the shell will call execvp to attempt to execute the command. The simulator also supports file redirection as well as command piping.
