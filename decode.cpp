#include <iostream>
#include "decode.h"

bool Decoder::insertBuffer(std::vector<instInfo> insts)
{
	if (isFull() || insts.size() > width) {
		std::cout << "Decoder stage: Input insts width larger than buffer width" \
			<< std::endl;
		return false;
	}

	decode_buffer.push_back(insts);
	size++;
	return true;
}

/* you have to check if the buffer is empty before you get the insts */
std::vector<instInfo> Decoder::getBuffer()
{
	std::vector<instInfo> tmp_insts;
	tmp_insts = decode_buffer.front();
	decode_buffer.pop_front();
	size--;
	return tmp_insts;
}

bool Decoder::decodeInst(std::vector<Addr> insts, std::vector<instInfo> &inst_info)
{
	if (insts.size() > width) {
		std::cout << "Decoder stage: insts number larger than width" \
			<< std::endl;
		return false;
	}
	else if (!decoder_in_work) {
		std::cout << "Decoder stage: no inst for decoder" \
			<< std::endl;
		return false;
	}

	int num_insts = insts.size();
	for (int i = 0; i < num_insts; i++) {
		Addr addr = insts[i];
		instInfo info;
		InstOp op;
		InstDataType dt;
		InstReg reg_src1, reg_src2, reg_dst;
		bool imdt;

		/* detect the operation type */
		switch (addr & OPERATION_MASK) {
		case 0: 
			op = InstOp::ADD;
			break;
		case 1:
			op = InstOp::SUB;
			break;
		case 2:
			op = InstOp::MUL;
			break;
		case 3:
			op = InstOp::DIV;
			break;
		case 4:
			op = InstOp::LDR;
			break;
		case 5:
			op = InstOp::STR;
			break;
		case 6:
			op = InstOp::JMP;
			break;
		default:
			op = InstOp::MOV;
			break;
		}

		/* detect the data type */
		if ((addr & DATATYPE_MASK_SRC1) | \
			(addr & DATATYPE_MASK_SRC2) | \
			(addr & DATATYPE_MASK_DST))
			dt = InstDataType::FLOAT;
		else
			dt = InstDataType::INT;

		/* detecet the used registers */
		switch ((addr & REGISTER_MASK_SRC1) >> REGISTER_SHIFT_SRC1) {
		case 0:
			reg_src1 = InstReg::R0;
			break;
		case 1:
			reg_src1 = InstReg::R1;
			break;
		case 2:
			reg_src1 = InstReg::R2;
			break;
		case 3:
			reg_src1 = InstReg::R3;
			break;
		case 4:
			reg_src1 = InstReg::R4;
			break;
		case 5:
			reg_src1 = InstReg::R5;
			break;
		case 6:
			reg_src1 = InstReg::R6;
			break;
		case 7:
			reg_src1 = InstReg::R7;
			break;
		case 8:
			reg_src1 = InstReg::R8;
			break;
		case 9:
			reg_src1 = InstReg::R9;
			break;
		default:
			reg_src1 = InstReg::RX;
			break;
		}

		switch ((addr & REGISTER_MASK_SRC2) >> REGISTER_SHIFT_SRC2) {
		case 0:
			reg_src2 = InstReg::R0;
			break;
		case 1:
			reg_src2 = InstReg::R1;
			break;
		case 2:
			reg_src2 = InstReg::R2;
			break;
		case 3:
			reg_src2 = InstReg::R3;
			break;
		case 4:
			reg_src2 = InstReg::R4;
			break;
		case 5:
			reg_src2 = InstReg::R5;
			break;
		case 6:
			reg_src2 = InstReg::R6;
			break;
		case 7:
			reg_src2 = InstReg::R7;
			break;
		case 8:
			reg_src2 = InstReg::R8;
			break;
		case 9:
			reg_src2 = InstReg::R9;
			break;
		default:
			reg_src2 = InstReg::RX;
			break;
		}

		switch ((addr & REGISTER_MASK_DST) >> REGISTER_SHIFT_DST) {
		case 0:
			reg_dst = InstReg::R0;
			break;
		case 1:
			reg_dst = InstReg::R1;
			break;
		case 2:
			reg_dst = InstReg::R2;
			break;
		case 3:
			reg_dst = InstReg::R3;
			break;
		case 4:
			reg_dst = InstReg::R4;
			break;
		case 5:
			reg_dst = InstReg::R5;
			break;
		case 6:
			reg_dst = InstReg::R6;
			break;
		case 7:
			reg_dst = InstReg::R7;
			break;
		case 8:
			reg_dst = InstReg::R8;
			break;
		case 9:
			reg_dst = InstReg::R9;
			break;
		default:
			reg_dst = InstReg::RX;
			break;
		}

		/* detect the immediate */
		imdt = (addr & IMMEDIATE_MASK) >> IMMEDIATE_SHIFT;

		/* store the inst info */
		instInfo tmp_info;
		tmp_info.op = op;
		tmp_info.dt = dt;
		tmp_info.reg_src1 = reg_src1;
		tmp_info.reg_src2 = reg_src2;
		tmp_info.reg_dst = reg_dst;
		tmp_info.imdt = imdt;
		inst_info.push_back(tmp_info);
	}
	/* release deocedr, then the next inst packet can be decoded */
	decoder_in_work = false;
	return true;
}

bool Decoder::insertInsts(std::vector<Addr> insts)
{
	if (isFull()) {
		std::cout << "Decoder stage: decode_buffer is full" << std::endl;
		return false;
	}
	else if(decoder_in_work) {
		std::cout << "Decoder stage: decoder is in work" << std::endl;
		return false;
	}

	inst_pkt = insts;
	decoder_in_work = true;
	return true;
}

void Decoder::do_decoder_stage()
{
	if (decoder_in_work || isFull()) {
		std::vector<Addr> tmp_insts = inst_pkt;
		std::vector<instInfo> tmp_info;
		bool res;

		res = decodeInst(tmp_insts, tmp_info);
		if(res)
			insertBuffer(tmp_info);
	}
}