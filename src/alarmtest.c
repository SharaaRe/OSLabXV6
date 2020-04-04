#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void loop_n_bilion(int n) {
    for (int i = 0; i <  n * 100000000; i++)
        if (i % 2500000 == 0)
            write(2, ".", 1);

}

int main(int argc, char* argv[]) {
    printf(1, "\nFirst scenario, should get alarm after approximately 5 seconds\n");
    set_alarm(5000);
    loop_n_bilion(10);
    printf(1, "\nSecond scenario, should get alarm after approximately 1 seconds instead of 5\n");
    set_alarm(5000);
    set_alarm(1000);
    loop_n_bilion(5);

    set_alarm(1000);
    printf(1, "\nThird scenario, should get error and alarm is not changed so it gets alarm after 1 second\n");
    set_alarm(-1);
    loop_n_bilion(5);



    exit();
}