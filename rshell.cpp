#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
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

void usernam(string& nam){
    nam = getlogin();
    if (nam == "\0"){
        perror("Username could not be obtained");
        exit(1);
    }
    return;
}

void execRun(){
    string uin;
    string uname;
    char hnam[512];
    size_t hnamLen = 512;
    char* cmdsave;

    if(gethostname(hnam, hnamLen) == -1){
        perror("gethostname failed");
        exit(1);
    }
    usernam(uname);
        cout << uname << "@" << hnam <<  "$ ";
    getline(cin, uin);
    char* cstr = new char [uin.length()+1];
    strcpy(cstr, uin.c_str());
    vector<int> symbs;
    cstr = strtok(cstr, "#");

    for(unsigned j = 0; j < uin.size(); j++){
        if(uin.at(j) == '&' && uin.at(j+1) == '&'){
            symbs.push_back(0);
            j++;
        }
        else if(uin.at(j) == '&' && uin.at(j) != '&')
            symbs.push_back(0);
        else if( uin.at(j) == '|' && uin.at(j+1) == '|'){
            symbs.push_back(1);
            j++;
        }
        else if(uin.at(j) == '|' && uin.at(j+1) != '|')
            symbs.push_back(1);
        else if(uin.at(j) == ';')
            symbs.push_back(2);
    }
    symbs.push_back(2);

    char* cmdl = strtok_r(cstr, ";&|", &cmdsave);

    while(symbs.size() != 0 || cmdl != NULL){
        char* cmd = strtok(cmdl, " ");
        char* argv[2048];
        clr_argv(argv);
        argv[0] = cmd;
        char* ctemp = strtok(NULL," ");
        bool cmdpass = true;

        for(int i = 1; ctemp != NULL; i++){
            argv[i]= ctemp;
            ctemp = strtok(NULL, " ");
        }
        if(strcmp(cmd, "exit") == 0){
            exit(0);
        }


        int pid = fork();
        if(pid == -1){
            perror("fork failed");
            exit(1);
        }
        else if (pid == 0){
            if(-1 == execvp(cmd,argv)){
                perror("execvp failed");
                exit(1);
            }
            exit(0);
        }
        else{
            int wtret;
            int wtpid = wait(&wtret);
            if(-1 == wtpid){
                perror("wait failed");
                exit(1);
            }

            if (wtret == 0){
                cmdpass = true;
            }
            else{
                cmdpass = false;
            }
        }
        if(symbs.size() == 0)
            return;
        if(symbs.at(0) == 0){
            if(cmdpass == false){
                return;
            }
        }
        else if (symbs.at(0) == 1){
            if(cmdpass == true){
                return;
            }
        }
        symbs.erase(symbs.begin());
        cmdl = strtok_r(NULL, "&|;", &cmdsave);
    }
}

int main(){
    while(true){
        execRun();
    }
    return 0;
}
