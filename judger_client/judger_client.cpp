#include "header"
Config   config;
Mysqlc   db;
Client   client;
char buf[1000];

int main (int argc, char **argv) {
    // About the argv:
    // 1 -> solution_id
    // 2 -> running_num

    //Set environment value
    int pid;
    int status;
    int count;
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
    // cout << COMPILE_CMD << endl;

    //Check client dir common or not
    if (!access(PATH.c_str(), F_OK)) {
        if (system(((string)"rm -r " + PATH).c_str())) {
            throwError(REMOVE_ERROR);
        }
    }
    if (chdir(JUDGE_HOME.c_str())) {
        throwError(CHDIR_ERROR);
    }
    if (mkdir(PATH.c_str(), 755)) {
        throwError(MKDIR_ERROR);
    }
    if (mkdir((PATH + "/data").c_str(), 755)) {
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
        timeval end;
        bool kill_flag = true;
        do {
            gettimeofday(&end, NULL);
            if (waitpid(pid, NULL, WNOHANG) == pid) {
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
    // cout << cp_cmd << endl;
    
    //Get datain and dataout
    dirent **in_list;
    dirent **out_list;
    count = scandir((PATH + "/data").c_str(), &in_list, dirSelectIn, alphasort);
    count = scandir((PATH + "/data").c_str(), &out_list, dirSelectOut, alphasort);
    if (count == 0) {
        throwError(DATA_ERROR);
    }
    // cout << count << endl;
    // for (int i = 0; i < count; i ++) {
    //     cout << in_list[i] -> d_name << endl;
    // }
    // for (int i = 0; i < count; i ++) {
    //     cout << out_list[i] -> d_name << endl;
    // }
    
    passwd *judger = getpwnam("judger");
    chownDir(PATH.c_str(), judger->pw_uid, judger->pw_gid);

    //Set limit for running it.
    for (int i = 0; i < count; i ++) {
        pid = fork();
        if (pid < 0) {
            throwError(FORK_ERROR);
        } else if (pid == 0) {
            rlimit limit;
            int memory = client.getMemory();
            int time = client.getTime();
            limit = (rlimit) {memory * 1024 * 1024 * 8, memory * 1024 * 1024 * 8};
            if (setrlimit(RLIMIT_AS, &limit)) {
                throwError(LIMIT_ERROR);
            }

            limit = (rlimit) {time, time};
            if (setrlimit(RLIMIT_CPU, &limit)) {
                throwError(LIMIT_ERROR);
            }

            limit = (rlimit) {0, 0};
            if (setrlimit(RLIMIT_CORE, &limit)) {
                throwError(LIMIT_ERROR);
            }
            if (setrlimit(RLIMIT_NPROC, &limit)) {
                throwError(LIMIT_ERROR);
            }
            
            chdir(PATH.c_str());
            chroot(PATH.c_str());
            setuid(judger->pw_uid);

            FILE *input = NULL;
            FILE *output = NULL;
            input = fopen(("/data/" + (string)(in_list[i] -> d_name)).c_str(), "r");
            output = fopen("ans.out", "w");
            // getcwd(buf, sizeof(buf));
            // cout << buf << endl;
            // cout << "/data/" + (string) (in_list[i] -> d_name) << endl;
            // cout << input << endl;
            if (input == NULL) {
                throwError(FOPEN_ERROR);
            }
            if (dup2(fileno(input), fileno(stdin))) {
                throwError(DUP2_ERROR);
            }
            // if (dup2(fileno(stdout), fileno(output))) {
            //     throwError(DUP2_ERROR);
            // }

            // DIR *dir = opendir("/");
            // cout << errno << endl;
            // cout << (dir == NULL) << endl;
            // exit(0);
            // dirent *list;
            // while ((list = readdir(dir)) != NULL) {
            //     cout << list -> d_name << endl;
            // }
            errno = 0;
            execl("/main", "main", NULL);
            cout << errno << endl;
            throwError(EXEC_ERROR);
            // cout << getuid() << endl;
            
        } else {
            rusage *usage;
            wait4(pid, &status, WSTOPPED, usage);
            // cout << WIFSIGNALED(status) << endl;
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