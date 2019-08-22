#include "header"
Config   config;
Mysqlc   db;
Client   client;
char buf[1000];

void chrootPre () {
    mkdir("lib64", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    mkdir("lib", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    mkdir("lib/x86_64-linux-gnu", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    mkdir("bin", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    mkdir("usr", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    mkdir("usr/lib", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    mkdir("usr/lib/x86_64-linux-gnu", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    system("cp /lib/x86_64-linux-gnu/libdl.so.2 ./lib/x86_64-linux-gnu");
    system("cp /lib/x86_64-linux-gnu/libc.so.6 ./lib/x86_64-linux-gnu");
    system("cp /lib/x86_64-linux-gnu/libgcc_s.so.1 ./lib/x86_64-linux-gnu");
    system("cp /lib/x86_64-linux-gnu/libtinfo.so.5 ./lib/x86_64-linux-gnu");
    system("cp /lib/x86_64-linux-gnu/libm.so.6 ./lib/x86_64-linux-gnu");
    system("cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6 ./usr/lib/x86_64-linux-gnu");
    system("cp /lib64/ld-linux-x86-64.so.2 ./lib64");
    system("cp /bin/bash bin");
}

int diffTwoFile (const char *file1, const char *file2) {
    int l1;
    int l2;
    string ans = " ";
    string test = " ";
    ifstream fp;
    fp.open(file1, ios::in);
    while (!fp.eof()) {
        ans += fp.get();
    }
    fp.close();
    l1 = ans.length();
    for (int i = 0; ans[i] != '\0'; i ++) {
        if ((ans[i] < '0' || ans[i] > '9') && (ans[i] < 'a' || ans[i] > 'z') && (ans[i] < 'A' || ans[i] > 'Z')) {
            ans.erase(i, 1);
            i --;
            continue;
        }
        break;
    }
    l1 = ans.length();
    for (int i = l1-1; i >= 0; i --) {
        if ((ans[i] < '0' || ans[i] > '9') && (ans[i] < 'a' || ans[i] > 'z') && (ans[i] < 'A' || ans[i] > 'Z')) {
            ans.erase(i, 1);
            continue;
        }
        break;
    }
    
    fp.open(file2, ios::in);
    while (!fp.eof()) {
        test += fp.get();
    }
    fp.close();
    l2 = test.length();
    for (int i = 0; test[i] != '\0'; i ++) {
        if ((test[i] < '0' || test[i] > '9') && (test[i] < 'a' || test[i] > 'z') && (test[i] < 'A' || test[i] > 'Z')) {
            test.erase(i, 1);
            i --;
            continue;
        }
        break;
    }
    l2 = test.length();
    for (int i = l2-1; i >= 0; i --) {
        if ((test[i] < '0' || test[i] > '9') && (test[i] < 'a' || test[i] > 'z') && (test[i] < 'A' || test[i] > 'Z')) {
            test.erase(i, 1);
            continue;
        }
        break;
    }
    if (ans == test) {
        return AC;
    }
    l1 = ans.length();
    l2 = test.length();
    cout << ans << endl;
    cout << test << endl;
    for (int i = 0; i < l1; i ++) {
        if ((ans[i] < '0' || ans[i] > '9') && (ans[i] < 'a' || ans[i] > 'z') && (ans[i] < 'A' || ans[i] > 'Z')) {
            ans.erase(i, 1);
            i --;
        }
    }
    for (int i = 0; i < l2; i ++) {
        if ((test[i] < '0' || test[i] > '9') && (test[i] < 'a' || test[i] > 'z') && (test[i] < 'A' || test[i] > 'Z')) {
            test.erase(i, 1);
            i --;
        }
    }
    if (test == ans) {
        return PE;
    }
    return WA;
}

int main (int argc, char **argv) {
    // About the argv:
    // 1 -> solution_id
    // 2 -> running_num

    //Set environment value
    int pid;
    int status;
    int sid = strToInt(argv[1]);
    int num = strToInt(argv[2]);
    config.load("/home/judger/config");
    db.load(config);
    client.load(sid, config, db);
    int compile_time = strToInt(config.get("COMPILE_TIME"));
    
    const string JUDGE_HOME     = (string)config.get("JUDGE_HOME");
    const string COMPILE_CMD    = (string)config.get("COMPILE_CMD") + " -fmax-errors=" + (string)config.get("COMPILE_ERROR_OUT") + " 2>&1";
    const string PATH           = JUDGE_HOME + "/client/client" + intToStr(num);
    const string CODE_FILE      = PATH + "/main.cpp";
    const string RUNNING_FILE   = PATH + "/main";
    const string DATA_HOME      = (string)config.get("DATA_HOME");

    //Check client dir common or not
    if (!access(PATH.c_str(), F_OK)) {
        if (system(((string)"rm -r " + PATH).c_str())) {
            throwError(REMOVE_ERROR);
        }
    }
    if (chdir(JUDGE_HOME.c_str())) {
        throwError(CHDIR_ERROR);
    }
    if (mkdir(PATH.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
        throwError(MKDIR_ERROR);
    }
    if (mkdir((PATH + "/data").c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)) {
        throwError(MKDIR_ERROR);
    }
    
    //Write main.cpp with user's code
    ofstream code;
    code.open(CODE_FILE.c_str(), ios::out);
    code << client.getCode();
    code.close();

    //Compile the cpp file.
    timeval start;
    gettimeofday(&start, NULL);
    pid = fork();
    if (pid < 0) {
        throwError(FORK_ERROR);
    } else if (pid == 0) {
        if (chdir(PATH.c_str())) {
            throwError(CHDIR_ERROR);
        }
        FILE *fp = NULL;
        fp = popen(COMPILE_CMD.c_str(), "r");
        if (fp == NULL) {
            throwError(POPEN_ERROR);
        } else {
            if (fgets( buf, sizeof(buf), fp) != NULL) {
                db.updateCompile(sid, fp, buf);
                throwError(COMPILE_ERROR);
            }
            pclose(fp);
            exit(0);
        }
    } else {
        int stat = 0;
        timeval end;
        bool kill_flag = true;
        do {
            gettimeofday(&end, NULL);
            if (waitpid(pid, &stat, WNOHANG) == pid) {
                if (stat) {
                    exit(0);
                }
                kill_flag = false;
                break;
            }
        } while (end.tv_sec - start.tv_sec <= compile_time);
        if (kill_flag) {
            kill(pid, SIGKILL);
            db.updateCompile(sid, NULL ,"Compile timeout!");
        }
    }

    //Copy data to client
    string cp_cmd = "cp " + DATA_HOME + "/" + intToStr(client.getProblemId()) + "/* " + PATH + "/data";
    status = system(cp_cmd.c_str());
    if (-1 == status) {
        throwError(COPY_FAIL);
    } else {
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status)) {
                throwError(COPY_FAIL);
            }
        } else {
            throwError(COPY_FAIL);
        }
    }
    
    //Get datain and dataout.
    int count1, count2;
    dirent **in_list;
    dirent **out_list;
    count1 = scandir((PATH + "/data").c_str(), &in_list, dirSelectIn, alphasort);
    if (count1 == 0) {
        throwError(DATA_ERROR);
    }
    count2 = scandir((PATH + "/data").c_str(), &out_list, dirSelectOut, alphasort);
    if (count2 == 0) {
        throwError(DATA_ERROR);
    }
    if (count1 != count2) {
        throwError(DATA_ERROR);
    }
    
    //Change running environment.
    passwd *judger = getpwnam("judger");
    chdir(PATH.c_str());
    chrootPre();
    chownDir(PATH.c_str(), judger->pw_uid, judger->pw_gid);
    chroot(PATH.c_str());

    // signal(SIGCHLD, handle);

    //Set limit for running it.
    for (int i = 0; i < count1; i ++) {
        pid = fork();
        if (pid < 0) {
            throwError(FORK_ERROR);
        } else if (pid == 0) {
            // cout << "I am in" << endl;
            int memory = client.getMemory();
            int time = client.getTime();
            
            rlimit mlimit;
            mlimit = (rlimit) {memory * 1024 * 1024 * 2, memory * 1024 * 1024 * 2};
            if (setrlimit(RLIMIT_AS, &mlimit)) {
                throwError(LIMIT_ERROR);
            }
            mlimit = (rlimit) {memory * 1024 * 1024 * 2, memory * 1024 * 1024 * 2};
            if (setrlimit(RLIMIT_DATA, &mlimit)) {
                throwError(LIMIT_ERROR);
            }

            rlimit tlimit;
            tlimit = (rlimit) {time, time + 1};
            if (setrlimit(RLIMIT_CPU, &tlimit)) {
                throwError(LIMIT_ERROR);
            }

            rlimit climit;
            climit = (rlimit) {0, 0};
            if (setrlimit(RLIMIT_CORE, &climit)) {
                throwError(LIMIT_ERROR);
            }
            
            rlimit slimit;
            slimit = (rlimit) { 20 * 1024 * 1024, 20 * 1024 * 1024};
            if (setrlimit(RLIMIT_STACK, &slimit)) {
                throwError(LIMIT_ERROR);
            }

            //Set gid must before uid
            setgid(judger->pw_gid);
            setuid(judger->pw_uid);

            FILE *input = NULL;
            FILE *output = NULL;
            input = freopen(("/data/" + (string)(in_list[i] -> d_name)).c_str(), "r", stdin);
            output = freopen("ans.out", "w", stdout);
            if (input == NULL || output == NULL) {
                throwError(FREOPEN_ERROR);
            }

            // errno = 0;
            execl("/main", "main", NULL);
            close(fileno(input));
            close(fileno(output));
            // cout << errno << endl;
            throwError(EXEC_ERROR);
        } else {
            rusage usage;
            int stat;
            int dieid;
            int time_used;
            int memory_used;
            bool diff = false;
            while (true) {
                dieid = wait4(pid, &stat, WUNTRACED, &usage);
                // wait4 exit error
                if (dieid == -1) {
                    // Check errno.
                    throwError(WAIT4_ERROR);
                }
                // cout << client.getMemory() << endl;
                // Child process exit normally.
                if (WIFEXITED(stat)) {
                    time_used = usage.ru_utime.tv_sec  * 1000 
                          + usage.ru_utime.tv_usec / 1000
                          + usage.ru_stime.tv_sec  * 1000
                          + usage.ru_stime.tv_usec / 1000;
                    memory_used = usage.ru_maxrss / 1024;
                    cout << time_used << "ms" << endl;
                    cout << memory_used << "MB" << endl;
                    if (memory_used > client.getMemory()) {
                        cout << "MLE" << endl;
                        break;
                    }
                    diff = true;
                    // cout << "AC" << endl;
                    break;
                }
                if (WIFSIGNALED(stat)) {
                    ptrace(PTRACE_KILL, pid, NULL, NULL);
                    time_used = usage.ru_utime.tv_sec  * 1000 
                          + usage.ru_utime.tv_usec / 1000
                          + usage.ru_stime.tv_sec  * 1000
                          + usage.ru_stime.tv_usec / 1000;
                    memory_used = usage.ru_maxrss / 1024;
                    cout << time_used << "ms" << endl;
                    cout << memory_used << "MB" << endl;
                    status = WTERMSIG(stat);
                    if (status == SIGALRM || status == SIGXCPU) {
                        cout << "TLE" << endl;
                        break;
                    } else if (status == SIGSEGV) {
                        cout << "MLE1" << endl;
                        break;
                    } else {
                        cout << "RE" << endl;
                        break;
                    }
                }
                //continue pid process
                ptrace(PTRACE_CONT, pid, NULL, NULL);
            }
            if (diff) {
                string test = (string)"/data/" + (string)(out_list[i] -> d_name);
                status = diffTwoFile("ans.out", test.c_str());
                if (status == AC) {
                    cout << "AC" << endl;
                } else if (status == WA) {
                    cout << "WA" << endl;
                } else if (status == PE) {
                    cout << "PE" << endl;
                }
            }
        }
    }
    wait(NULL);
    exit(0);

    //Delete dir
    if (system(((string)"rm -r " + PATH).c_str())) {
        throwError(REMOVE_ERROR);
    }

    return 0;
}