#include <tool.h>
#include <global.h>
#include <Level.h>

using namespace std;
void* SymHook(const char* sym, void* hook, void* org) {
	void* found = SYM(sym);
	if (!found)
		cerr << "Failed to hook " << sym << endl;
	HookFunction(found, org, hook);
	return org;
}

constexpr size_t Hash(const char* s) {
	unsigned h = 0;
	for (; *s; ++s)
		h = 5 * h + *s;
	return static_cast<size_t>(h);
}

//创建包
uintptr_t createPacket(int type) {
	uintptr_t pkt[2];
	SymCall("?createPacket@MinecraftPackets@@SA?AV?$shared_ptr@VPacket@@@std@@W4MinecraftPacketIds@@@Z",
		pkt, type);
	return *pkt;
}
//是否为玩家
bool isPlayer(void* ptr) {
	for (auto& p : g_level->getAllPlayers()) {
		if (ptr == p)
			return true;
	}
	return false;
}
