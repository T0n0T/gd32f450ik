#include "string.h"
#include "stdint.h"

uint32_t hellobss[4] = {0};
uint32_t hellodata[4] = {0xa, 0xb, 0xc, 0xd};

int main(void)
{
    memcpy(hellobss, hellodata, sizeof(hellobss));
    __asm volatile("svc #1");
    while (1) {
    }
}
