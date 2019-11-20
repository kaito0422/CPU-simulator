#include <iostream>
#include <windows.h>

#include "fetch.h"
#include "decode.h"
#include "rename.h"

int main()
{
	FetchStage fetch;
	Decoder decoder;
	Rename rename;
	while (1) {
		/* fetch stage */
		fetch.do_fetch_stage();

		/* decode stage */
		std::vector<Addr> insts_from_fetch;
		insts_from_fetch = fetch.getInsts();
		decoder.insertInsts(insts_from_fetch);
		decoder.do_decoder_stage();

		/* rename stage */
		std::vector<instInfo> instsInfo_from_decode;
		instsInfo_from_decode = decoder.getBuffer();
		rename.getInsts(instsInfo_from_decode);
		rename.do_rename_stage();

		// kaito output info 
		std::cout << "PC Addr: " << fetch.getPC();
		std::vector<instInfo> insts_info = rename.outputInsts();
		
		// kaito end

		if (fetch.getPC() > 10240) {
			std::cout << "ICache miss number: " \
				<< fetch.getCacheMissNum() << std::endl;
			break;
		}

		
	//	Sleep(1);
	}

	system("pause");
	return 0;
}