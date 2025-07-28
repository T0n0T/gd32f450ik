#include "gd32f4xx.h"
#include "gd32f450i_eval.h"
#include <stdio.h>
#include <string.h>

void main(void)
{
    printf(" ____ ____ _________  _____ _  _  ____   ___  _ \n");
    printf(" / ___|  _ \\___ /___ \\|  ___| || || ___| / _ \\| |\n");
    printf("| |  _| | | ||_ \\ __) | |_  | || ||___ \\| | | | |\n");
    printf("| |_| | |_| |__) / __/|  _| |__   _|__) | |_| |_|\n");
    printf(" \\____|____/____/_____|_|      |_||____/ \\___/(_)\n");

    extern void cdc_acm_init(uint8_t busid, uint32_t reg_base);
    cdc_acm_init(0, USBFS_BASE);

    while (1) {
        extern void cdc_acm_data_send_with_dtr_test(uint8_t busid);
        cdc_acm_data_send_with_dtr_test(0);
    }
}
