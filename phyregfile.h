#ifndef _PHYREGFILE_H
#define _PHYREGFILE_H

#include <vector>
#include "base.h"

class PhyRegFile {
public:
	PhyRegFile();
	PhyRegFile(int _num);

	/* allocate a physical register; -1 means allocate failed
	 * this function may be used in rename stage
	 * */
	int allocatePhyReg();

	/* free used physical register
	 * this function may be used in writeback/commit stage
	 * */
	void freePhyReg(int num);

	/* dereference operator overload */
	unsigned long& operator[] (int num);

private:
	int phyRegNum;
	/* limit the max number of PhyRegFile to 1024 */
	bool free_table[1024];
	std::vector<unsigned long> phy_reg_file;


};

#endif