#include "sdram.h"
#include "base.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

/* initialize the SDRAM with the code_file */
void SDRAM::initSDRAM(std::string code_file)
{
    std::ifstream file(code_file);
    std::string str_line;
    while(std::getline(file, str_line)) {
        std::istringstream ss1(str_line);
    
        unsigned long data_tmp;
        ss1 >> data_tmp;
        Data d1, d2, d3, d4;
        d1 = data_tmp & 0xFF;
        d2 = (data_tmp & 0xFF00) >> 8;
        d3 = (data_tmp & 0xFF0000) >> 16;
        d4 = (data_tmp & 0xFF000000) >> 24;

        int i = 0;

        CacheLine line_tmp;
        line_tmp.data[i++] = d1;
        line_tmp.data[i++] = d2;
        line_tmp.data[i++] = d3;
        line_tmp.data[i++] = d4;

        /* the compiler has to make the number of lines of
         * the binary code can be divided by 16
         *  */
        for(int j = 0; j < 15; j++) {
            std::getline(file, str_line);
            std::istringstream ss(str_line);
            ss >> data_tmp;

            d1 = data_tmp & 0xFF;
            d2 = (data_tmp & 0xFF00) >> 8;
            d3 = (data_tmp & 0xFF0000) >> 16;
            d4 = (data_tmp & 0xFF000000) >> 24;

            line_tmp.data[i++] = d1;
            line_tmp.data[i++] = d2;
            line_tmp.data[i++] = d3;
            line_tmp.data[i++] = d4;
        }
        
        line_tmp.invalid = false;
        line_tmp.tag = 0;           /* tag is not needed in SDRAM  */
        sdram.push_back(line_tmp);
    }

    file.close();
}

SDRAM::SDRAM(std::string code_file) : size(512*1024*1024), \
                                    line_num(512*1024*1024/64)
{
    initSDRAM(code_file);
}

SDRAM::SDRAM(int _size, std::string code_file) : \
                                size(_size), line_num(_size / 64)
{
    initSDRAM(code_file);
}

int SDRAM::CacheLineAlign(Addr addr)
{
    return (addr >> 6) & (line_num - 1);
}

CacheLine SDRAM::access(Addr addr)
{
    int index = CacheLineAlign(addr);
	/* need to check if the addr is legal (not out of range) */
    return sdram[index];
}