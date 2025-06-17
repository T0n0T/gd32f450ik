#include <stdio.h>
#include "gd32f450i_eval.h"
#include "uart.h"

UINT8 UartGetc(void)
{
    return (UINT8)usart_data_receive(EVAL_COM0);
}

/* retarget the C library printf function to the USART */
#if defined(__CC_ARM)
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
#elif defined(__GNUC__)
ssize_t _write(int fd, char *ptr, int len)
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
        while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
        if (*ptr == '\r') {
            usart_data_transmit(EVAL_COM0, '\n');
            while (RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
        }

        i++;
        ptr++;
    }

    return i;
}

int _open(const char *path, int oflag, ...)
{
    return -1;
}

int _close(int fd)
{
    return -1;
}

ssize_t _read(int fd, void *buf, size_t nbyte)
{
    return -1;
}

off_t _lseek(int fd, off_t offset, int whence)
{
    return -1;
}

int _unlink(const char *path)
{
    return -1;
}

int _fstat(int fd, struct stat *buf)
{
    return -1;
}

int _stat(const char *path, struct stat *buf)
{
    return -1;
}

int _sbrk(int incr)
{
    return -1;
}

#endif
