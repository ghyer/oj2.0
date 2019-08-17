#include <iostream>
#include <unistd.h>
#include <cstring>
#include <linux/seccomp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstring>
using namespace std;
const string ROOT_DIR = "/home/judger";
const char *COMPILE_CMD = "g++ -o main main.cpp -w";
// const char *COMPILE_CMD = "cat /etc/passwd";
int main(int argc, char **argv) {
    // About the argv:
    // 1 -> solution_id
    // 2 -> problem_id
    // 3 -> time_limit
    // 4 -> memory_limit
    // 5 -> running_num
    // 6 -> user_code

    // cout << "Judging solution_id=" << argv[1] << " problem_id=" << argv[2] << " time_limit=" << argv[3] << "s memory_limit=" << argv[4] << "MB" << endl;
    // cout << PATH << endl;
    const string PATH = ROOT_DIR + "/client/client" + argv[5];
    const string CODE_FILE = PATH + "/main.cpp";
    const string RUNNING_FILE = PATH + "/main";

    chdir(ROOT_DIR.c_str());
    mkdir(PATH.c_str(), 755);

    ofstream code;
    code.open(CODE_FILE, ios::out);
    code << argv[6];
    code.close();

    chdir(PATH.c_str());
    FILE *fp = NULL;
    fp = popen(COMPILE_CMD, "r");
    if(fp == NULL) {
        cout << "fail" << endl;
    }
    char buf[1000];
    fgets( buf, sizeof(buf), fp);
    memset(buf,'\0',sizeof(buf));
    // fgets(buf,sizeof(buf),fp);
    cout << "buf=" << buf ;
    // char ch;
    // while( (ch=fgetc(fp)) != EOF ){
    //     putchar(ch);
    // }
    cout << "-------------------------" << endl;
    pclose(fp);

    remove(RUNNING_FILE.c_str());
    remove(CODE_FILE.c_str());
    rmdir(PATH.c_str());

    return 0;
}