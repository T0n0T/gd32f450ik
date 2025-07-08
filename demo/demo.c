#include "string.h"
#include "platform/svc/svc.h"

SVCALL(1, int, hello_svc(uint32_t arg0, uint32_t arg1, uint32_t arg2));

int main(void)
{
    while (1) {
        hello_svc(1, 2, 3);
    }
}
