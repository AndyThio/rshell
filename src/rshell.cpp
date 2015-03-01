#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>
#include <unordered_map>
using namespace std;

#define SEPS "|&;><"

typedef void (*redircmd) ();
//comhands
//exit
void endprog(){
    exit(0);
}

// <
void inpip(char* innam){
    int fdi;
    if (-1==(fdi=open(innam,O_RDONLY))) {
        perror ("open input failed");
        exit(1);
    }
    char buf[BUFSIZ];
    int bytesize;
    while(bytesize = read(fdi,buf, (BUFSIZ))){
        if(bytesize == -1){
            perror("read failed");
            exit(1);
        }
        if(-1 == write(0, buf, bytesize)){
            perror("write to stdin failed");
            exit(1);
        }
    }

    if(close(fdi)){
        perror("input file failed to close");
    }
    return;
}

// >
void outpip(char* innam, int fdnum){
    int fdo;
    if ( -1 == (fdo=open(innam, O_WRONLY | O_CREAT | S_IRWXU
                    | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))) {
        perror ("open outputfile failed");
        exit(1);
    }
    char buf[BUFSIZ];
    int bytesize;
    close(1);
    while(bytesize = read(fdnum, buf, (BUFSIZ))){
        if(bytesize == -1){
            perror("read failed");
            exit(1);
        }
        if(-1 == write(fdo, buf, bytesize)){
            perror("write failed");
            exit(1);
        }
    }
    if(close(fdo)){
        perror("close failed");
    }
}

// >>
void cerrpip(char* innam){
    int fdo;
    if ( -1 == (fdo=open(innam, O_WRONLY | O_CREAT | O_APPEND
                    | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))) {
        perror ("open outputfile failed");
        exit(1);
    }
    char buf[BUFSIZ];
    int bytesize;
    close(2);
    while(bytesize = read(1, buf, (BUFSIZ))){
        if(bytesize == -1){
            perror("read failed");
            exit(1);
        }
        if(-1 == write(fdo, buf, bytesize)){
            perror("write failed");
            exit(1);
        }
    }
    if(close(fdo)){
        perror("close failed");
    }
}

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


void execRun(char* uname, char hnam[]){
    string uin;
    char* cmdsave;

    cout << uname << "@" << hnam <<  "$ ";
    getline(cin, uin);
    if((uin.size() != 0 && uin.at(0) == '#')|| uin.size() == 0)
        return;
    char* cstr = new char [uin.length()+1];
    strcpy(cstr, uin.c_str());
    vector<int> symbs;
    cstr = strtok(cstr, "#");
    /* && == & == 0
     * || == 1
     * ; == 2
     * | == 3
     * < == 4
     * > == 5
     * >> == 6
     */
    for(unsigned j = 0; j < uin.size()-1; j++){
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
            symbs.push_back(3);
        else if(uin.at(j) == ';')
            symbs.push_back(2);
        else if(uin.at(j) == '<' && uin.at(j+1) != '<')
            symbs.push_back(4);
        else if(uin.at(j) == '>' && uin.at(j+1) != '>')
            symbs.push_back(5);
        else if(uin.at(j) == '>' && uin.at(j+1) == '>')
            symbs.push_back(6);
    }
    symbs.push_back(2);

    char* cmdl = strtok_r(cstr, SEPS, &cmdsave);
    while(symbs.size() != 0 && cmdl != NULL){
        char* cmd = strtok(cmdl, " ");
        if(cmd == NULL){
            symbs.erase(symbs.begin());
            cmdl = strtok_r(NULL, SEPS, &cmdsave);
        }
        else{
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

        if(symbs.at(0) == 4){
            int pid = fork();
            if(pid == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid == 0){
            }
            else{
                int wtret;
                int wtpid = wait(&wtret);
                if(-1 == wtpid){
                    perror("wait failed");
                    exit(1);
                }
                if(wtret == 0){
                    cmdpass = true;
                }
                else{
                    cmdpass = false;
                }
            }
        }

        else if(symbs.at(0) == 5){
            int fdnum;
            if( -1 == ( fdnum = dup(1))){
                perror("dup failed");
                exit(1);
            }
            if(-1==close(1)){
                perror("close failed");
                exit(1);
            }
            int pid = fork();
            if(pid == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid == 0){
                int pid2 = fork();
                if(pid2 == -1){
                    perror("fork failed");
                    exit(1);
                }
                else if(pid2 == 0){
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
                    outpip(strtok_r(cstr, SEPS, &cmdsave),fdnum);
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
                if(wtret == 0){
                    cmdpass = true;
                }
                else{
                    cmdpass = false;
                }
            }
        }

        else if(symbs.at(0) == 6){
            int pid = fork();
            if(pid == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid == 0){
            }
            else{
                int wtret;
                int wtpid = wait(&wtret);
                if(-1 == wtpid){
                    perror("wait failed");
                    exit(1);
                }
                if(wtret == 0){
                    cmdpass = true;
                }
                else{
                    cmdpass = false;
                }
            }
        }

        else{
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
        cmdl = strtok_r(NULL, SEPS, &cmdsave);
        }
    }
}

int main(){
    char* uname;
    char hnam[512];
    size_t hnamLen = 512;
    if(gethostname(hnam, hnamLen) == -1){
    perror("gethostname failed");
    exit(1);
    }
    uname = getlogin();
    if(uname ==  NULL){
        perror("Username could not be obtained");
        char nam[] = "unknown";
        uname = nam;
    }
    while(true){
        execRun(uname,hnam);
    }
    return 0;
}
