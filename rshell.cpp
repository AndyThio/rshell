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
    tokand = strtok_r(cstr, ";&|", &svand);
    tokor = strtok_r(cstr, ";&|", &svor);
    tokcln = strtok_r(cstr, ";&|", &svcln);

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
            int wtret = wait(0);
            if(-1 == wtret){
                perror("wait failed");
                exit(1);
            }
            else if (wtret == 1){
                cmdpass = false;
            }
            else{
                cmdpass = true;
            }
        }
        if (!cmdpass && failstp)
            break;
cout << "starting block3" << endl;
        strcpy(svand,cmdsave);
        strcpy(svor,cmdsave);
        strcpy(svcln,cmdsave);
cout << "passed the 3block" << endl;
        tokand = strtok_r(NULL, "&", &svand);
        tokor = strtok_r(NULL, "|", &svor);
        tokcln = strtok_r(NULL, ";", &svcln);
cout << "starting to count" << endl;
cout << tokcln << endl;
        string andtemp = tokand;
cout << "WE HAVE NOT FAILED" << endl;
        int lenand = andtemp.size();
        string ortemp = tokor;
        int lenor = ortemp.size();
        string clntemp = tokcln;
        int lencln = clntemp.size();
cout << "ending counting " << endl;
        if(lenand > lenor && lenand > lencln){
            strcpy(cmdsave,svand);
            strcpy(cmd,tokand);
            failstp=true;
        }
        else if(lenor > lenand && lenor > lencln){
            strcpy(cmdsave,svor);
            strcpy(cmdl,tokor);
            failstp=false;
        }
        else{
            cout << cmdl << endl;
            strcpy(cmdsave,svcln);
            strcpy(cmdl,tokcln);
cout << endl << cmdl << endl << endl;
            failstp = false;
        }
    }
}

int main(){
    while(true){
        execRun();
    }
    return 0;
}
