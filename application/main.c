#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "stdarg.h"
#include "platform/svc/svc.h"

uint32_t hellobss[4]  = {0};
uint32_t hellodata[4] = {0xa, 0xb, 0xc, 0xd};

SVCALL(1, int, hello_svc(uint32_t arg0, uint32_t arg1, uint32_t arg3));
SVCALL(2, int, average_svc(uint32_t count, ...));

int main(void)
{
    memcpy(hellobss, hellodata, sizeof(hellobss));
    printf("svc_1 return: %lu\r\n", hello_svc(1, 2, 3));
    printf("svc_2 return: %x\r\n", average_svc(3, 1, 2, 3));

    while (1) {
    }
}
