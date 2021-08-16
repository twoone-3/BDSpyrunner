#include <NetWork.h>

std::string NetworkIdentifier::getAddress() {
	std::string str;
	SymCall<std::string&>("?getAddress@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &str);
	return str;
}

std::string NetworkIdentifier::toString() {
	std::string str;
	SymCall<std::string&>("?toString@NetworkIdentifier@@QEBA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ",
		this, &str);
	return str;
}

SystemAddress::SystemAddress() {
	SymCall("??0SystemAddress@RakNet@@QEAA@XZ", this);
}

std::string SystemAddress::toString() {
	return SymCall<const char*>("?ToString@SystemAddress@RakNet@@QEBAPEBD_ND@Z",
		this, true, ':');
}

SystemAddress RakPeer::getSystemAddress(NetworkIdentifier* ni) {
	SystemAddress sa;
	SymCall<SystemAddress&>("?GetSystemAddressFromGuid@RakPeer@RakNet@@UEBA?AUSystemAddress@2@URakNetGUID@2@@Z",
		this, &sa, ni);
	return sa;
}

Player* ServerNetworkHandler::_getServerPlayer(VA id, VA pkt) {
	return SymCall<Player*>("?_getServerPlayer@ServerNetworkHandler@@AEAAPEAVServerPlayer@@AEBVNetworkIdentifier@@E@Z",
		this, id, FETCH(char, pkt + 16));
}
