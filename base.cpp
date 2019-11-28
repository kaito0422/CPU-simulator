#include "base.h"

int log2(int x)
{
    int res = 0;
    while(x - 1) {
        x = x >> 1;
        res++;
    }
    return res;
}

Addr align(Addr addr)
{
    return addr & (~0x3);
}

Addr alignCacheLine(Addr addr)
{
    return addr & (~(LINE_SIZE - 1));
}

const char* range_line = "----------------------------------------------\n";