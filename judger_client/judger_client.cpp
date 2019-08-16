#include <iostream>
#include <unistd.h>
#include <cstring>
#include <linux/seccomp.h>
using namespace std;
int main(int argc, char **argv) {
    cout << getlogin() << endl;
    // cout << getuid() << endl;
    // cout << getgid() << endl;
    sleep(3);
    return 0;
}