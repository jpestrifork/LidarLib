//
// Created by pbe on 10.11.2025.
//

#ifndef CULIDAR_PROCESSORCALLBACKS_HPP
#define CULIDAR_PROCESSORCALLBACKS_HPP

#include <sdk_client/lidar_client.h>

namespace inno
{

	void messageCallback(int handle, void* context, uint32_t fromRemote,
	                     InnoMessageLevel level, InnoMessageCode code, const char* errorMessage);

	int dataPacketCallback(int lidar_handle, void* ctx, const InnoDataPacket* pkt);
	int statusPacketCallback(int lidar_handle, void* ctx, const InnoStatusPacket* pkt);
	// double hostTimeCallback();
	int setupCallbacks(int handle, void* ctx);

}
#endif //CULIDAR_PROCESSORCALLBACKS_HPP