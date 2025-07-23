#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include "SEGGER_RTT.h"

#if defined(__GNUC__)
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

    SEGGER_RTT_Write(0, ptr, len);
    return len;
}

__attribute__((weak)) int _close(int file)
{
    return -1;
}

__attribute__((weak)) int _fstat(int file, struct stat *st)
{
    return -1;
}

__attribute__((weak)) int _getpid(void)
{
    return 1;
}

__attribute__((weak)) int _isatty(int fd)
{
    return 0;
}

__attribute__((weak)) int _kill(int pid, int sig)
{
    return -1;
}

__attribute__((weak)) int _lseek(int file, int ptr, int dir)
{
    return 0;
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
    return 0;
}

__attribute__((weak)) caddr_t _sbrk(int incr)
{
    extern char _end; // Linker script defined symbol
    static char *heap_end = &_end;
    char *prev_heap_end   = heap_end;
    // 简单堆增长（需确保内存安全）
    heap_end += incr;
    return (caddr_t)prev_heap_end;
}
#endif