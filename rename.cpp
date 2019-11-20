#include <iostream>
#include "rename.h"

Rename::Rename() : phy_reg_num(isa_reg_num), width(4), worded(false)
{
	for (int i = 0; i < isa_reg_num; i++) {
		isa_reg_file.push_back(i);
		PhyReg tmp_reg = { true, 0 };
		phy_reg_file.push_back(tmp_reg);
	}
}

Rename::Rename(int _phy_reg_num, int _width) : \
			phy_reg_num(_phy_reg_num), width(_width), worded(false)
{
	for (int i = 0; i < isa_reg_num; i++)
		isa_reg_file.push_back(i);
	for (int i = 0; i < phy_reg_num; i++) {
		PhyReg tmp_reg = { true, 0 };
		phy_reg_file.push_back(tmp_reg);
	}
}

int Rename::renameReg(int isa_reg)
{
	isa_reg = isa_reg % isa_reg_num;
	return isa_reg_file[isa_reg];
}

int Rename::allocaReg(int isa_reg)
{
	isa_reg = isa_reg % isa_reg_num;
	for (int i = 0; i < phy_reg_num; i++) {
		if (!phy_reg_file[i].free)
			continue;
		phy_reg_file[i].free = false;
		isa_reg_file[isa_reg] = i;
		return i;
	}
}

void Rename::getInsts(std::vector<instInfo> insts)
{
	tmp_insts.clear();
	if (insts.size() != width) {
		std::cout << "Rename stage: insts width not equal to rename width" \
			<< std::endl;
		return;
	}
	worded = true;
	tmp_insts = insts;
}

void Rename::do_rename_stage()
{
	if (!worded)
		return;
	int src1, src2, dst;
	for (int i = 0; i < width; i++) {
		src1 = tmp_insts[i].reg_src1;
		src2 = tmp_insts[i].reg_src2;
		dst = tmp_insts[i].reg_dst;
		tmp_insts[i].reg_src1 = renameReg(src1);
		tmp_insts[i].reg_src2 = renameReg(src2);
		tmp_insts[i].reg_dst = allocaReg(dst);
	}
}