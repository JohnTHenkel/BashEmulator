/**
 * This file contains the implementations of the builtin functions provided by
 * the shell (or will, once you've finished implementing them all).
 */

#include "shell.h"
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <cstdlib>
#include <regex>
#include <readline/history.h>
using namespace std;

int Shell::com_ls(vector<string>& argv) {
	if(argv.size()>2) return 1;
	DIR * dir;
	struct dirent *directory;
	if (argv.size()>1){
	
	dir = opendir(argv.at(1).c_str());
	}
	else{
        dir = opendir("./");
	}
	if (dir)
	{
		while ((directory= readdir(dir))!=NULL)
			{
				cout<< directory->d_name <<" ";
			}
		cout <<endl;

	}
	else{
		return 1;
	}
	closedir(dir);
  return 0;
}


int Shell::com_cd(vector<string>& argv) {
	
	if(argv.size()>1){
		if(argv.size()>2) return -1;
		return  chdir(argv.at(1).c_str());
	}
	else{
		 return chdir(getenv("HOME"));
	}
  return 1;
}


int Shell::com_pwd(vector<string>& argv) {
	if(argv.size()>1) return 1;
	char cCurrentPath[FILENAME_MAX];
	getcwd(cCurrentPath, FILENAME_MAX);
	cout << cCurrentPath << endl;

  return 0;
}


int Shell::com_alias(vector<string>& argv) {
  	if (argv.size()==1){
		for (auto& kv: aliases){
			cout<<"alias "<<kv.first<<"='"<<kv.second <<"'"<<endl;
		}
	}
	else{
		for (int i=1;i<static_cast<int>(argv.size());i++){
			regex r ("[A-z0-9].*=[A-z0-9].*");
			smatch m;
			if(regex_search(argv.at(i),m,r)){
				stringstream stream(argv.at(i));
				string key,value;
				getline(stream,key, '=');
				getline(stream,value,'=');
				aliases.insert({key,value});

			}
			else{
			return 1;
			}

		}

	}

  return 0;
}


int Shell::com_unalias(vector<string>& argv) {
 	if(argv.size()==1){
		cout<<"unalias requires an argument"<<endl;
		return 1;
	}
	else if(argv.size()>2) return 2;
	else if(argv.at(1)=="-a") aliases.clear();
	else{
		for (int i=1;i<static_cast<int>(argv.size());i++){
			aliases.erase(argv.at(i));
		}		
		
	}
	
	return 0;
}


int Shell::com_echo(vector<string>& argv) {
	for (int i=1;i<static_cast<int>(argv.size());i++){
		if(argv.at(i).at(0)=='$'){
		

		cout << getenv((argv.at(i).substr(1)).c_str());
		}
		else{

		cout<< argv.at(i) << " ";
		}
	}
	cout <<endl;

  return 0;
}


int Shell::com_history(vector<string>& argv) {
  	if(argv.size()>1) return 1;
	HIST_ENTRY **history;
		history=history_list();
	for (int i=0;history[i];i++){
		cout <<i+1 << " "<< history[i]->line<<endl;

	}
	return 0;
}


int Shell::com_exit(vector<string>& argv) {
 	exit(0);
  return 0;
}
