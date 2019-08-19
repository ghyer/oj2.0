#include "header"
Config   config;
Mysqlc   db;
Client   client;

char buf[1000];
int main (int argc, char **argv) {
    // About the argv:
    // 1 -> solution_id
    // 2 -> running_num
    int pid;
    int sid = strToInt(argv[1]);
    int num = strToInt(argv[2]);

    config.load("/home/judger/config");
    db.load(config);
    client.load(sid, config, db);

    const string JUDGE_HOME     = (string)config.get("JUDGE_HOME");
    const string COMPILE_CMD    = (string)config.get("COMPILE_CMD") + " -fmax-errors=" + (string)config.get("COMPILE_ERROR_OUT") + " 2>&1";
    // cout << COMPILE_CMD << endl;
    const string PATH           = JUDGE_HOME + "/client/client" + intToStr(num);
    const string CODE_FILE      = PATH + "/main.cpp";
    const string RUNNING_FILE   = PATH + "/main";

    if (!chdir(JUDGE_HOME.c_str())) {
        throwError(CHDIR_ERROR);
    }
    if (!mkdir(PATH.c_str(), 755)) {
        throwError(MKDIR_ERROR);
    }

    ofstream code;
    code.open(CODE_FILE, ios::out);
    code << client.getCode();
    code.close();

    //Popen() doesn't work at g++ so turn to use pipe()
    //g++ use stderr but not stdout
    pid = fork();
    if (pid < 0) {
        throwError(FORK_ERROR);
    } else if (pid == 0) {
        if (!chdir(PATH.c_str())) {
            throwError(CHDIR_ERROR);
        }
        FILE *fp = NULL;
        fp = popen(COMPILE_CMD.c_str(), "r");
        if (fp == NULL) {
            throwError(POPEN_ERROR);
        } else {
            if (fgets( buf, sizeof(buf), fp) != NULL) {
                db.updateCompile(sid, fp);
                throwError(COMPILE_ERROR);
            }
            pclose(fp);
        }
    } else {
        wait(NULL);
        exit(0);
    }
    

    pid = fork();
    if (pid < 0) {
        exit(1);
    } else if (pid == 0) {
        //modify
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

    remove(RUNNING_FILE.c_str());
    remove(CODE_FILE.c_str());
    rmdir(PATH.c_str());
    
    return 0;
}