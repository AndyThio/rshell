#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

#include <iostream>
#include <string>
#include <utility>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <unordered_map>

using namespace std;

#define INITPF(x) x.second = true;
#define IFSTREQ(x) strcmp(x.first.c_str(),argv[i]) == 0 \
                        && x.second

/*
 * This is a BARE BONES example of how to use opendir/readdir/closedir.
 * Notice that there is no error checking on these functions.
 * You MUST add error checking yourself.
 */

#define IFHEXEC() (buf->st_mode & S_IXUSR || buf->st_mode & S_IXGRP \
                    || buf->st_mode & S_IXOTH) && dprt->d_name[0] == '.' \
                    && dprt->d_type != DT_DIR
#define IFEXEC() (buf->st_mode & S_IXUSR || buf->st_mode & S_IXGRP \
                    || buf->st_mode & S_IXOTH) && dprt->d_type != DT_DIR
void printl(const dirent* dprt, const int maxflen, const struct stat* buf){
    if(dprt->d_type == DT_DIR && dprt->d_name[0] == '.'){
        cout << "\033[34;100m" << setw(maxflen + 2) << left<< dprt->d_name
            << "\033[39;49m";
    }
    else if(IFHEXEC()){
        cout << "\033[92;100m" << setw(maxflen +2) << left << dprt->d_name
            << "\033[39;49m";
    }
    else if(IFEXEC()){
        cout << "\033[92m" << setw(maxflen + 2) << left << dprt->d_name
            << "\033[39m";
    }
    else if(dprt->d_type == DT_DIR){
        cout << "\033[34m" << setw(maxflen + 2) << left<< dprt->d_name
            << "\033[39m";
    }

    else
        cout << setw(maxflen + 2) << left << dprt->d_name;
}

void printr(const dirent* dprt, const struct stat* buf){
    if(dprt->d_type == DT_DIR && dprt->d_name[0] == '.'){
        cout << "\033[34;100m" << left << dprt->d_name << "\033[39;49m" << endl;
    }
    else if(IFHEXEC()){
        cout << "\033[92;100m" << left << dprt->d_name
            << "\033[39;49m" << endl;
    }
    else if(IFEXEC()){
        cout << "\033[92m" <<left << dprt->d_name
            << "\033[39m" << endl;;
    }
    else if(dprt->d_type == DT_DIR){
        cout << "\033[34m" << left << dprt->d_name
            << "\033[39m" << endl;
    }
    else
        cout << dprt->d_name << endl;
}

void fpermis(const struct stat* buf){
    cout << ((buf->st_mode & S_IFDIR)?"d":"-");

    cout << ((buf->st_mode & S_IRUSR)?"r":"-");
    cout << ((buf->st_mode & S_IWUSR)?"w":"-");
    cout << ((buf->st_mode & S_IXUSR)?"x":"-");

    cout << ((buf->st_mode & S_IRGRP)?"r":"-");
    cout << ((buf->st_mode & S_IWGRP)?"w":"-");
    cout << ((buf->st_mode & S_IXGRP)?"x":"-");

    cout << ((buf->st_mode & S_IROTH)?"r":"-");
    cout << ((buf->st_mode & S_IWOTH)?"w":"-");
    cout << ((buf->st_mode & S_IXOTH)?"x":"-");
    cout << " ";
//inode
    cout << setw(5) << right << buf->st_nlink << " ";
}

void filesz(const struct stat* buf){
        cout << setw(6) << right  << buf->st_size << " ";
    }

#define MONTM(x,y) if(ftime->tm_mon == x) \
                    cout << y << " ";

void fdate(const struct stat* buf){
    struct tm* ftime = localtime((time_t*)&buf->st_mtime);
    if( ftime == NULL){
        perror("localtime failed");
        exit(1);
    }
    if(ftime->tm_mon == 0)
        cout << "Jan" << " ";
    MONTM(1,"Feb");
    MONTM(2,"Mar");
    MONTM(3,"Apr");
    MONTM(4,"May");
    MONTM(5,"Jun");
    MONTM(6,"Jul");
    MONTM(7,"Aug");
    MONTM(8,"Sep");
    MONTM(9,"Oct");
    MONTM(10,"Nov");
    MONTM(11,"Dec");

    cout <<setw(2) << right << ftime->tm_mday << " ";
    cout << setw(2) << right << setfill('0') << ftime->tm_hour << ":";
    cout << setw(2) << right << setfill('0') << ftime->tm_min << " ";
    cout << setfill(' ');
}

