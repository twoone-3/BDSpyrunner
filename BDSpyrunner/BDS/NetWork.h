#pragma once
#include "../tool.h"

struct NetworkIdentifier {
	string getAddress();
	string toString();
};
struct SystemAddress {
	char _this[132];
	SystemAddress();
	string toString();
};
struct RakPeer {
	SystemAddress getSystemAddress(NetworkIdentifier* ni);
};
struct ServerNetworkHandler {
	struct Player* _getServerPlayer(VA id, VA pkt);
};
