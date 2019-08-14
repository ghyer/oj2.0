#include "header"
#include "config"
#include "mysqlc"
class Judger {
private:
    int solution_id[1000];
    int problem_id[1000];
    int running_pid[100];
    int cnt;
    int default_memory;
    int default_time;
    int max_running;
    int running_cnt;
public:
    Judger () {
        memset(running_pid, -1, sizeof(running_pid));
        running_cnt = 0;
    }
    void load (Config &conf) {
        default_memory = strToInt(conf.get("DEFAULT_MEMORY"));
        default_time   = strToInt(conf.get("DEFAULT_TIME"));
        max_running    = strToInt(conf.get("MAX_RUNNING"));
    }
    void add (StoreQueryResult &submit) {
        StoreQueryResult::const_iterator it;
        int i = 0;
        Row line;
        for (it = submit.begin(); it != submit.end(); it ++) {
            line = *it;
            solution_id[i] = line["solution_id"];
            problem_id[i] = line["problem_id"];
            i ++;
        }
        cnt = i;
    }
    void judgeCode (Mysqlc &db) {
        int pid;
        pair<int,int> limit(0,0);
        string code;
        for (int i = 0; i < cnt; i ++) {
            // cout << problem_id[i] << endl;
            // cout << solution_id[i] << endl;
            limit = db.getLimit(problem_id[i]);
            // cout << limit.first << endl << limit.second << endl;
            if (limit.first <= 0) {
                limit.first = default_memory;
            }
            if (limit.second <= 0) {
                limit.second = default_time;
            }
            code = db.getCode(solution_id[i]);
            while (running_cnt < max_running) {
                pid = fork();
                if (pid < 0) {
                    continue;
                } else if (pid == 0) {
                    this -> judgeClient(limit.second, limit.first, code);
                } else {
                    running_cnt ++;
                    for (int i = 0; i < max_running; i ++) {
                        if (running_pid[i] == -1) {
                            running_pid[i] = pid;
                        }
                    }
                }
            }
        }
    }
protected:
    void judgeClient (int &time_limit, int &memory_limit, string &code) {
        return ;
    }
};

#include "init"
inline void init() {
    config.load("/etc/judger/config");
    db.load(config);
    judger.load(config);
}

int main(int argc, char **argv) {
    init();
    while(true) {
        submit = db.listen();
        judger.add(submit);
        judger.judgeCode(db);
        sleep(200);
    }
    return 0;
}