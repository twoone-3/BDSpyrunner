#pragma once
#include "tool.h"
enum class CommandFlag : unsigned char {
	None = 0,
	Usage = 1,
	Visibility2 = 2,
	Visibility4 = 4,
	Visibility6 = 6,
	Sync = 8,
	Execute = 16,
	Type = 32,
	Cheat = 64,
};
struct CommandOutput {

};
struct CommandOrigin {

};
//所有命令的基类
struct Command {
protected:
	int unk8;           // 8
	void* unk16;        // 16
	int unk24;          // 24
	unsigned char b28;  // 28
	CommandFlag flag;   // 29
	Command() {
		SymCall("??0Command@@QEAA@XZ", this);
	}

public:
	virtual ~Command() { SymCall("??1Command@@UEAA@XZ", this); }
	virtual void execute(CommandOrigin*, CommandOutput*) = 0;
};
static_assert(sizeof(Command) == 32);