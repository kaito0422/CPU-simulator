#include <iostream>
#include "fetch.h"

#define SDRAM_SIZE	(512 * 1024)

CacheLine SDRAM[SDRAM_SIZE];

static int log2(int x)
{
    int res = 0;
    while(x - 1) {
        x = x >> 1;
        res++;
    }
    return res;
}

void ICache::initICache()
{
	/* set the cache size */
    size = setNum * assoc * cachelineSize;

    /* allocate the storage size for icache, and all the initial data is zero */
	for (int i = 0; i < setNum; i++) {
		CacheSet set;
		for (int j = 0; j < assoc; j++) {
			struct CacheLine tmp_line;
			tmp_line.tag = -1;
			for (int k = 0; k < cachelineSize; k++)
				tmp_line.data[k] = 0;
			set.push_back(tmp_line);
		}
		cache.push_back(set);
	}

    /* calculate the Mask bits and Shift bits */
    offsetMask = cachelineSize - 1;
    offsetBits = log2(cachelineSize);
    indexMask = (setNum - 1) << offsetBits;
    indexBits = log2(setNum);
    tagMask = ~(indexMask | offsetMask);
    tagBits = ADDR_WIDTH - indexBits - offsetBits;

    miss_num = 0;
}

ICache::ICache(int _setNum, int _assoc, int _cachelineSize) : \
	setNum(_setNum), assoc(_assoc), cachelineSize(_cachelineSize)
{
	initICache();
}

ICache::CacheSet::iterator ICache::findVictim(int index)
{
	/* use the LRU replacement policy */
    auto iter = cache[index].end();
    iter--;
    return iter;
}

void ICache::access(Addr addr, Data *data_res, Cycle *latency)
{
    Addr tag = (addr & tagMask) >> (indexBits + offsetBits);
    int index = (addr & indexMask) >> offsetBits;
    int offset = addr & offsetMask;

    bool isFound = false;
    /* search the icache with the index to the cache set */
    for(auto line = cache[index].begin(); line != cache[index].end(); ++line) {
        if(line->tag == tag) {
			struct CacheLine tmp_line;
			tmp_line.tag = tag;
			for (int i = 0; i < cachelineSize; i++)
				tmp_line.data[i] = line->data[i];
            *data_res = line->data[offset];
            *latency = hit_latency;
            cache[index].push_front(tmp_line);
            cache[index].erase(line);
            isFound = true;
            break;
        }
    }
    *latency = 10;

    /* if icache miss */
    if(!isFound) {
        miss_num++;
        auto victim = findVictim(index);
		struct CacheLine tmp_line;
		tmp_line.tag = tag;
		for(int i = 0; i < cachelineSize; i++)
	        tmp_line.data[i] = SDRAM[(addr & tagMask) % SDRAM_SIZE].data[i];
        *data_res = tmp_line.data[offset];
        *latency += 100;
        cache[index].push_front(tmp_line);
        cache[index].erase(victim);
    }
}

bool FetchBuffer::insert(std::vector<Addr> insts)
{
	if (size == capacity || insts.size() > width) {
		std::cout << "Fetch stage: insts width larger than the buffer" \
			<< std::endl;
		return false;
	}

	buffer.push_back(insts);
	size++;
}

std::vector<Addr> FetchBuffer::get()
{
	std::vector<Addr> tmp_insts;
	tmp_insts = buffer.front();
	buffer.pop_front();
	size--;
	return tmp_insts;
}

void FetchStage::do_fetch_stage()
{
	if (stalled)
		return;
	Data tmp_data;
	Cycle latency = 0, tmp_latency;
	std::vector<Addr> insts;

	// kaito output info 
	std::cout << "PC Addr: " << pc << std::endl;
	// kaito end


	int width = fetch_buffer.getWidth();
	/* fetch 4 instructions from icache */
	for (int i = 0; i < width; i++) {
		icache.access(pc, &tmp_data, &tmp_latency);
		insts.push_back(tmp_data);
	}
	/* fetch stage only get 4 insts every time accessing icache,
	 * thus, we only add the latency once
	 */
	latency = tmp_latency;


	if (!fetch_buffer.isFull()) {
		fetch_buffer.insert(insts);
		pc = pc + 4;
	}
}

std::vector<Addr> FetchStage::getInsts()
{
	return fetch_buffer.get();
}

void FetchBuffer::clearBuffer()
{
	buffer.clear();
	size = 0;
}