#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
// #include "defs.h"
// #include 
// #include "printf.h"

void pass_by_register(int n) {
    int backup;
    asm volatile(
        "movl %%ebx, %0;"
        "movl %1, %%ebx;"
        : "=r" (backup)
        : "r"(n)
    );
    count_num_of_digits();
    asm("movl %0, %%ebx" : : "r"(backup));

}

int main(int argc, char* argv[]) {
    // int n = 12345;
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            printf(1, "TEST for ");
            pass_by_register(atoi(argv[i]));
        }
    }else {
        printf(1, "TEST for ");
        pass_by_register(12345);        
    }
    
    
    exit();

}