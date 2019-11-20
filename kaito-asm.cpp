#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

#define OP_SHIFT		0
#define DST_SHIFT		3
#define SRC1_SHIFT		8
#define SRC2_SHIFT		13
#define IMDT_SHIFT		18

int main()
{
	std::ifstream infile("add.kasm");
	std::string str;
	std::ofstream outfile("add.kbin");

	while (std::getline(infile, str)) {
		std::cout << str << std::endl;
		std::istringstream ss(str);
		std::string op, src1, src2, dst;
		int imdt, isa_reg_src1, isa_reg_src2, isa_reg_dst;
		bool imdt_used = false;
		unsigned int hex_code = 0x00000000;
		
		/* get the op field */
		ss >> op;
		if (op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV") {
			ss >> dst >> src1 >> src2;
			dst.erase(--dst.end());
			src1.erase(--src1.end());

			/* detect if immediate used */
			if (src2[0] == '#') {
				imdt_used = true;
				src2 = src2.substr(1, src2.size() - 1);
				std::istringstream s_src2(src2);
				s_src2 >> imdt;
			}
			else {
				src2 = src2.substr(1, src2.size() - 1);
				std::istringstream s_src2(src2);
				s_src2 >> isa_reg_src2;
			}

			/* detect destination register */
			dst = dst.substr(1, dst.size() - 1);
			std::istringstream s_dst(dst);
			s_dst >> isa_reg_dst;

			/* detect the source1 register */
			src1 = src1.substr(1, src1.size() - 1);
			std::istringstream s_src1(src1);
			s_src1 >> isa_reg_src1;

			if(!imdt_used)
				hex_code = 0 | isa_reg_dst << DST_SHIFT | isa_reg_src1 << SRC1_SHIFT \
					| isa_reg_src2 << SRC2_SHIFT | 0 << IMDT_SHIFT;
			else
				hex_code = 0 | isa_reg_dst << DST_SHIFT | isa_reg_src1 << SRC1_SHIFT \
				| 10 << SRC2_SHIFT | imdt << IMDT_SHIFT;

			if (op == "ADD")
				hex_code = hex_code | 0;
			if (op == "SUB")
				hex_code = hex_code | 1;
			if (op == "MUL")
				hex_code = hex_code | 2;
			if (op == "DIV")
				hex_code = hex_code | 3;

			outfile << std::hex << std::setw(8) << std::setfill('0') << hex_code << std::endl;
		}
		else if (op == "LDR") {
			ss >> dst >> src1;

			/* detect destination register */
			dst = dst.substr(1, dst.size() - 1);
			std::istringstream s_dst(dst);
			s_dst >> isa_reg_dst;

			/* detect the source1 register */
			src1 = src1.substr(2, src1.size() - 1);
			std::istringstream s_src1(src1);
			s_src1 >> isa_reg_src1;

			hex_code = 4 | isa_reg_dst << DST_SHIFT | isa_reg_src1 << SRC1_SHIFT | 0 << SRC2_SHIFT | \
				0 << IMDT_SHIFT;

			outfile << std::hex << std::setw(8) << std::setfill('0') << hex_code << std::endl;
		}
		else if (op == "STR") {
			ss >> dst >> src1;

			/* detect destination register */
			dst = dst.substr(2, dst.size() - 1);
			std::istringstream s_dst(dst);
			s_dst >> isa_reg_dst;

			/* detect the source1 register */
			src1 = src1.substr(1, src1.size() - 1);
			std::istringstream s_src1(src1);
			s_src1 >> isa_reg_src1;

			hex_code = 5 | isa_reg_dst << DST_SHIFT | isa_reg_src1 << SRC1_SHIFT | 0 << SRC2_SHIFT | \
				0 << IMDT_SHIFT;

			outfile << std::hex << std::setw(8) << std::setfill('0') << hex_code << std::endl;
		}
		else if (op == "JMP") {
			ss >> src1;

			/* detect the source1 register */
			src1 = src1.substr(1, src1.size() - 1);
			std::istringstream s_imdt(src1);
			s_imdt >> imdt;

			hex_code = 6 | 0 << DST_SHIFT | 0 << SRC1_SHIFT | 0 << SRC2_SHIFT | \
				imdt << IMDT_SHIFT;

			outfile << std::hex << std::setw(8) << std::setfill('0') << hex_code << std::endl;
		}
		else if (op == "MOV") {
			ss >> dst >> src1;
			dst.erase(--dst.end());

			dst = dst.substr(1, dst.size() - 1);
			std::istringstream s_dst(dst);
			s_dst >> isa_reg_dst;

			/* detect if immediate used */
			if (src1[0] == '#') {
				imdt_used = true;
				src1 = src1.substr(1, src1.size() - 1);
				std::istringstream s_src1(src1);
				s_src1 >> imdt;
			}
			else {
				src1 = src1.substr(1, src1.size() - 1);
				std::istringstream s_src1(src1);
				s_src1 >> isa_reg_src1;
			}

			if (imdt_used)
				hex_code = 7 | isa_reg_dst << DST_SHIFT | 0 << SRC1_SHIFT | 0 << SRC2_SHIFT | \
						imdt << IMDT_SHIFT;
			else
				hex_code = 7 | isa_reg_dst << DST_SHIFT | isa_reg_src1 << SRC1_SHIFT | 0 << SRC2_SHIFT | \
						0 << IMDT_SHIFT;

			outfile << std::hex << std::setw(8) << std::setfill('0') << hex_code << std::endl;
		}
	}
	
	infile.close();
	outfile.close();

	system("pause");
	return 0;
}