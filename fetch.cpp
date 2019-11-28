#include "fetch.h"
#include "base.h"
#include "sdram.h"
#include <iostream>
#include <vector>
#include <list>
#include <cmath>

#define SDRAM_LATENCY   100
#define Min(a, b)       (a > b ? b : a)

/* instance an SDRAM */
extern SDRAM sdram;

/* initialize the cache configurations */
void ICache::initCacheConfig()
{
    size = setNum * assoc * line_size;
    indexShift = log2(LINE_SIZE);
    indexMask = (setNum - 1) << indexShift;
    tagShift = indexShift + log2(setNum);
    tagMask = ((int)pow(2, ADDR_LEN - tagShift) - 1) << tagShift;

    /* allocate the place for the cache */
    CacheLine line_tmp;
    line_tmp.tag = 0x00000000;
    line_tmp.invalid = true;
    /* initialize one cacheline */
    for(int i = 0; i < LINE_SIZE; i++)  /* cacheline size */
        line_tmp.data[i] = 0;
    /* initialize one cache set */
    std::list<CacheLine> set_tmp;
    for(int i = 0; i < assoc; i++)      /* cache associativity */
        set_tmp.push_back(line_tmp);
    /* initialize ICache */
    for(int i = 0; i < setNum; i++)     /* set number */
        cache.push_back(set_tmp);
}

ICache::ICache() : setNum(64), assoc(4), latency(2)
{
    initCacheConfig();
}

ICache::ICache(int _setNum, int _assoc, Cycle _latency) : setNum(_setNum), assoc(_assoc), latency(_latency)
{
    initCacheConfig();
}

/* extract the tag value of the pc */
Addr ICache::getTag(Addr pc)
{
    return (pc & tagMask) >> tagShift;
}

/* extract the index value of the pc */
int ICache::getIndex(Addr pc)
{
    return (pc & indexMask) >> indexShift;
}

/* extract the offset value of the pc */
int ICache::getOffset(Addr pc)
{
    return pc & (LINE_SIZE - 1);
}

/* find a victim cacheline when cache miss */
std::list<CacheLine>::iterator ICache::findVictim(int index)
{
	std::list<CacheLine>::iterator iter = cache[index].end();
    iter--;
    return iter;
}

/* access one cacheline in the cache */
CacheLine ICache::accessCacheLine(Addr pc, Cycle *lat)
{
    /* because we return all the cacheline, offset isn't needed */
    Addr addr = alignCacheLine(pc);
    Addr tag = getTag(addr);
    int index = getIndex(addr);

    CacheLine line_tmp;     /* used to return */
	
    /* index to the cache set */
    std::list<CacheLine> &set = cache[index];
    /* search all the cachelines in the cache set */
    bool Found = false;
    for(auto iter = set.begin(); iter != set.end(); ++iter) {
        /* if the cacheline is in the cache and valid */
        if(iter->tag == tag && iter->invalid == false) {
            line_tmp.tag = tag;
            line_tmp.invalid = false;
            for(int i = 0; i < LINE_SIZE; i++)
                line_tmp.data[i] = iter->data[i];
            /* using the LRU replacement policy */
            set.push_front(line_tmp);
            set.erase(iter);
            Found = true;
            break;
        }
    }

    /* accumulate the accessing latency */
    *lat = latency;

    /* if cache miss */
    if(!Found) {
		std::list<CacheLine>::iterator victim =  findVictim(index);
        line_tmp.tag = tag;
        line_tmp.invalid = false;
        CacheLine line = sdram.access(pc);
		for (int i = 0; i < LINE_SIZE; i++)
			line_tmp.data[i] = line.data[i];       /* need to from the SDRAM DDR */
        /* using the LRU replacement policy */
        set.push_front(line_tmp);
        set.erase(victim);
        *lat = latency + SDRAM_LATENCY;     /* cache miss, latency increases */
        miss_num++;
    }

    return line_tmp;
}

