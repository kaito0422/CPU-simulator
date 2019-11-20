#ifndef _DECODE_H_
#define _DECODE_H_

#include <list>
#include <vector>
#include "base.h"

class Decoder {
public:
	Decoder() : width(4), capacity(16), decoder_in_work(false){}
	Decoder(int _width, int _capacity) : width(_width), capacity(_capacity) {}
	bool insertBuffer(std::vector<instInfo>);
	std::vector<instInfo> getBuffer();
	bool insertInsts(std::vector<Addr>);

	bool decodeInst(std::vector<Addr> insts, std::vector<instInfo> &inst_info);

	bool isEmpty() { return size == 0; }
	bool isFull() { return size == capacity; }

	/* this function is for pipeline replay */
	void clearDecodeBuffer() {
		decode_buffer.clear();
		size = 0;
		decoder_in_work = false;
		inst_pkt.clear();
	}

	void do_decoder_stage();

private:
	int width;
	int size;
	int capacity;
	std::list<std::vector<instInfo>> decode_buffer;
	std::vector<Addr> inst_pkt;
	bool decoder_in_work;
};

#endif