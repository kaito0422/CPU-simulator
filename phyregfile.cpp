#include "phyregfile.h"
#include <iostream>

PhyRegFile::PhyRegFile() : phyRegNum(32)
{
	for (int i = 0; i < phyRegNum; i++) {
		free_table[i] = true;		/* set every phy reg to free state */
		phy_reg_file.push_back(0);
	}
}

PhyRegFile::PhyRegFile(int _num) : phyRegNum(_num)
{
	for (int i = 0; i < phyRegNum; i++) {
		free_table[i] = true;
		phy_reg_file.push_back(0);
	}
}

/* allocate a free physical register */
int PhyRegFile::allocatePhyReg()
{
	for (int i = 0; i < phyRegNum; i++) {
		if (free_table[i]) {
			free_table[i] = false;
			return i;
		}
	}

	/* if no free physical register, return -1 */
	return -1;
}

/* free a used physical register */
void PhyRegFile::freePhyReg(int num)
{
	free_table[num] = true;
}

/* dereference operator overload */
unsigned long& PhyRegFile::operator[] (int num)
{
	if (free_table[num]) {
		std::cout << "Warning: PhyRegFile: accessed physical register is not allocated" \
			<< std::endl;
	}

	return phy_reg_file[num];
}