/* access ICache */
void ICache::access(Addr pc, std::vector<Addr> &inst_vec, int inst_num, Cycle *lat)
{
    Addr addr = align(pc);          /* align 4-bytes */
    Addr tag = getTag(addr);
    int index = getIndex(addr);
    int offset = getOffset(addr);

    /* determine which cacheline to access
     * or, even, determine how many cacheline we need to access
     * */
    /* only access one cacheline */
    if((pc % 64) / 4 + inst_num - 1 < 16) {			/* 16 = LINE_SIZE / 4 */
        CacheLine line = accessCacheLine(addr, lat);
        for(int i = 0; i < inst_num; i++) {
            Addr inst = 0;
            inst = line.data[4 * i + offset + 3];
            inst = inst << 8;
            inst = inst | line.data[4 * i + offset + 2];
            inst = inst << 8;
            inst = inst | line.data[4 * i + offset + 1];
            inst = inst << 8;
            inst = inst | line.data[4 * i + offset + 0];
            inst_vec.push_back(inst);
        }   
    }
    /* need to access two cacheline because of unaligned */
    else {
        Cycle c1, c2;
        CacheLine line1 = accessCacheLine(addr, &c1);
        CacheLine line2 = accessCacheLine(addr + 64, &c2);
        *lat = c1 + c2;
        /* get the inst data from the first cacheline */
        for(int i = (addr % 64) / 4; i < LINE_SIZE / 4; i++) {
            Addr inst = 0;
            inst = line1.data[4 * i + /*offset*/ 0 + 3];
            inst = inst << 8;
            inst = inst | line1.data[4 * i + /*offset*/ 0 + 2];
            inst = inst << 8;
            inst = inst | line1.data[4 * i + /*offset*/ 0 + 1];
            inst = inst << 8;
            inst = inst | line1.data[4 * i + /*offset*/ 0 + 0];
            inst_vec.push_back(inst);
        }
        /* get the inst data from the second cacheline */
        for(int i = 0; i < inst_num - (LINE_SIZE / 4 - (addr % 64) / 4); i++) {
            Addr inst = 0;
            inst = line2.data[4 * i + 3];
            inst = inst << 8;
            inst = inst | line2.data[4 * i + 2];
            inst = inst << 8;
            inst = inst | line2.data[4 * i + 1];
            inst = inst << 8;
            inst = inst | line2.data[4 * i + 0];
            inst_vec.push_back(inst);
        }
    }
}

FetchBuffer::FetchBuffer() : size(0), capacity(16), width(4)
{

}

FetchBuffer::FetchBuffer(int _capacity, int _width) : 
	size(0), capacity(_capacity), width(_width)
{

}

bool FetchBuffer::insertInsts(std::vector<Addr> insts)
{
	if (isFull()) {
		std::cout << "Fetch stage: FetchBuffer is full" << std::endl;
		return false;
	}

	if (insts.size() > width) {
		std::cout << "insts width: " << insts.size() << ";\tbuffer width: " << width << std::endl;
		std::cout << "Fetch stage: insts width larger then FetchBuffer width" << std::endl;
		return false;
	}

	fetch_buffer.push_back(insts);

	size++;
	return true;
}

/* note:
* have to make sure fetch_buffer is not empty
* before call this function
* */
std::vector<Addr> FetchBuffer::getInsts()
{
	std::vector<Addr> insts;
	insts = fetch_buffer.front();
	fetch_buffer.pop_front();
	size--;
	return insts;
}

void FetchStage::do_fetch_stage()
{
	/* if the FetchBuffer is stalled, the fetch stage cannot work */
	if (fetch_buffer.isFull() || stall) {
		stall = true;
		return;
	}

	std::vector<Addr> inst_vec;
	Cycle lat;
	/* access icache */
	icache.access(pc, inst_vec, fetch_buffer.getWidth(), &lat);
	numCycle = numCycle + lat;

	/* insert the insts get from icache to FetchBuffer */
	fetch_buffer.insertInsts(inst_vec);

	/* increase pc */
	pc = pc + 16;

	/* recode the number of insts fetched */
	fetched_num = fetched_num + 4;
}
