#ifndef _SDRAM_H_
#define _SDRAM_H_

#include <vector>
#include "base.h"

class SDRAM {
public:
	typedef CacheLine CacheLine;

	SDRAM();
	SDRAM(int _size);
	bool write(Addr addr, CacheLine);
	bool read(Addr addr, CacheLine *line);

private:
	std::vector<CacheLine> sdram;
	int size;
	const int latency = 100;
};

#endif
