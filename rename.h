#ifndef _RENAME_H_
#define _RENAME_H_

#include <vector>
#include <map>
#include "base.h"

struct PhysicalRegister {
	bool free;
	Data data;		// I am not sure if the plece that really store
					// the data should be set here
};

class Rename {
public:
	typedef struct PhysicalRegister PhyReg;
	Rename();
	Rename(int _phy_reg_num, int _width);
	int renameReg(int isa_reg);
	int allocaReg(int isa_reg);
	void getInsts(std::vector<instInfo> insts);
	std::vector<instInfo> outputInsts() { return tmp_insts; }
	void do_rename_stage();
	void freePhyReg(int phy_reg) { phy_reg_file[phy_reg].free = true; }

private:
	const int isa_reg_num = 11;
	std::vector<int> isa_reg_file;
	int phy_reg_num;
	std::vector<PhyReg> phy_reg_file;
	int width;
	std::vector<instInfo> tmp_insts;
	bool worded;

};

#endif