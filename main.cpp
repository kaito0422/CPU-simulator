#include <iostream>
#include "base.h"
#include "fetch.h"
#include "decode.h"
#include "sdram.h"
#include "phyregfile.h"
#include "rename.h"

/* instance an SDRAM */
SDRAM sdram("code.kbin");

/* instance a physical register file */
PhyRegFile phy_reg_file;

int main()
{
	FetchStage fetch;
	DecodeStage decode;
	RenameStage rename;

	/* simulation start */
	std::cout << range_line << "simulation start" << std::endl;

	/* if the PC starts with 0, the simulated cycles are 216;
	 * if the PC starts with 8, the simulated cycles are 320
	 */
	fetch.setPC(0);

	while (1) {
		/* kaito info */
		std::cout << "PC: " << fetch.getPC() << "\t";
		std::cout << "Num of insts fetched: " << fetch.getFetchedNum() << std::endl;

		/******************************* fetch stage *******************************/
		/* do fetch stage */
		fetch.do_fetch_stage();


		/******************************* decode stage *******************************/
		/* if the fetch_buffer is not empty and decode stage is not busy,
		 * then, we can do decode stage 
		 * */
		if ((!fetch.isFetchBufferEmpty()) && (!decode.isBusy())) {
			/* get the insts from the fetch buffer */
			std::vector<Addr> insts = fetch.getInsts();
			/* put the insts to decoder */
			decode.insertInsts(insts);

			for (auto i : insts)
				std::cout << i << ' ';
			std::cout << std::endl;
		}

		/* do the decode stage */
		decode.do_decode_stage();

		if (fetch.isfetchBufferFull())
			std::cout << "FetchBuffer is full" << std::endl;

		if (decode.isFull())
			std::cout << "DecodeBuffer is full" << std::endl;

		/******************************* rename stage *******************************/
		std::vector<InstInfo> info_tmp;
		/* before transfer insts info from decode stage to rename stage, we have to 
		 * make sure that decode buffer is not empty and rename stage is not busy
		 * */
		if (!decode.isEmpty() && !rename.isBusy()) {
			info_tmp = decode.getInstInfo();
			rename.insertInstInfo(info_tmp);
		}

		/* do rename stage */
		rename.do_rename_stage();

		if (!rename.isEmpty()) {
			auto res = rename.getInstInfo();
			for (auto i : res) {
				phy_reg_file.freePhyReg(i.reg_dst);
				phy_reg_file.freePhyReg(i.reg_src1);
				phy_reg_file.freePhyReg(i.reg_src2);
			}
		}

		/******************************* stop the simulation *******************************/
		/* loop */
		if (fetch.getFetchedNum() >= 1024)
			break;
	}

	std::cout << "ICache miss number: " << fetch.getMissNum() << std::endl;
	std::cout << "Simulated cycle: " << fetch.getCycle() << std::endl;
	std::cout << "simulation finished" << std::endl;

	/* test the physical register file */
	std::cout << range_line << "test the physical register file" << std::endl;
	int reg_num = phy_reg_file.allocatePhyReg();
	std::cout << phy_reg_file[reg_num] << std::endl;
	phy_reg_file[reg_num] = 22;
	std::cout << phy_reg_file[reg_num] << std::endl;


	system("pause");
	return 0;
}