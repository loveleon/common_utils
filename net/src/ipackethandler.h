//
// Created by cxh on 18-12-3.
//

#ifndef NET_IPACKETHANDLER_H
#define NET_IPACKETHANDLER_H

namespace net{
class IPacketHandler {
 public:
	enum HPRetCode{
		KEEP_CHANNEL = 0,
		CLOSE_CHANNEL = 1,
		FREE_CHANNEL = 2
	};
//	IPacketHandler(){}//use default constructor.
	virtual ~IPacketHandler(){}
	virtual HPRetCode handlePacket(Packet *packet, void *args) = 0;
};
}


#endif //COMMON_UTILS_IPACKETHANDLER_H