string pathctor(const char* dirp, const char* currf){
    int slen = strlen(currf);
    int slen2 = strlen(dirp);
    char dirpc[512];
    if(currf[0] == '.' && currf[1] == '\0'){
        strcpy(dirpc, dirp);
    }
    else{
        int i;
        for( i = 0; i < (slen+slen2+1);i++){
            if(i < slen)
                dirpc[i] = currf[i];
            else if(i > slen)
                dirpc[i] = dirp[i - slen-1];
            else
                dirpc[i] = '/';
        }
        dirpc[i] = '\0';
    }
    string ret = dirpc;
    return ret;
}

void fusergrp(const struct stat* buf){
    struct passwd *pw = getpwuid(buf->st_uid);
    if(pw)
        cout << setw(12) << pw->pw_name << " ";
    else{
        perror("getpwuid failed");
        exit(1);
    }
    struct group* gpw = getgrgid(buf->st_gid);
    if(gpw == NULL){
        perror("getgrgid failed");
        exit(1);
    }
    cout<< setw(8) << gpw->gr_name << " ";
}

void lsl(const dirent* dirp, const char* fpath){
    struct stat buf;

    if(stat(fpath, &buf)){
        perror("stat lsl failed");
        exit(1);
    }
    fpermis(&buf);
    fusergrp(&buf);
    filesz(&buf);
    fdate(&buf);
    printr(dirp, &buf);
}

#define CTOR_BUF(x) struct stat buf; \
                        if(stat(x, &buf)){ \
                            perror("stat print failed"); \
                            exit(1); \
                        }
#define CTOR_BUF2(x,y) struct stat buf; \
                        string temppah = pathctor(x,y); \
                        if(stat(temppah.c_str(), &buf)){ \
                            perror("stat print 2 failed"); \
                            exit(1); \
                        }
void lsR(const dirent* drtp, string pathn){
    string prevfil = "..";
    if(drtp->d_type == DT_DIR && strcmp(drtp->d_name,prevfil.c_str()) != 0){
        DIR *dirp = opendir(pathn.c_str());
        if(dirp == NULL){
            perror("opendir failed");
            exit(1);
        }
        cout << pathn << ":" << endl;
        unordered_map<string, dirent*> filist;
        dirent *direntp;
        vector<string> fnam;
        direntp = readdir(dirp);
        if(!direntp){
            perror("readdir failed");
            exit(1);
        }
        string tempfnam = direntp->d_name;
        filist.emplace(make_pair(tempfnam, direntp));
        fnam.push_back(tempfnam);
        unsigned maxflen = tempfnam.size();
        while ((direntp = readdir(dirp))){
            tempfnam = direntp->d_name;
            filist.emplace(make_pair(tempfnam, direntp));
            fnam.push_back(tempfnam);
            if(maxflen < tempfnam.size())
                maxflen = tempfnam.size();
        }
        sort(fnam.begin(), fnam.end(), locale("en_US.UTF-8"));

        bool left = true;
        for(auto &e: fnam){
            auto fsearch = filist.find(e);
            if(left){
                if(e.c_str()[0] != '.'){
                    CTOR_BUF2(e.c_str(), pathn.c_str());
                    printl(fsearch->second, maxflen, &buf);
                    left = false;
                }
            }
            else if(!left){
                if(e.c_str()[0] != '.'){
                    CTOR_BUF2(e.c_str(), pathn.c_str());
                    printr(fsearch->second, &buf);
                    left = true;
                }
            }
        }
        if(!left)
            cout << endl;
        cout << endl;
        if(pathn.find("/.") != pathn.size()-2){
            for(auto &e: fnam){
                if(e.c_str()[0] != '.'){
                    auto fsrch = filist.find(e);
                    lsR(fsrch->second, pathctor(fsrch->second->d_name, pathn.c_str()));
                }
            }
        }
        if(closedir(dirp) == -1){
            perror("closedir failed");
            exit(1);
        }
    }
}

