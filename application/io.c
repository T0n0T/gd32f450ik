#include <stdio.h>
#include "gd32f450i_eval.h"

/* retarget the C library printf function to the USART */
#if defined(__CC_ARM)
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE))
        ;
    return ch;
}
#elif defined(__GNUC__)
int _write(int fd, char *ptr, int len)
{
    int i = 0;

    /*
     * write "len" of char from "ptr" to file id "fd"
     * Return number of char written.
     *
     * Only work for STDOUT, STDIN, and STDERR
     */
    if (fd > 2) {
        return -1;
    }

    while (*ptr && (i < len)) {
        usart_data_transmit(EVAL_COM0, *(uint8_t *)ptr);
        while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE))
            ;
        if (*ptr == '\n') {
            usart_data_transmit(EVAL_COM0, '\r');
            while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE))
                ;
        }

        i++;
        ptr++;
    }

    return i;
}

#endif