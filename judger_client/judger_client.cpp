#include "header"
Config   config;
Mysqlc   db;
Client   client;
char buf[1000];
int dirSelectIn (const dirent *dirname) {
    const char *buf = dirname -> d_name;
    int l = strlen(buf);
    if (buf[l-2] == 'i' && buf[l-1] == 'n') {
        return true;
    }
    if (buf[l-2] == 'I' && buf[l-1] == 'N') {
        return true;
    }
    return false;
}
int dirSelectOut (const dirent *dirname) {
    const char *buf = dirname -> d_name;
    int l = strlen(buf);
    if (buf[l-3] == 'a' && buf[l-2] == 'n' && buf[l-1] == 's') {
        return true;
    }
    if (buf[l-3] == 'o' && buf[l-2] == 'u' && buf[l-1] == 't') {
        return true;
    }
    if (buf[l-3] == 'A' && buf[l-2] == 'N' && buf[l-1] == 'S') {
        return true;
    }
    if (buf[l-3] == 'O' && buf[l-2] == 'U' && buf[l-1] == 'T') {
        return true;
    }
    return false;
}
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
    // for (int i = 0; i < count; i ++) {
    //     cout << in_list[i] -> d_name << endl;
    // }
    // for (int i = 0; i < count; i ++) {
    //     cout << out_list[i] -> d_name << endl;
    // }

    //Set limit for running it.
    pid = fork();
    if (pid < 0) {
        throwError(FORK_ERROR);
    } else if (pid == 0) {
        //AS base Byte
        rlimit limit;
        limit = {128*1024*1024*8, 128*1024*1024*8};
        setrlimit(RLIMIT_AS, &limit);

        limit = {0, 0};
        setrlimit(RLIMIT_CORE, &limit);

        limit = {1, 1};
        cout << setrlimit(RLIMIT_CPU, &limit) << endl;

        sleep(2);
        getrlimit(RLIMIT_CPU, &limit);
        cout << limit.rlim_cur << ' ' << limit.rlim_max << endl;

        exit(0);
    } else {
        cout << "-----------------------------------" << endl;
    }

    //Delete dir
    if (system(((string)"rm -r " + PATH).c_str())) {
        throwError(REMOVE_ERROR);
    }

    return 0;
}