void lsRa(const dirent* drtp, string pathn){
    string prevfil = "..";
    if(drtp->d_type == DT_DIR && strcmp(drtp->d_name,prevfil.c_str()) != 0){
        DIR *dirp = opendir(pathn.c_str());
        if(dirp == NULL){
            perror("opendir failed");
            exit(1);
        }
        cout << pathn << ":" << endl;
        unordered_map<string, dirent*> filist;
        dirent *direntp;
        vector<string> fnam;
        direntp = readdir(dirp);
        if(!direntp){
            perror("readdir failed");
            exit(1);
        }
        string tempfnam = direntp->d_name;
        filist.emplace(make_pair(tempfnam, direntp));
        fnam.push_back(tempfnam);
        unsigned maxflen = tempfnam.size();
        while ((direntp = readdir(dirp))){
            tempfnam = direntp->d_name;
            filist.emplace(make_pair(tempfnam, direntp));
            fnam.push_back(tempfnam);
            if(maxflen < tempfnam.size())
                maxflen = tempfnam.size();
        }
        sort(fnam.begin(), fnam.end(), locale("en_US.UTF-8"));

        bool left = true;
        for(auto &e: fnam){
            auto fsearch = filist.find(e);
            if(left){
                CTOR_BUF2(e.c_str(), pathn.c_str());
                printl(fsearch->second, maxflen, &buf);
                left = false;
            }
            else if(!left){
                CTOR_BUF2(e.c_str(), pathn.c_str());
                printr(fsearch->second, &buf);
                left = true;
            }
        }
        if(!left)
            cout << endl;
        cout << endl;
        if(pathn.find("/.") != pathn.size()-2){
            for(auto &e: fnam){
                if(strcmp(e.c_str(), ".") != 0 && strcmp(e.c_str(), "..")){
                    auto fsrch = filist.find(e);
                    lsRa(fsrch->second, pathctor(fsrch->second->d_name, pathn.c_str()));
                }
            }
        }
        if(closedir(dirp) == -1){
            perror("closedir failed");
            exit(1);
        }
    }
}

void lsRl(const dirent* drtp, string pathn){
    string prevfil = "..";
    if(drtp->d_type == DT_DIR && strcmp(drtp->d_name,prevfil.c_str()) != 0){
        DIR *dirp = opendir(pathn.c_str());
        if(dirp == NULL){
            perror("opendir failed");
            exit(1);
        }
        cout << pathn << ":" << endl;
        unordered_map<string, dirent*> filist;
        dirent *direntp;
        vector<string> fnam;
        direntp = readdir(dirp);
        if(!direntp){
            perror("readdir failed");
            exit(1);
        }
        string tempfnam = direntp->d_name;
        filist.emplace(make_pair(tempfnam, direntp));
        fnam.push_back(tempfnam);
        unsigned maxflen = tempfnam.size();
        while ((direntp = readdir(dirp))){
            tempfnam = direntp->d_name;
            filist.emplace(make_pair(tempfnam, direntp));
            fnam.push_back(tempfnam);
            if(maxflen < tempfnam.size())
                maxflen = tempfnam.size();
        }
        sort(fnam.begin(), fnam.end(), locale("en_US.UTF-8"));

        for(auto &e: fnam){
            auto fsearch = filist.find(e);
            if(e.c_str()[0] != '.'){
                string temppah = pathctor(e.c_str(), pathn.c_str());
                lsl(fsearch->second, temppah.c_str());
            }
        }
        cout << endl;
        if(pathn.find("/.") != pathn.size()-2){
            for(auto &e: fnam){
                if(e.c_str()[0] != '.'){
                    auto fsrch = filist.find(e);
                    lsRl(fsrch->second, pathctor(fsrch->second->d_name, pathn.c_str()));
                }
            }
        }
        if(closedir(dirp) == -1){
            perror("closedir failed");
            exit(1);
        }
    }
}

