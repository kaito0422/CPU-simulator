#ifndef _FETCH_H_
#define _FETCH_H_

#include <vector>
#include <list>
#include "base.h"

class ICache {
public:
	typedef int Cycle;


    ICache();
    ICache(int _setNum, int _assoc, Cycle _latency);


    /* access the cache with addr, *inst, inst_num and *latency
     * addr: where to get the insts
     * &inst_vec: store the insts after accessing ICache
     * inst_num: how many insts to fetch
     * *latency: the accessing latency
     *  */
    void access(Addr pc, std::vector<Addr> &inst_vec, int inst_num, Cycle *lat);


	/* get the ICache miss number */
	int getMissNum() { return miss_num; }

private:
    /* the parameters of the ICache */
    int size;       /* byte-based size */
    int assoc;
    const int line_size = LINE_SIZE;
    int setNum;
    Cycle latency;  /* the latency when cache hit */

    int miss_num = 0;

    /* all cache sets are orgnized as vector;
     * in one cache set, all cachelines are orgnized as list
     *  */
    std::vector<std::list<CacheLine>> cache;

    /* init cache configuration */
    void initCacheConfig();

    /* to extract the parameters of the pc address */
    Addr tagMask, tagShift;
    int indexMask, indexShift;
    Addr getTag(Addr pc);
    int getIndex(Addr pc);
    int getOffset(Addr pc);

    /* find a victim by using the replacement policy */
    std::list<CacheLine>::iterator findVictim(int index);

    /* access one cacheline */
    CacheLine accessCacheLine(Addr pc, Cycle *lat);
};

class FetchBuffer {
public:
	FetchBuffer();
	FetchBuffer(int _capacity, int _width);

	int getWidth() { return width; }
	bool isEmpty() { return size == 0; }
	bool isFull() { return size == capacity; }

	bool insertInsts(std::vector<Addr> insts);

	/* note:
	 * have to make sure fetch_buffer is not empty 
	 * before call this function 
	 * */
	std::vector<Addr> getInsts();

private:
	int size;
	int capacity;
	int width;
	std::list<std::vector<Addr>> fetch_buffer;

};

class FetchStage {
public:
	FetchStage() : pc(0), stall(false) {}
	bool isFetchBufferEmpty() { return fetch_buffer.isEmpty(); }
	bool isfetchBufferFull() { return fetch_buffer.isFull(); }

	/* note:
	 * have to make sure fetch_buffer is not empty
	 * before call this function
	 * */
	std::vector<Addr> getInsts() { return fetch_buffer.getInsts(); }

	void do_fetch_stage();

	void setPC(Addr _pc) { pc = _pc; }
	Addr getPC() { return pc; }

	/* number of insts fetched */
	int fetched_num = 0;

	int getFetchedNum() { return fetched_num; }
	/* get the ICache miss number */
	int getMissNum() { return icache.getMissNum(); }

	/* get the simulation cycle */
	Cycle getCycle() { return numCycle; }

private:
	ICache icache;
	FetchBuffer fetch_buffer;
	Addr pc;
	bool stall;
	Cycle numCycle = 0;
};

#endif