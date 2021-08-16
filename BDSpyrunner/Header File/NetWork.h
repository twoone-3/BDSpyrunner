#pragma once
#include <string>
#include <tool.h>

struct NetworkIdentifier {
	std::string getAddress();
	std::string toString();
};
struct SystemAddress {
	char _this[132];
	SystemAddress();
	std::string toString();
};
struct RakPeer {
	SystemAddress getSystemAddress(NetworkIdentifier* ni);
};
struct ServerNetworkHandler {
	struct Player* _getServerPlayer(VA id, VA pkt);
};
