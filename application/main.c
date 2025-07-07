#include "string.h"
#include "stdint.h"
#include "stdio.h"

uint32_t hellobss[4]  = {0};
uint32_t hellodata[4] = {0xa, 0xb, 0xc, 0xd};

int main(void)
{
    memcpy(hellobss, hellodata, sizeof(hellobss));
    __asm volatile("svc #1");

    int x, y, z;
    x = 10;
    y = 0;
    z = x / y;
    printf("z = %d \r\n", z);

    while (1) {
    }
}
