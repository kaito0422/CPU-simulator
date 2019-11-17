#include <iostream>
#include <windows.h>

#include "fetch.h"
#include "decode.h"

int main()
{
	FetchStage fetch;
	Decoder decoder;
	while (1) {
		/* fetch stage */
		fetch.do_fetch_stage();

		std::vector<Addr> insts_from_fetch;
		insts_from_fetch = fetch.getInsts();

		/* decode stage */
		decoder.insertInsts(insts_from_fetch);
		decoder.do_decoder_stage();

		if (fetch.getPC() > 10240) {
			std::cout << "ICache miss number: " \
				<< fetch.getCacheMissNum() << std::endl;
			break;
		}

		decoder.getBuffer();
	//	Sleep(1);
	}

	system("pause");
	return 0;
}