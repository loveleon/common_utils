//
// Created by cxh on 18-12-3.
//

#ifndef NET_IPACKETFACTORY_H
#define NET_IPACKETFACTORY_H

namespace net{
class IPacketFactory {
 public:
	virtual ~IPacketFactory(){}
	Packet* createPacket(int pcode) = 0;
};
}


#endif //COMMON_UTILS_IPACKETFACTORY_H