void lsRla(const dirent* drtp, string pathn){
    string prevfil = "..";
    if(drtp->d_type == DT_DIR && strcmp(drtp->d_name,prevfil.c_str()) != 0){
        DIR *dirp = opendir(pathn.c_str());
        if(dirp == NULL){
            perror("opendir failed");
            exit(1);
        }
        cout << pathn << ":" << endl;
        unordered_map<string, dirent*> filist;
        dirent *direntp;
        vector<string> fnam;
        direntp = readdir(dirp);
        if(!direntp){
            perror("readdir failed");
            exit(1);
        }
        string tempfnam = direntp->d_name;
        filist.emplace(make_pair(tempfnam, direntp));
        fnam.push_back(tempfnam);
        unsigned maxflen = tempfnam.size();
        while ((direntp = readdir(dirp))){
            tempfnam = direntp->d_name;
            filist.emplace(make_pair(tempfnam, direntp));
            fnam.push_back(tempfnam);
            if(maxflen < tempfnam.size())
                maxflen = tempfnam.size();
        }
        sort(fnam.begin(), fnam.end(), locale("en_US.UTF-8"));

        for(auto &e: fnam){
            auto fsearch = filist.find(e);
            string temppah = pathctor(e.c_str(), pathn.c_str());
            lsl(fsearch->second, temppah.c_str());
        }
        cout << endl;
        if(pathn.find("/.") != pathn.size()-2){
            for(auto &e: fnam){
                if(strcmp(e.c_str(), ".") != 0 && strcmp(e.c_str(), "..")){
                    auto fsrch = filist.find(e);
                    lsRla(fsrch->second, pathctor(fsrch->second->d_name, pathn.c_str()));
                }
            }
        }
        if(closedir(dirp) == -1){
            perror("closedir failed");
            exit(1);
        }
    }
}

#define DMUIF(x) x.second && (strcmp(x.first.first.c_str(), argv[i]) == 0 \
                    || strcmp(x.first.second.c_str(),argv[i]) == 0)
