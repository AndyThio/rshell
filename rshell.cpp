#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

void printo(char** p){
    for(int c = 0; p[c] != NULL; c++){
        p[c] = NULL;
    }
    for(int i = 0; p[i] != NULL; i++){
        cout << p[i];
    }
    cout << endl;
}

void clr_argv(char** c){
    for(int i = 0; c[i] != NULL; i++){
        c[i] = NULL;
    }
}

void execRun(){
    string uin;
    cout << "$ ";
    getline(cin, uin);
    char* cstr = new char [uin.length()+1];
    strcpy(cstr, uin.c_str());

    char* cmdl = strtok(cstr, ";&|");
    while( cmdl != NULL){
        char* cmd = strtok(cmdl, " ");
        char* argv[512];
        clr_argv(argv);
        argv[0] = cmd;
        char* ctemp = strtok(NULL," ");
        for(int i = 1; ctemp != NULL; i++){
            argv[i]= ctemp;
            ctemp = strtok(NULL, " ");
        }
        int pid = fork();
        if(pid == -1){
            perror("fork failed");
            exit(1);
        }
        else if (pid == 0){
        execvp(cmd,argv);
            exit(0);
        }
        else{
            if(-1 == wait(0)){
                perror("wait failed");
                exit(1);
            }
        }
        cmdl = strtok(cstr, cmdl);
        cmdl = strtok(NULL, ";&|");
    }
}

int main(){
    while(true){
        execRun();
    }
    return 0;
}
