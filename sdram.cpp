#include "sdram.h"

SDRAM::SDRAM()
{
	size = 512 * 1024 * 16;
	for (int i = 0; i < size; i++) {
		CacheLine tmp_line;
		tmp_line.tag = 0x00;
		for (int j = 0; j < LINE_SIZE; j++)
			tmp_line.data[j] = 0;
		sdram.push_back(tmp_line);
	}
}

SDRAM::SDRAM(int _size) : size(_size) {
	for (int i = 0; i < _size; i++) {
		CacheLine tmp_line;
		tmp_line.tag = 0x00;
		for (int j = 0; j < LINE_SIZE; j++)
			tmp_line.data[j] = 0;
		sdram.push_back(tmp_line);
	}
}

bool SDRAM::write(Addr addr, CacheLine line)
{
	int index = LINE_ALIGNED(addr);
	if (index >= size * LINE_SIZE)
		return false;
	sdram[index] = line;
	return true;
}

bool SDRAM::read(Addr addr, CacheLine *line)
{
	int index = LINE_ALIGNED(addr);
	if (index >= size * LINE_SIZE)
		return false;
	*line = sdram[index];
	return true;
}