#include "header"
#include "work"
#include "run"

int main (int argc, char **argv) {
    // About the argv:
    // 1 -> solution_id
    // 2 -> running_num
    Config   config;
    Mysqlc   db;
    Client   client;

    //Set environment value
    int cnt;
    int sid = strToInt(argv[1]);
    int num = strToInt(argv[2]);
    passwd *ju;
    dirent **in_list;
    dirent **out_list;

    init(sid, db, config, client);
    
    const string DATA_HOME      = (string)config.get("DATA_HOME");
    const string JUDGE_HOME     = (string)config.get("JUDGE_HOME");
    const string COMPILE_CMD    = (string)config.get("COMPILE_CMD") + " -fmax-errors=" + (string)config.get("COMPILE_ERROR_OUT") + " 2>&1";
    
    const string PATH           = JUDGE_HOME + "/client/client" + intToStr(num);
    const string CODE_FILE      = PATH + "/main.cpp";
    const string RUNNING_FILE   = PATH + "/main";
    const string CP_CMD         = "cp " + DATA_HOME + "/" + intToStr(client.getProblemId()) + "/* " + PATH + "/data";

    makeDir(PATH.c_str(), JUDGE_HOME.c_str(), (PATH+"/data").c_str(), ("rm -r "+ PATH).c_str());
    writeCpp(CODE_FILE.c_str(), client);
    compileCpp(PATH.c_str(), COMPILE_CMD.c_str(), config, db, sid);
    copyData(CP_CMD.c_str());
    getData((PATH+"/data").c_str(), in_list, out_list, cnt);
    changeEnvironment(PATH.c_str(), ju);
    run(db, config, client, in_list, out_list, ju, sid, cnt);

    cout << "solution " << sid << "  problem " << client.getProblemId() << ' ' << client.getMaxTime() << "ms " << client.getMaxMemory() << "MB" << " score: " << client.getScore(cnt) << endl;
    // Delete path

    return 0;
}