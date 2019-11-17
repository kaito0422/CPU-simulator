#ifndef _FETCH_H_
#define _FETCH_H_

#include <vector>
#include <list>
#include "base.h"

class ICache {
public:
    typedef std::list<CacheLine> CacheSet;

    ICache() : setNum(64), assoc(4), cachelineSize(64) {
        initICache();
    }
	ICache(int _setNum, int _assoc, int _cachelineSize);

	/* public functions */

	void initICache();
    void access(Addr addr, Data *data_res, Cycle *latency);
	int getMissNum() { return miss_num; }

private:
    int setNum;
    int assoc;
    int cachelineSize;
    int size;           /* cache size (bytes) */

    int tagMask, tagBits;
    int indexMask, indexBits;
    int offsetMask, offsetBits;

    std::vector<CacheSet> cache;

	const int hit_latency = 10;
    int miss_num;

	/* private functions */

	CacheSet::iterator findVictim(int index);
};

class FetchBuffer {
public:
	FetchBuffer() : size(0), width(4), capacity(16) {}
	FetchBuffer(int _capacity, int _width) : size(0), width(_width), capacity(_capacity) {}
	bool isEmpty() { return size == 0; }
	bool isFull() { return size == capacity; }
	bool insert(std::vector<Addr> insts);
	std::vector<Addr> get();
	int getWidth() { return width; }
	int getSize() { return size; }
	void clearBuffer();

private:
	std::list<std::vector<Addr>> buffer;
	int size;
	int capacity;
	int width;

};

class FetchStage {
public:
	FetchStage() : pc(0), stalled(false) {}
	void do_fetch_stage();
	std::vector<Addr> getInsts();
	int getPC() { return pc; }
	int getCacheMissNum() {	return icache.getMissNum();	}
	/* setPC is for target PC of JMP from exectution stage */
	void setPC(Addr new_pc) { pc = new_pc; }	
	/* this function is for pipeline replay */
	void clearFetchBuffer() { 
		fetch_buffer.clearBuffer(); 
		stalled = false;
	}

private:
	ICache icache;
	FetchBuffer fetch_buffer;

	Addr pc;
	bool stalled;
};

#endif