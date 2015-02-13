#include <iostream>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Timer.h"
using namespace std;

void _timer(double wall, double user, double _system, Timer &t, char flag){
    t.elapsedWallclockTime(wall);
    t.elapsedUserTime(user);
    t.elapsedSystemTime(_system);
    cout << "Method " << flag << endl;
    cout << "Wall Clock: " << wall << endl;
    cout << "User Time: " << user << endl;
    cout << "System Time: " << system << endl << endl;
}

void cpMethod1(char* innam, char* outnam, char flag){
    struct stat ibuf, obuf;
    Timer t;
    double wall, user, _system;
    t.start();
    if(stat(innam, &ibuf)){
    perror("File does not exist");
    exit(1);
    }
    fstream check;
    check.open(outnam);
    if(check.is_open())
    {
        cerr << "File exist" << endl;
        check.close();
        exit(1);
    }
    fstream cpyfil,pstfil;
    
    cpyfil.open(innam);
    pstfil.open(outnam);
    
    if(!cpyfil.is_open()){
        perror("Input file failed to open");
        exit(1);
    }
    if(!pstfil.is_open()){
        pstfil.clear();
        pstfil.open(outnam, ios::out);
    }
    
    char c;
    while(cpyfil.get(c)){
        pstfil.put(c);
    }
    cpyfil.close();
    pstfil.close();
    check.close();
    _timer(wall, user, _system, t, flag);
}

void cpMethod2(char* innam, char* outnam, char flag){
    int fdi,fdo;
    Timer t;
    double wall, user, _system;
    t.start();
    if (-1==(fdi=open(innam,O_RDONLY))) {
        perror ("open input failed");
        exit(1);
    }
    if (-1==(fdo=open(outnam,O_WRONLY | O_CREAT | O_EXCL))) {
        perror("open output file failed");
        close(fdo);
        exit(1);
    }
    char buf[1];
    
    while(read(fdi,buf, sizeof(buf))){
        write(fdo, buf, sizeof(buf));
    }
    
    if(close(fdi)){
        perror("input file failed to close");
    }
    if(close(fdo)){
        perror("outputfile failed to close");
    }
    _timer(wall, user, _system, t, flag);
    return;
    
}

void cpMethod3(char* innam, char* outnam, char flag){
    int fdi,fdo;
    Timer t;
    double wall, user, _system;
    t.start();

    if (-1==(fdi=open(innam,O_RDONLY))) {
    perror ("open input failed");
        exit(1);
    }
    if (-1==(fdo=open(outnam,O_WRONLY | O_CREAT | O_EXCL))) {
        perror("open output file failed");
        close(fdo);
        exit(1);
    }
    char buf[BUFSIZ];
    int bytesize;
    while(bytesize = read(fdi,buf, BUFSIZ)){
        write(fdo, buf, bytesize);
    }
    
    if(close(fdi)){
        perror("input file failed to close");
    }
    if(close(fdo)){
        perror("outputfile failed to close");
    }
    _timer(wall, user, _system, t, flag);
    return;
    
}

int main(int argc, char * argv[]){
    //initialize defaults
    int i = 1, j = 1;
    char flags[] = "123";
    struct stat buf;
    char * inputFile, * outputFile;
    
    if(argc < 5){
        inputFile = argv[1];
        outputFile = argv[2];
    }
    else{
        cerr << "argc: too many arguments" << endl;
        exit(0);
    }
    if(argv[3] != NULL){
        if(strcmp(argv[3], "1") == 0) cpMethod1(inputFile, outputFile, flags[0]);
        else if(strcmp(argv[3], "2") == 0) cpMethod2(inputFile, outputFile, flags[1]);
        else cpMethod3(inputFile, outputFile, flags[2]);
    }
    else cpMethod3(inputFile, outputFile, flags[2]);
    
    return 0;
}