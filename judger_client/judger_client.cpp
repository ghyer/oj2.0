#include "header"

int main (int argc, char **argv) {
    // About the argv:
    // 1 -> solution_id
    // 2 -> running_num
    init( strToInt( (string) argv[1] ), (string) argv[2] );
    
    return 0;
}