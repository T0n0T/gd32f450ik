#include "gd32f4xx.h"
#include "gd32f450i_eval.h"
#include <stdio.h>
#include <string.h>

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"

usb_core_driver cdc_acm;

void main(void)
{
    printf(" ____ ____ _________  _____ _  _  ____   ___  _ \n");
    printf(" / ___|  _ \\___ /___ \\|  ___| || || ___| / _ \\| |\n");
    printf("| |  _| | | ||_ \\ __) | |_  | || ||___ \\| | | | |\n");
    printf("| |_| | |_| |__) / __/|  _| |__   _|__) | |_| |_|\n");
    printf(" \\____|____/____/_____|_|      |_||____/ \\___/(_)\n");

    usb_gpio_config();
    usb_rcu_config();
    usb_timer_init();

    usbd_init(&cdc_acm,
#ifdef USE_USB_FS
              USB_CORE_ENUM_FS,
#elif defined(USE_USB_HS)
              USB_CORE_ENUM_HS,
#endif /* USE_USB_FS */
              &cdc_desc,
              &cdc_class);

    usb_intr_config();

    /* main loop */
    static uint32_t tick = 0;
    const char *test_string = "Hello from GD32 USB CDC! This is a test message.\r\n";
    
    while (1) {
        if (USBD_CONFIGURED == cdc_acm.dev.cur_status) {
            if (0U == cdc_acm_check_ready(&cdc_acm)) {
                cdc_acm_data_receive(&cdc_acm);
            } else {
                /* 周期性发送测试字符串 */
                if (tick++ % 1000000 == 0) {
                    usb_cdc_handler *cdc = (usb_cdc_handler *)cdc_acm.dev.class_data[0];
                    if (cdc != NULL) {
                        strcpy((char *)cdc->data, test_string);
                        cdc->receive_length = strlen(test_string);
                        cdc_acm_data_send(&cdc_acm);
                    }
                } else {
                    cdc_acm_data_send(&cdc_acm);
                }
            }
        }
    }
}
