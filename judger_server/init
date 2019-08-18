Config              config;
Mysqlc              db;
StoreQueryResult    submit;
Judger              judger;

inline void init() {
    //Create a background process
    // int pid = fork();
    // if (pid > 0) {
    //     exit(EXIT_SUCCESS);
    // } else if (pid < 0) {
    //     perror("Fail to running judge_server!");
    //     exit(EXIT_FAILURE);
    // } else {
    //     setsid();
    //     umask(0);
    // }

    config.load("/home/judger/config");
    db.load(config);
    judger.load(config);
}