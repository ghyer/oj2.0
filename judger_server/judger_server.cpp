#include "header"
#include "config"
#include "mysqlc"
#include "judger"
#include "init.h"

int main(int argc, char **argv) {
    init();
    while(true) {
        submit = db.listen();
        judger.add(submit);
        // judger.changeEnvironment();
        judger.judgeCode(db);
         cout << "over" << endl;
        sleep(200);
    }
    return 0;
}