#ifndef _RENAME_H_
#define _RENAME_H_

#include <vector>
#include <list>
#include "base.h"

class RenameBuffer {
public:
	RenameBuffer();
	RenameBuffer(int _capacity);

	bool isFull() { return size == capacity; }
	bool isEmpty() { return size == 0; }

	/* insert info to the rename buffer */
	bool insertInstInfo(std::vector<RenamedInstInfo> info);

	/* get info from the rename buffer 
	 * you have to check if the rename buffer is empty
	 * before call this function
	 * */
	std::vector<RenamedInstInfo> getInstInfo();
	
private:
	int size;
	int capacity;
	std::list<std::vector<RenamedInstInfo>> rename_buffer;

};

class RenameStage {
public:
	RenameStage();

	bool isFull() { return rename_buffer.isFull(); }
	bool isEmpty() { return rename_buffer.isEmpty(); }

	/* get the inst info from the decode buffer */
	bool insertInstInfo(std::vector<InstInfo> info);

	/* get info from the rename buffer
	 * you have to check if the rename buffer is empty
	 * before call this function
	 * */
	std::vector<RenamedInstInfo> getInstInfo();

	/* mentioned to clear info_res before call this function */
	bool renameInstInfo(std::vector<InstInfo>& info, std::vector<RenamedInstInfo>& info_res);

	/* do rename stage */
	void do_rename_stage();

	bool isBusy() { return worked || isFull(); }

private:
	RenameBuffer rename_buffer;
	bool worked = false;
	std::vector<InstInfo> info_in;
	int rat[16];		/* RAT: Register Allocation Table
						 * 16 is the number of general 
						 * purpose registers
						 * */
};

#endif