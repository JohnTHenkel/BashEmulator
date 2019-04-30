/**
 * This file contains implementations of the functions that are responsible for
 * executing commands that are not builtins.
 *
 * Much of the code you write will probably be in this file. Add helper methods
 * as needed to keep your code clean. You'll lose points if you write a single
 * monolithic function!
 */

#include "shell.h"
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include "command.h"
#include <fcntl.h>
using namespace std;

/*
 * Partions and executes the commands passed in by tokens.
*/
int Shell::execute_external_command(vector<string>& tokens) {
  	

	vector<command_t> commands;
	if(!partition_tokens(tokens,commands)){
		return 1;
	}
	return	execute(commands,0);


}


/*
 *Handles external command execution including piping (implemented recursively)
 *Detects where input should come from and output should go to and handles accordingly.
 */
int execute(std::vector<command_t>& commands,int read_pipe_fd){
	int pid;
	int status;
	command_t cmd= commands.at(0);
	commands.erase(commands.begin());
	char * argv[cmd.argv.size()+1];

	int i=0;
	for (string str:cmd.argv){
		argv[i]=(char *)cmd.argv.at(i).c_str();
		i++;
	}
	argv[cmd.argv.size()]=NULL;
	int the_pipe[2];
	if(pipe(the_pipe)<0){
		perror("opening pipe");
		return 1;	
	}

	if ((pid=fork())==-1){
		perror("fork failed");
		return 1;
	}
	if(pid==0){//child
		switch(cmd.input_type){
			case READ_FROM_STDIN:
				break;
			case READ_FROM_FILE:
				{
					int fd=open(cmd.infile.c_str(),O_RDONLY,0644);
					if(fd==-1){
						perror("failed to open file");
						return -1;

					}
					dup2(fd,STDIN_FILENO);
					break;
				}
			case READ_FROM_PIPE:
				{
					
				if(cmd.output_type!=WRITE_TO_PIPE){
					close(the_pipe[1]);	
				}
				dup2(read_pipe_fd,STDIN_FILENO);
				break;
				}
		}
		switch(cmd.output_type){
			case WRITE_TO_STDOUT:
				break;
			case WRITE_TO_PIPE:
				{
					close(the_pipe[0]);
					dup2(the_pipe[1],STDOUT_FILENO);
					

				break;
				}
			case WRITE_TO_FILE:
				{
					int fd =open(cmd.outfile.c_str(),O_WRONLY | O_CREAT | O_TRUNC, 0644);
					if(fd==-1){
						perror("failed to open file");
						return -1;
					}
					dup2(fd,STDOUT_FILENO);
					break;
				}
			case APPEND_TO_FILE:
				{
										
					
					int fd=open(cmd.outfile.c_str(),O_WRONLY | O_CREAT | O_APPEND,0644);
					if(fd==-1){
						perror("failed to open file");
						return -1;
					}
					dup2(fd,STDOUT_FILENO);
					break;
				}
				
		}

		execvp(argv[0],&argv[0]);
		perror("exec failed");
		return 2;
	}
	else{//parent
	

		waitpid(pid,&status,0);
		if (cmd.input_type==READ_FROM_PIPE){
			close(read_pipe_fd);
		}
		close(the_pipe[1]);
		
		

		
		if(WIFEXITED(status)){
			int exit= WEXITSTATUS(status);
			
			if(exit!=0&&commands.empty())return exit;
		}
		
	
	}
		if (!commands.empty()){
				return  execute(commands,the_pipe[0]);
			
		}



return 0;
}

/*
 *Partions the input (tokens) into separate, easily readible commands in the form of command_t variables. 
 *Passes the results back as a vector of command_t, commands.
*/
bool partition_tokens(vector<string> tokens, vector<command_t>& commands) {
	
	command_t command;
	int i=-1;
	bool skip=false;
	bool piped=false;

	for (auto & str:tokens){
		i++;
				
		if(piped){
			command.input_type=READ_FROM_PIPE;
			piped=false;
		}
		if(skip){
			skip=false;
			continue;
		}
		if (str==">"||str==">>" ){
			if (command.output_type!=WRITE_TO_STDOUT){
				cerr << "multiple outputs";
				return false;
			}
			if(str==">"){
				command.output_type=WRITE_TO_FILE;
			}
			else{
				command.output_type=APPEND_TO_FILE;
			}
			size_t t =i+1;
			if(t<tokens.size()){
				if(tokens.at(i+1)!="|"){
					if(command.argv.empty()){

						cerr << "no command given";
						return false;
					}
					command.outfile= tokens.at(i+1);
				}
				else {
					return false;
				}
			}
			else{
				cerr << "invalid input";
				return false;
			}
			skip=true;
		}
		else if (str=="<"){
			if (command.input_type!=READ_FROM_STDIN){
				cerr << "multiple inputs";
				return false;
					
			}	
			
			command.input_type=READ_FROM_FILE;
			size_t t = i+1;
			if (t<tokens.size()){
				if(tokens.at(i+1)!="|"){
					if(command.argv.empty()){

						cerr << "no command given";
						return false;
					}
					command.infile= tokens.at(i+1);
				}
				else {
					cerr << "no file given for input";
					return false;
				}
			}
			else{
				cerr << "invalid input";
				return false;
			}
			skip=true;
		}
		

		else if( str!="|"){

			command.argv.push_back(str);
		}
		else{
			if (tokens.at(0)=="|"){
				cerr << "no input to pipe";
				return false;
			}
			size_t t = i+1;
			if(t<tokens.size()){
				if(tokens.at(t)=="|"){
					cerr << "command is empty";
					return false;
				}
			}
			if (command.output_type!=WRITE_TO_STDOUT){
				cerr << "multiple outputs";
				return false;	
			}
			command.output_type=WRITE_TO_PIPE;	
			piped=true;		
			commands.push_back(command);
			command.argv.clear();
			command.input_type = READ_FROM_STDIN;
			command.output_type=WRITE_TO_STDOUT;
			command.infile="";
			command.outfile="";
			continue;
			

		}
	}


	if(piped){
			cerr << "No output for pipe";
			return false;
	}
	if (!command.argv.empty()){
		if(command.output_type==WRITE_TO_PIPE){
			cerr << "No output for pipe";
			return false;
		}
		commands.push_back(command);
	}
	

  return true;
}
