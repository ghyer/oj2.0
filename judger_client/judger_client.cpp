#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <cstring>
using namespace std;
int main(int argc, char **argv) {
    cout << getlogin() << endl;
    cout << chroot("/home/judger/judge") << endl;
    char buf[100];
    getcwd(buf, sizeof(buf));
    cout << buf << endl;
    return 0;
}