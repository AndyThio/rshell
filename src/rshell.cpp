#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <vector>
#include <unordered_map>
#include <dirent.h>
#include <signal.h>
using namespace std;

#define SEPS "|&;><"

typedef void (*redircmd) ();
//comhands
//exit

string currwrkDir(){
    char buf[FILENAME_MAX];
    if(getcwd(buf,sizeof(buf)) == NULL){
        perror("getcwd failed");
        exit(1);
    }
    string ret = buf;
    return ret;
}

char* execRun(char* command){
    vector<char*> paths;

    char *pathHolder = getenv("PATH");
    if (pathHolder == NULL){
        perror("getenv failed");
        exit(1);
    }

    char *pathList = pathHolder;
    strcpy(pathList,pathHolder);
    char* currPATH = strtok(pathList,":");

    while(currPATH != NULL){
        paths.emplace_back(currPATH);
        currPATH = strtok(NULL, ":");
    }

    for(auto &e : paths){
        DIR* currDirent = opendir(e);
        if(currDirent == NULL){
            if(false){
                perror("execRun opendir failed");
            }
        }
        else{
            dirent* filname;
            //error check here

            while((filname = readdir(currDirent))){
                if(strcmp(command,filname->d_name) == 0){
                    char slashtemp[] = "/";
                    char* temp = strcat(slashtemp, command);
                    return strcat(e,temp);
                }
            }
        }
    }
    return command;
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
//TODO: make sure you can link them (fix your last check thingy)
//TODO: :wq
//
void execvpRun(){
    int numsymbs = 0;
    string uin;
    char* cmdsave;
    cout << endl << "\033[35m" << currwrkDir() << endl;

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
    cout << "\033[32m" << uname << "@" << hnam <<  "$ \033[39m";
    getline(cin, uin);
    if((uin.size() != 0 && uin.at(0) == '#')|| uin.size() == 0)
        return;
    char ctorthing[BUFSIZ];
    char* cstr = ctorthing;
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
     * <<< == 7
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
        else if(uin.at(j) == '<' && uin.at(j+1) == '<'
                    && uin.at(j+2) == '<')
            symbs.push_back(7);
    }

    char* cmdl = strtok_r(cstr, SEPS, &cmdsave);
    while(symbs.size() > numsymbs && cmdl != NULL){
        char* cmd = strtok(cmdl, " ");
        if(cmd == NULL){
            cmdl = strtok_r(NULL, SEPS, &cmdsave);
            numsymbs++;
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
        else if(strcmp(cmd, "cd") == 0){
            if(argv[1] == NULL){
                char* homevar = getenv("HOME");
                if (homevar == NULL){
                    perror("getenv failed");
                    exit(1);
                }
                if(-1 == chdir(homevar)){
                    perror("chdir failed");
                    exit(1);
                }
            }
            else if(-1 == chdir(argv[1])){
                perror("chdir failed");
                exit(1);
            }
        }

        else if(symbs.at(numsymbs) == 3){
            char* cmdtemp3 = strtok_r(NULL, SEPS, &cmdsave);
            char* argv4[2048];
            clr_argv(argv4);
            argv4[0] = strtok(cmdtemp3, " ");
            for(int i = 1; argv4[i-1] != NULL; i++){
                argv4[i] = strtok(NULL, " ");
            }
            int fd4[2];
            if(pipe(fd4) == -1){
                perror("pipe failed");
                exit(1);
            }
            int pid4 = fork();
            if(pid4 == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid4 == 0){
                if(dup2(fd4[1],1)== -1){
                    perror("dup failed");
                    exit(1);
                }
                if(close(fd4[1]) == -1){
                    perror("close failed");
                    exit(1);
                }
                if(close(fd4[0])==-1){
                    perror("close failed");
                    exit(1);
                }
                if(-1 == execv(execRun(cmd),argv)){
                    perror("exec failed");
                    exit(1);
                }
                exit(0);
            }
            if(wait(0) == -1){
                perror("piping failed");
                exit(1);
            }
            int pid42 = fork();
            if(pid42 == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid42 == 0){
                if(dup2(fd4[0],0) == -1){
                    perror("dup failed");
                    exit(1);
                }
                if(close(fd4[1]) == -1){
                    perror("close failed");
                    exit(1);
                }
                if(close(fd4[0])==-1){
                    perror("close failed");
                    exit(1);
                }
                if(-1 == execv(execRun(argv4[0]),argv4)){
                    perror("exec failed");
                    exit(1);
                }
                exit(0);
            }
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

        else if(symbs.at(numsymbs) == 4){
            int fdnum4;
            char* temptest4 = strtok_r(NULL, SEPS, &cmdsave);
            temptest4 = strtok(temptest4, " ");
            int fdopen4 = open(temptest4, O_RDONLY);
            if(fdopen4 == -1){
                perror("open failed");
                exit(1);
            }
            int pid = fork();
            if(pid == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid == 0){
                if(SIG_ERR == signal(SIGINT, SIG_DFL)){
                    perror("singal failed");
                    exit(1);
                }
                if( -1 == ( fdnum4 = dup2(fdopen4,0))){
                    perror("dup failed");
                    exit(1);
                }
                if(-1 == execv(execRun(cmd),argv)){
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
                if(wtret == 0){
                    cmdpass = true;
                }
                else{
                    cmdpass = false;
                }
            }
            if(-1 == close(fdopen4)){
                perror("open failed");
                exit(1);
            }
        }

        else if(symbs.at(numsymbs) == 5){
            int usefd5 = 1;
            int i = 1;
            while(argv[i] != NULL){
                if(argv[i+1] == NULL &&
                    (strcmp(argv[1],"2") == 0 || strcmp(argv[1], "0") ==0)){
                    usefd5 = atoi(argv[1]);
                    argv[i] = NULL;
                }
            }
            int fdnum5;
            char* temptest = strtok_r(NULL, SEPS, &cmdsave);
            temptest = strtok(temptest, " ");
            int fdopen5 = open(temptest, O_WRONLY | O_CREAT | O_TRUNC);
            fchmod(fdopen5, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
            if(fdopen5 == -1){
                perror("open failed");
                exit(1);
            }
            int pid = fork();
            if(pid == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid == 0){
                if(SIG_ERR == signal(SIGINT, SIG_DFL)){
                    perror("singal failed");
                    exit(1);
                }
                if( -1 == ( fdnum5 = dup2(fdopen5,usefd5))){
                    perror("dup failed");
                    exit(1);
                }
                if(-1 == execv(execRun(cmd),argv)){
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
                if(wtret == 0){
                    cmdpass = true;
                }
                else{
                    cmdpass = false;
                }
            }
            if(-1 == close(fdopen5)){
                perror("open failed");
                exit(1);
            }
        }

        else if(symbs.at(numsymbs) == 6){
            int usefd6 = 1;
            int i = 1;
            while(argv[i] != NULL){
                if(argv[i+1] == NULL &&
                    (strcmp(argv[1],"2") == 0 || strcmp(argv[1], "0") ==0)){
                    usefd6 = atoi(argv[1]);
                    argv[i] = NULL;
                }
            }
            int fdnum6;
            char* temptest6 = strtok_r(NULL, SEPS, &cmdsave);
            temptest6 = strtok(temptest6, " ");
            int fdopen6 = open(temptest6, O_WRONLY | O_CREAT | O_APPEND);
            fchmod(fdopen6, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
            if(fdopen6 == -1){
                perror("open failed");
                exit(1);
            }
            int pid = fork();
            if(pid == -1){
                perror("fork failed");
                exit(1);
            }
            else if(pid == 0){
                if(SIG_ERR == signal(SIGINT, SIG_DFL)){
                    perror("singal failed");
                    exit(1);
                }
                if( -1 == ( fdnum6 = dup2(fdopen6,usefd6))){
                    perror("dup failed");
                    exit(1);
                }
                if(-1 == execv(execRun(cmd),argv)){
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
                if(wtret == 0){
                    cmdpass = true;
                }
                else{
                    cmdpass = false;
                }
            }
            if(-1 == close(fdopen6)){
                perror("open failed");
                exit(1);
            }
            if(pid == -1){
                perror("fork failed");
                exit(1);
            }
        }

        else if(symbs.at(numsymbs) == 7){
            string input7 = strtok_r(NULL, SEPS, &cmdsave);
            int fd[2];
            if(pipe(fd) == -1){
                perror("pipe failed");
                exit(1);
            }
            if(input7.find("\"") != string::npos){
                input7 = input7.substr(input7.find("\"")+1);
                input7 = input7.substr(0,input7.find("\""));
                char buf[BUFSIZ];
                strcpy(buf, input7.c_str());
                if(-1==write(fd[1], buf, input7.size())){
                    perror("write failed");
                    exit(1);
                }
                if(close(fd[1]) == -1){
                    perror("close failed");
                    exit(1);
                }
                int pid = fork();
                if(pid == -1){
                    perror("fork failed");
                    exit(1);
                }
                else if(pid == 0){
                    if(SIG_ERR == signal(SIGINT, SIG_DFL)){
                        perror("singal failed");
                        exit(1);
                    }
                    if(fd[0] >= 0){
                        if(-1==dup2(fd[0],STDIN_FILENO)){
                            perror("dup failed");
                            exit(1);
                        }
                        if(close(fd[0]) == -1){
                            perror("close failed");
                            exit(1);
                        }
                    }
                    if(-1 == execv(execRun(cmd),argv)){
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
                    if(wtret == 0){
                        cmdpass = true;
                    }
                    else{
                        cmdpass = false;
                    }
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
                if(SIG_ERR == signal(SIGINT, SIG_DFL)){
                    perror("singal failed");
                    exit(1);
                }
                if(-1 == execv(execRun(cmd),argv)){
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
        if(symbs.size() <= numsymbs)
            return;
        if(symbs.at(numsymbs) == 0){
            if(cmdpass == false){
                return;
            }
        }
        else if (symbs.at(numsymbs) == 1){
            if(cmdpass == true){
                return;
            }
        }
        cmdl = strtok_r(NULL, SEPS, &cmdsave);
        numsymbs++;
        }
    }
}

void sigHandler(int sig){
    execvpRun();
}

int main(){
    if(SIG_ERR == signal(SIGINT,sigHandler )){
        perror("signal failed");
        exit(1);
    }
    while(true){
        execvpRun();
    }
    return 0;
}
