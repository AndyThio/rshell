#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <utility>

using namespace std;

#define INITPF(x) x.second = true;
#define IFSTREQ(x) x.first.c_str() == argv[i] \
                        && x.second

/*
 * This is a BARE BONES example of how to use opendir/readdir/closedir.
 * Notice that there is no error checking on these functions.
 * You MUST add error checking yourself.
 */

int main(int argc, char** argv)
{
/*
 * flg is an indicator as to which flags were entered
 * 0) no flags were entered
 * 1) -a    2) -l   4) -R
 * 3) -a -l     5) -a -R    6) -l -R
 * 7) -l -R -a
 */
    pair<string,bool> da, dl, dR;
    INITPF(da);
    INITPF(dl);
    INITPF(dR);
    da.first = "-a";
    dl.first = "-l";
    dR.first = "-R";
    char filnam[512][256];
    int fnsz = 0;
    string curdir = ".";

    int flg = 0;
    for(int i = 1; i < argc; i++){
        if(IFSTREQ(da)){
            flg += 1;
            da.second = false;
        }
        else if(IFSTREQ(dl)){
            flg += 2;
            dl.second = false;
        }
        else if(IFSTREQ(dR)){
            flg += 4;
            dl.second = false;
        }
        else {
            strcpy(filnam[fnsz], argv[i]);
            fnsz++;
        }
    }
    if(fnsz == 0){
        fnsz++;
        strcpy(filnam[0],curdir.c_str());
    }

    for(int j = 0; j < fnsz; j++){
        char *dirName = filnam[j];
        DIR *dirp = opendir(dirName);
        if(dirp == NULL){
            perror("opendir failed");
            exit(1);
        }

        dirent *direntp;
        if(flg == 0){
            while ((direntp = readdir(dirp))){
                if(direntp->d_name[0] != '.')
                cout << direntp->d_name << endl;  // use stat here to find attributes of file
            }
        }
        if(closedir(dirp) == -1){
            perror("closedir failed");
            exit(1);
        }
    }
}

