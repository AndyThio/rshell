#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <string>
#include <utility>
#include <iomanip>

using namespace std;

#define INITPF(x) x.second = true;
#define IFSTREQ(x) strcmp(x.first.c_str(),argv[i]) == 0 \
                        && x.second

/*
 * This is a BARE BONES example of how to use opendir/readdir/closedir.
 * Notice that there is no error checking on these functions.
 * You MUST add error checking yourself.
 */
void printl(const dirent* dprt){
    cout << setw(30) << left << dprt->d_name;
}

void printr(const dirent* dprt){
    cout << setw(30) << left << dprt->d_name << endl;
}

void printr(const char* dprt){
    cout << setw(30) << left << dprt << endl;
}

void fpermis(const struct stat* buf){
    cout << ((buf->st_mode & S_IFDIR)?"d":"-");

    cout << ((buf->st_mode & S_IRUSR)?"r":"-");
    cout << ((buf->st_mode & S_IWUSR)?"w":"-");
    cout << ((buf->st_mode & S_IXUSR)?"x":"-");

    cout << ((buf->st_mode & S_IRGRP)?"r":"-");
    cout << ((buf->st_mode & S_IRGRP)?"w":"-");
    cout << ((buf->st_mode & S_IRGRP)?"x":"-");

    cout << ((buf->st_mode & S_IROTH)?"r":"-");
    cout << ((buf->st_mode & S_IROTH)?"w":"-");
    cout << ((buf->st_mode & S_IROTH)?"x":"-");
    cout << " ";
//inode
    cout << setw(5) << right << buf->st_nlink << " ";
}

void filesz(const struct stat* buf);

void fdate(const struct stat* buf){
    char* ftime = ctime((time_t*)&buf->st_mtime);
    if( ftime == NULL){
        perror("ctime failed");
        exit(1);
    }
    for(int i = 0; ftime[i] != '\n' ; i++){
        cout << ftime[i];
    }
    cout << " ";
}

char *pathctor(const dirent* dirp, const char* currf){
    int slen = strlen(currf);
    int slen2 = strlen(dirp->d_name);
    char dirpc[512];
    if(currf[0] == '.' && currf[1] == '\0'){
        strcpy(dirpc, dirp->d_name);
    }
    else{
        int i;
        for( i = 0; i < (slen+slen2+1);i++){
            if(i < slen)
                dirpc[i] = currf[i];
            else if(i > slen)
                dirpc[i] = dirp->d_name[i - slen-1];
            else
                dirpc[i] = '/';
        }
        dirpc[i] = '\0';
    }
    char* ret = dirpc;
    return ret;
}

void lsl(const dirent* dirp){
    struct stat buf;

    if(stat(dirp->d_name, &buf)){
        perror("stat failed");
        exit(1);
    }
    fpermis(&buf);
//    fusergrp(&buf);
    //filesz(&buf);
    fdate(&buf);
    printr(dirp);
}


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

//printing out according to flags
        dirent *direntp;
        if(flg == 0){
            while ((direntp = readdir(dirp))){
                if(direntp->d_name[0] != '.')
                cout << direntp->d_name << endl;
            }
        }
        else if(flg == 1){
            while ((direntp = readdir(dirp)))
                cout << direntp->d_name << endl;
        }

        else if (flg == 2){
            while((direntp = readdir(dirp))){
                lsl(direntp);
            }
        }



//closing opened directory
        if(closedir(dirp) == -1){
            perror("closedir failed");
            exit(1);
        }
    }
}