#define DIF(x) strcmp(argv[i], x) == 0
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
    pair<pair<string,string>,bool> dla, dRa, dRl;
    bool dRla = true;
    unordered_map<string, dirent*> filist;
    INITPF(da);
    INITPF(dl);
    INITPF(dR);
    INITPF(dla);
    INITPF(dRa);
    INITPF(dRl);
    da.first = "-a";
    dl.first = "-l";
    dR.first = "-R";
    dla.first.first = "-la";
    dla.first.second = "-al";
    dRa.first.first = "-Ra";
    dRa.first.second = "-aR";
    dRl.first.first = "-Rl";
    dRl.first.second = "-lR";

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
            dR.second = false;
        }
        else if(DMUIF(dla)){
            if(dl.second){
                flg += 2;
                dl.second =false;
            }
            if(da.second){
                flg += 1;
                da.second = false;
            }
        }
        else if(DMUIF(dRa)){
            if(dR.second){
                flg += 4;
                dR.second =false;
            }
            if(da.second){
                flg += 1;
                da.second = false;
            }
        }
        else if(DMUIF(dRl)){
            if(dl.second){
                flg += 2;
                dl.second =false;
            }
            if(dR.second){
                flg += 4;
                dR.second = false;
            }
        }
        else if(dRla && (DIF("-lRa") || DIF("-laR") || DIF("-aRl")
            || DIF("-alR") || DIF("-Ral") || DIF("-Rla"))){
            if(dl.second){
                flg += 2;
                dl.second = false;
            }
            if(da.second){
                flg += 1;
                da.second = false;
            }
            if(dR.second){
                flg += 4;
                dR.second = false;
            }
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
        if(fnsz > 1)
            cout << filnam[j] << ":" << endl;
        char *dirName = filnam[j];
        DIR *dirp = opendir(dirName);
        if(dirp == NULL){
            perror("opendir failed");
            exit(1);
        }

//printing out according to flags
        dirent *direntp;
        vector<string> fnam;
        direntp = readdir(dirp);
        if(!direntp){
            perror("readdir failed");
            exit(1);
        }
        string tempfnam = direntp->d_name;
        filist.emplace(make_pair(tempfnam, direntp));
        fnam.push_back(tempfnam);
        unsigned maxflen = tempfnam.size();
        while ((direntp = readdir(dirp))){
            tempfnam = direntp->d_name;
            filist.emplace(make_pair(tempfnam, direntp));
            fnam.push_back(tempfnam);
            if(maxflen < tempfnam.size())
                maxflen = tempfnam.size();
        }
        sort(fnam.begin(), fnam.end(), locale("en_US.UTF-8"));

        if(flg == 0){
            bool left = true;
            for(auto &e: fnam){
                if(left){
                    if(e.c_str()[0] != '.'){
                        string pout1 = pathctor(e.c_str(), filnam[j]);
                        CTOR_BUF(pout1.c_str());
                        auto fsearch = filist.find(e);
                        printl(fsearch->second, maxflen, &buf);
                        left = false;
                    }
                }
                else if(!left){
                    if(e.c_str()[0] != '.'){
                        string pout2 = pathctor(e.c_str(), filnam[j]);
                        CTOR_BUF(pout2.c_str());
                        auto fsrch = filist.find(e);
                        printr(fsrch->second, &buf);
                        left = true;
                    }
                }
            }
            if(!left)
                cout << endl;
        }
        else if(flg == 1){
            bool left = true;
            for(auto &e: fnam){
                if(left){
                    string pout3 = pathctor(e.c_str(), filnam[j]);
                    CTOR_BUF(pout3.c_str());
                    auto fsearch = filist.find(e);
                    printl(fsearch->second, maxflen, &buf);
                    left = false;
                }
                else if(!left){
                    string pout4 = pathctor(e.c_str(), filnam[j]);
                    CTOR_BUF(pout4.c_str());
                    auto fsrch = filist.find(e);
                    printr(fsrch->second, &buf);
                    left = true;
                }
            }
            if(!left)
                cout << endl;
        }

        else if (flg == 2){
            for(auto &e : fnam){
                if(e.c_str()[0] != '.'){
                    string tempath = pathctor(e.c_str(), filnam[j]);
                    auto fsrch = filist.find(e);
                    lsl(fsrch->second, tempath.c_str());
                }
            }
        }

        else if(flg == 3){
            for(auto &e : fnam){
                auto fsrch = filist.find(e);
                string tempath = pathctor(e.c_str(), filnam[j]);
                lsl(fsrch->second, tempath.c_str());
            }
        }

        else if(flg == 4){
            for(auto &e : fnam){
                if(e.c_str()[0] != '.' || 0 == strcmp(e.c_str(), ".")){
                    auto filsrch = filist.find(e);
                    lsR(filsrch->second, pathctor(e.c_str(),filnam[j]));
                }
            }
        }

        else if(flg == 5){
            for(auto &e : fnam){
                if(strcmp(e.c_str(), "..") != 0){
                    auto filsrch = filist.find(e);
                    lsRa(filsrch->second, pathctor(e.c_str(),filnam[j]));
                }
            }
        }

        else if(flg == 6){
            for(auto &e : fnam){
                if(e.c_str()[0] != '.' || 0 == strcmp(e.c_str(), ".")){
                    auto filsrch = filist.find(e);
                    lsRl(filsrch->second, pathctor(e.c_str(),filnam[j]));
                }
            }
        }

        else if(flg == 7){
            for(auto &e : fnam){
                if(strcmp(e.c_str(), "..") != 0){
                    auto filsrch = filist.find(e);
                    lsRla(filsrch->second, pathctor(e.c_str(),filnam[j]));
                }
            }
        }

        else{
            perror("Flag miscount");
            exit(1);
        }
//closing opened directory
        if(closedir(dirp) == -1){
            perror("closedir failed");
            exit(1);
        }
        if(fnsz > 1)
            cout << endl;
    }
}

