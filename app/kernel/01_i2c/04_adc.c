#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define READ_SIZE   2

int main(void)
{
    int i, fd, len, value;
    char buffer[READ_SIZE];

    fd = open("/dev/adc-0", O_RDONLY);
    if (fd < 0) {
        printf("open /dev/adc faild!\r\n");
        exit(1);
    }

    while(1){
        len = read(fd, buffer, READ_SIZE);
        if (len < 0) {
            printf("read adc faild!\r\n");
            exit(1);
        }
        memcpy(&value, buffer, READ_SIZE);
        printf("adc value: %d.\r\n", value);
    }
}
