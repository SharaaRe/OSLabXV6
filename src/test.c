#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
// #include "defs.h"
// #include 
// #include "printf.h"

int main(int argc, char* argv[]) {
    int n = 12345;
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            printf(1, "TEST for ");
            count_num_of_digits(atoi(argv[i]));
        }
    }else {
        printf(1, "TEST for ");
        count_num_of_digits(n);        
    }
    
    
    exit();

}