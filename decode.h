#ifndef _DECODE_H_
#define _DECODE_H_

#include <iostream>
#include <vector>
#include <list>
#include "base.h"

class Decoder {
public:
	void setWidth(int _n) { width = _n; }

	/* decode insts (width) */
	std::vector<InstInfo> decodeInsts(std::vector<Addr> insts, Cycle *lat);

private:
	Cycle latency = 1;
	int width;
};

class DecodeBuffer {
public:
	DecodeBuffer();
	DecodeBuffer(int _capacity, int _width);

	void setWidth(int _n) { width = _n; }

	int getWidth() { return width; }
	bool isEmpty() { return size == 0; }
	bool isFull() { return size == capacity; }

	/* insert decoded insts info to the decode_buffer */
	bool insertInfo(std::vector<InstInfo>& info);

	/* get insts info from the decode buffer
	 * you have to check  if decode buffer is empty before call this function
	 * */
	std::vector<InstInfo> getInstInfo();

private:
	int size = 0;
	int capacity;
	int width;
	std::list<std::vector<InstInfo>> decode_buffer;

};

class DecodeStage {
public:
	/* set both widths */
	DecodeStage();
	DecodeStage(int _width);

	/* have to check if the width is out of range */

	/* get insts from fetch stage */
	bool insertInsts(std::vector<Addr> insts);

	/* do the decode stage */
	void do_decode_stage();

	/* check if the decode stage is busy */
	bool isBusy() { return worked || isFull(); }

	/* get insts info */
	std::vector<InstInfo> getInstInfo();

	/* the state of the decode buffer */
	bool isEmpty() { return decode_buffer.isEmpty(); }
	bool isFull() { return decode_buffer.isFull(); }

private:
	Decoder decoder;
	DecodeBuffer decode_buffer;
	bool worked = false;
	std::vector<Addr> insts_in;
};

#endif