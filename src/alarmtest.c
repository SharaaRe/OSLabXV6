#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char* argv[]) {
    set_alarm(500);
    for (int i = 0; i < 100000000000; i++)
    {
        if((i % 2500000) == 0)
            write(2, ".", 1);
        if (i == 20 * 2500000)
            set_alarm(1000);
    
    }
    exit();
}