#ifndef _SDRAM_H_
#define _SDRAM_H_

#include <vector>
#include <string>
#include "base.h"

class SDRAM {
public:
    SDRAM(std::string code_file);
    SDRAM(int _size, std::string code_file);

    CacheLine access(Addr addr);

private:
    int size;
    int line_num;
    std::vector<CacheLine> sdram;

    /* initialize the SDRAM with the code_file */
    void initSDRAM(std::string code_file);
	/* align the cacheline */
	int CacheLineAlign(Addr addr);
};

#endif