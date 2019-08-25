#include "header"
int main(int argc, char **argv) {
    init();
    while(true) {
        submit = db.listen();
        judger.add(submit);
        judger.judgeCode(db);
        sleep(2);
    }
    return 0;
}