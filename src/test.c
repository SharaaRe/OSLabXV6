#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
// #include "defs.h"
// #include 
// #include "printf.h"

int main(int argc, char* argv[]) {
    int n = 12345;
    int digit = count_num_of_digits(n);
    // write(1, &digit, sizeof(digit));
    // write(1, "\n", 1);

    printf(1, "TEST %d \n", digit);
    exit();
}