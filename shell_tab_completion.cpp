/**
 * This file contains implementations of the functions that provide
 * tab-completion for the shell.
 *
 * You will need to finish the implementations of the completion functions,
 * though you're spared from implementing the high-level readline callbacks with
 * their weird static variables...
 */

#include "shell.h"
#include <cstdlib>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
using namespace std;
extern char **environ;

void Shell::get_env_completions(const char* text, vector<string>& matches) {
	char*s = *environ;
	string envVar="";
	int i=1;
	for(;s;i++){
		envVar="$";
		envVar+=s;
		int pos = envVar.find("=");
		envVar.erase(pos,envVar.length()-1);
		if(envVar.find(text)==0){
			matches.push_back(envVar);
		}
		s=*(environ+i);
	}		


	
	for (auto & kv: localvars){
		envVar="$";
		envVar+=kv.first;
		if(envVar.find(text)==0){
			matches.push_back(envVar);
		}	
	}
}


void Shell::get_command_completions(const char* text, vector<string>& matches) {
	string test = "test";
 	for (auto & kv : aliases){
		if(kv.first.find(text)==0){
			matches.push_back(kv.first);
		}
	}
	for (auto & kv : builtins){

		if(kv.first.find(text)==0){
		matches.push_back(kv.first);
		}
	}
	string path = getenv("PATH");
	vector<string> directories;
	size_t pos=0;
	while((pos=path.find(":"))!=string::npos){
		directories.push_back(path.substr(0,pos));
		path.erase(0,pos+1);
	}
	directories.push_back(path);
	struct dirent *pathDirectory;
	DIR * dir;
	for(auto &direc:  directories){
		dir =opendir(direc.c_str());
		if(dir){
			while((pathDirectory=readdir(dir))!=NULL){
				string pathdir = pathDirectory->d_name;	
				if(pathdir.find(text)==0){
					matches.push_back(pathdir);
				}
			}	
		}
		else{
			
		}
		closedir(dir);
	}
}


char** Shell::word_completion(const char* text, int start, int end) {
  char** matches = NULL;

  if (text[0] == '$') {
    matches = rl_completion_matches(text, env_completion_generator);
  } else if (start == 0) {
    matches = rl_completion_matches(text, command_completion_generator);
  } else {
    // We get directory matches for free (thanks, readline!).
  }

  return matches;
}


char* Shell::env_completion_generator(const char* text, int state) {
  // A list of all the matches.
  // Must be static because this function is called repeatedly.
  static vector<string> matches;

  // If this is the first time called, construct the matches list with
  // all possible matches.
  if (state == 0) {
    getInstance().get_env_completions(text, matches);
  }

  // Return a single match (one for each time the function is called).
  return pop_match(matches);
}


char* Shell::command_completion_generator(const char* text, int state) {
  // A list of all the matches.
  // Must be static because this function is called repeatedly.
  static vector<string> matches;

  // If this is the first time called, construct the matches list with
  // all possible matches.
  if (state == 0) {
    getInstance().get_command_completions(text, matches);
  }

  // Return a single match (one for each time the function is called).
  return pop_match(matches);
}


char* Shell::pop_match(vector<string>& matches) {
  if (matches.size() > 0) {
    const char* match = matches.back().c_str();


    // We need to return a copy, because readline deallocates when done.
    char* copy = (char*) malloc(strlen(match) + 1);
    strcpy(copy, match);

    // Delete the last element.
    matches.pop_back();
    return copy;
  }

  // No more matches.
  return NULL;
}
