#include "header"
#include "config"
#include "mysqlc"
#include "judger"
#include "init"

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