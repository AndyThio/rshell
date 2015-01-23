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
    char* cmdsave, *svand, *svor, *svcln;
    char* tokand, *tokor, *tokcln;
    bool failstp = false;
    bool boolcln = false;

    if(gethostname(hnam, hnamLen) == -1){
        perror("gethostname failed");
        exit(1);
    }
    usernam(uname);
        cout << uname << "@" << hnam <<  "$ ";
    getline(cin, uin);
    char* cstr = new char [uin.length()+1];
    strcpy(cstr, uin.c_str());

    char* cmdl = strtok_r(cstr, ";&|", &cmdsave);
    tokand = strtok(cstr, "&");
    tokor = strtok(cstr, "|");
    tokcln = strtok(cstr, ";");
cout << tokand << endl << tokor << endl << tokcln << endl;
    size_t lenand = strlen(tokand);
    size_t lenor = strlen(tokor);
    size_t lencln = strlen(tokcln);
cout << lenand << endl << lenor << endl << lencln << endl;
    if(lenand > lenor && lenand > lencln){
        failstp=true;
        boolcln = false;
    }
    else if(lenor > lenand && lenor > lencln){
        failstp=false;
        boolcln = false;
    }
    else{
        boolcln = true;
    }

    while( cmdl != NULL){
        char* cmd = strtok(cmdl, " ");
        char* argv[512];
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
cout << "cmdpass: " << cmdpass << endl << "failstp: " << failstp << endl;
        if (!cmdpass && failstp || cmdpass && !failstp && boolcln)
            return;
        strcpy(svand,cmdsave);
        strcpy(svor,cmdsave);
        strcpy(svcln,cmdsave);
cout << " WE ARE DOING CMDL" << endl;
        cmdl = strtok_r(NULL, "&|;", &cmdsave);
        cout << cmdl << endl;
        tokand = strtok_r(NULL, "&", &svand);
        tokor = strtok_r(NULL, "|", &svor);
        tokcln = strtok_r(NULL, ";", &svcln);
        if(tokand == NULL || tokor == NULL || tokcln == NULL)
            return;
        size_t lenand = strlen(tokand);
        size_t lenor = strlen(tokor);
        size_t lencln = strlen(tokcln);
        if(lenand > lenor && lenand > lencln){
            failstp=true;
            boolcln = false;
        }
        else if(lenor > lenand && lenor > lencln){
            failstp=false;
            boolcln = false;
        }
        else{
            boolcln = true;
        }
    }
}

int main(){
    while(true){
        execRun();
    }
    return 0;
}
