#include "NetWork.h"
#include "tool.h"

using namespace std;
string NetworkIdentifier::getAddress() {
	string str;
	SymCall<string&>("?getAddress@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &str);
	return str;
}

string NetworkIdentifier::toString() {
	string str;
	SymCall<string&>("?toString@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &str);
	return str;
}

SystemAddress::SystemAddress() {
	SymCall("??0SystemAddress@RakNet@@QEAA@XZ", this);
}

string SystemAddress::toString() {
	return SymCall<const char*>("?ToString@SystemAddress@RakNet@@QEBAPEBD_ND@Z",
		this, true, ':');
}

SystemAddress RakPeer::getSystemAddress(NetworkIdentifier* ni) {
	SystemAddress sa;
	RakNetGUID& guid = Dereference<RakNetGUID>(ni->_this, 8);
	SymCall<SystemAddress&>("?GetSystemAddressFromGuid@RakPeer@RakNet@@UEBA?AUSystemAddress@2@URakNetGUID@2@@Z",
		this, &sa, &guid);
	return sa;
}

Player* ServerNetworkHandler::_getServerPlayer(uintptr_t id, uintptr_t pkt) {
	return SymCall<Player*>("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
		this, id, Dereference<char>(pkt, 16));
}
