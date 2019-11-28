#include "decode.h"
#include <iostream>

std::vector<InstInfo> Decoder::decodeInsts(std::vector<Addr> insts, Cycle *lat)
{
	std::vector<InstInfo> res_info;

	for (int i = 0; i < width; i++) {
		Addr inst = insts[i];
		InstOp op;
		ISAReg reg_dst, reg_src1, reg_src2;
		bool imdt_used;
		unsigned short imdt;

		/* extract the inst info */
		op = (InstOp)((inst & OPERATION_MASK) >> OPERATION_SHIFT);
		reg_dst = (ISAReg)((inst & REG_DST_MASK) >> REG_DST_SHIFT);
		reg_src1 = (ISAReg)((inst & REG_SRC1_MASK) >> REG_SRC1_SHIFT);
		reg_src2 = (ISAReg)((inst & REG_SRC2_MASK) >> REG_SRC2_SHIFT);
		imdt = (inst & IMDT_MASK) >> IMDT_SHIFT;
		/* if all bits of imdt is 1, imdt is not used
		 * and this needs compiler to support 
		 * */
		imdt_used = imdt != (IMDT_MASK >> IMDT_SHIFT);

		InstInfo info_tmp;
		info_tmp.op = op;
		info_tmp.reg_dst = reg_dst;
		info_tmp.reg_src1 = reg_src1;
		info_tmp.reg_src2 = reg_src2;
		info_tmp.imdt_used = imdt_used;
		info_tmp.imdt = imdt;
		res_info.push_back(info_tmp);
	}

	/* set the decode latency */
	*lat = latency;

	return res_info;
}

DecodeBuffer::DecodeBuffer() : size(0), capacity(16), width(4)
{

}

DecodeBuffer::DecodeBuffer(int _capacity, int _width) : size(0), capacity(_capacity), width(_width)
{

}

bool DecodeBuffer::insertInfo(std::vector<InstInfo>& info)
{
	if (isFull()) {
		std::cout << "Decode stage: decode buffer is full" << std::endl;
		return false;
	}

	if (info.size() > width) {
		std::cout << "Decode stage: info width larger than width" << std::endl;
		return false;
	}

	decode_buffer.push_back(info);

	size++;

	return true;
}

/* get insts info from the decode buffer
* you have to check  if decode buffer is empty before call this function
* */
std::vector<InstInfo> DecodeBuffer::getInstInfo()
{
	std::vector<InstInfo> info_tmp;
	info_tmp = decode_buffer.front();
	decode_buffer.pop_front();
	size--;
	return info_tmp;
}

DecodeStage::DecodeStage()
{
	decoder.setWidth(4);
	decode_buffer.setWidth(4);
}

DecodeStage::DecodeStage(int _width)
{
	decoder.setWidth(_width);
	decode_buffer.setWidth(_width);
}

/* get insts from fetch stage; 
 * only when worked is false, this function can be called
 * and worked should be marked as true to tell the decoder to decode
 * */
bool DecodeStage::insertInsts(std::vector<Addr> insts)
{
	if (worked || decode_buffer.isFull()) {
		std::cout << "Decode stage: can not insert insts into decode stage" << std::endl;
		if (worked)
			std::cout << "worked" << std::endl;
		else
			std::cout << "decode_buffer is full" << std::endl;
		return false;
	}

	insts_in.clear();
	insts_in = insts;

	worked = true;

	return true;
}

void DecodeStage::do_decode_stage()
{
	if (!worked || isFull())
		return;

	std::vector<InstInfo> info_tmp;
	Cycle lat;

	info_tmp = decoder.decodeInsts(insts_in, &lat);
	worked = false;				/* to tell the outside that decode is not busy */

	decode_buffer.insertInfo(info_tmp);
}

/* get insts info 
 * have to check if the decode buffer is empty
 * */
std::vector<InstInfo> DecodeStage::getInstInfo()
{
	return decode_buffer.getInstInfo();
}