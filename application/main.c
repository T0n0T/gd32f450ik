#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdarg.h"
#include "platform/svc/svc.h"

uint32_t hellobss[4]  = {0};
uint32_t hellodata[4] = {0xa, 0xb, 0xc, 0xd};

SVCALL(1, int, hello_svc(uint32_t arg0, uint32_t arg1, uint32_t arg2));

int main(void)
{
    memcpy(hellobss, hellodata, sizeof(hellobss));
    printf("svc_1 return: %lu\r\n", hello_svc(1, 2, 3));
    while (1) {
    }
}
