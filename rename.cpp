#include "rename.h"
#include "phyregfile.h"
#include <iostream>

extern PhyRegFile phy_reg_file;

RenameBuffer::RenameBuffer() : capacity(16)
{

}

RenameBuffer::RenameBuffer(int _capacity) : capacity(_capacity)
{

}

/* insert info to the rename buffer */
bool RenameBuffer::insertInstInfo(std::vector<RenamedInstInfo> info)
{
	if (isFull()) {
		std::cout << "Rename stage: rename buffer is full" << std::endl;
		return false;
	}

	rename_buffer.push_back(info);

	size++;

	return true;
}

/* get info from the rename buffer
 * you have to check if the rename buffer is empty
 * before call this function
 * */
std::vector<RenamedInstInfo> RenameBuffer::getInstInfo()
{
	std::vector<RenamedInstInfo> info_tmp;
	info_tmp = rename_buffer.front();
	size--;
	return info_tmp;
}

RenameStage::RenameStage()
{
	for (int i = 0; i < 16; i++)
		rat[i] = 0;
}

/* get the inst info from the decode buffer 
 * if the info is pushed into rename stage, rename stage should be marked as worked
 * */
bool RenameStage::insertInstInfo(std::vector<InstInfo> info)
{
	if(isFull()) {
		std::cout << "Rename stage: rename buffer is full" << std::endl;
		return false;
	}

	info_in.clear();
	info_in = info;

	worked = true;
	return true;
}

/* get info from the rename buffer
 * you have to check if the rename buffer is empty
 * before call this function
 * */
std::vector<RenamedInstInfo> RenameStage::getInstInfo()
{
	return rename_buffer.getInstInfo();
}

/* rename the insts info
 * if all insts are renamed successfully, return true;
 * else free the allocated physical registers and return false
 * /* mentioned to clear info_res before call this function
 * */
bool RenameStage::renameInstInfo(std::vector<InstInfo>& info, std::vector<RenamedInstInfo>& info_res)
{
	int len = info.size();
	
	bool if_success = true;
	/* this container is used to recorde all the allocated physical registers
	 * in this function
	 * */
	std::vector<int> already_allocate;

	for (int i = 0; i < len; i++) {
		/* get the ISAReg number
		 * the src1 and src2 are used to access RAT to get where the data stored in 
		 * physical register;
		 * any dst should allocate a free physical register
		 * */
		int reg_dst = (int)info[i].reg_dst;
		int reg_src1 = (int)info[i].reg_src1;
		int reg_src2 = (int)info[i].reg_src2;

		int res;
		/* kaito: you need to notice that not all insts need register */
		res = phy_reg_file.allocatePhyReg();
		/* if physical register allocation failed, free all the allocated
		 * physical register in already_allocate and marked if_success = false
		 * */
		if (reg_dst == -1) {
			if_success = false;
			break;
		}
		else {
			reg_dst = res;
			reg_src1 = rat[reg_src1];
			reg_src2 = rat[reg_src2];
			already_allocate.push_back(reg_dst);		/* recorde the allocated physical register */

			RenamedInstInfo info_tmp;
			info_tmp.op = info[i].op;
			info_tmp.reg_dst = reg_dst;
			info_tmp.reg_src1 = reg_src1;
			info_tmp.reg_src2 = reg_src2;
			info_tmp.imdt_used = info[i].imdt_used;
			info_tmp.imdt = info[i].imdt;

			info_res.push_back(info_tmp);
		}

	}

	if (if_success) {
		return true;
	}
	else {
		/* rename failed because of lack of free physical registers
		 * however, the already allocated physical registers should
		 * be freed
		 * */
		for (auto i : already_allocate)
			phy_reg_file.freePhyReg(i);

		return false;
	}
}

void RenameStage::do_rename_stage()
{
	if (!worked || isFull())
		return;

	bool res;
	std::vector<RenamedInstInfo> info_tmp;
	res = renameInstInfo(info_in, info_tmp);

	/* if rename failed, rename stage is still worked; in the next stage of rename,
	 * we should still try to rename these insts, until rename success
	 * */
	worked = res ? false : true;

	/* only when rename success, the renamed insts info can be pushed into the rename buffer */
	if(res)
		rename_buffer.insertInstInfo(info_tmp);
}