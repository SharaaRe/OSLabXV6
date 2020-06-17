#include "types.h"
#include "user.h"

int main(int argc, char **argv) {
    void *null_pointer = NULL;
    int null_pointer_content = *(int*)null_pointer;
    printf(1, "NULL pointer content: 0x%x\n", null_pointer_content);
    printf(1, "0:	8d 4c 24 04     lea ecx, [esp+0x4]\n");
    exit();
}