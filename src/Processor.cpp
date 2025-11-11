//
// Created by pbe on 07.11.2025.
//

#include "ProcessorInvasive.hpp"
#include "ProcessorCallbacks.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <sdk_client/lidar_client.h>


namespace inno
{
	Processor::Processor(const std::string& ip, const std::uint16_t commsPort, const std::uint16_t udpPort)
	{
		handle = inno_lidar_open_live("cuLiDAR", ip.c_str(), commsPort, INNO_LIDAR_PROTOCOL_PCS_UDP, udpPort);
		setupCallbacks(handle, this);
		inno_lidar_start(handle);
	}

	Processor::Processor(Processor&& processor) noexcept
	{
		*this = std::move(processor);
	}

	Processor& Processor::operator=(Processor&& processor) noexcept
	{
		this->handle = processor.handle;
		processor.handle = 0;
		return *this;
	}

	Processor::~Processor()
	{
		if (inno_lidar_stop(handle))
		{
			// write some error message
		}
		if (inno_lidar_close(handle))
		{
			// write some error message
		}
	}

	Processor::CloudPointer Processor::getPointCloud(const std::chrono::milliseconds waitTime)
	{
		std::unique_lock lock(pointCloudMutex);
		if (const auto errorOut = cloudAvailable.wait_for(lock, waitTime); errorOut != std::cv_status::timeout)
		{
			return pointCloud;
		}
		return {};
	}

	void messageCallback(int /*handle*/, void* /*context*/, uint32_t /*fromRemote*/,
		                 const InnoMessageLevel level, InnoMessageCode /*code*/, const char* errorMessage)
	{
		fmt::print("Message for you sir: ({}) {}\n", static_cast<int>(level), errorMessage);
	}

	template <typename PointerType>
	uint32_t insertHelper(const InnoDataPacket* const pkt, const Processor::CloudPointer& pc, PointerType* point )
	{
		ProcessorInvasive::PointCloud addition;
		addition.reserve(pkt->item_number);

		uint32_t i = 0;
		for (; i < pkt->item_number; i++)
		{
			pcl::PointXYZ pt;
			pt.x = point[i].x;
			pt.y = point[i].y;
			pt.z = point[i].z;
			addition.push_back(pt);
		}
		pc->insert(pc->end(), addition.begin(), addition.end());
		return i;
	}

	int dataPacketCallback(int /*lidar_handle*/, void* ctx, const InnoDataPacket* pkt)
	{
		// It seems every package that is the last will contain no packets, this can be used to finish off and start anew
		const auto context = static_cast<ProcessorInvasive*>(ctx);
		if (pkt->item_number == 0)
		{
			context->notify();
		}
		else
		{
			auto nc = context->getNextCloud();
			if (pkt->sub_idx == 0)
			{
				// We've started a new frame
				const ProcessorInvasive::PointCloud pc;
				// We should probably make sure it is not engaged
				nc = pc.makeShared();
			}
			if (pkt->type == INNO_ITEM_TYPE_XYZ_POINTCLOUD)
			{
				const auto point = pkt->xyz_points;
				auto inserted = insertHelper(pkt, nc, point);
				fmt::print("Inserted {:d} datapoints\n", inserted);
			}
			else if (CHECK_EN_XYZ_POINTCLOUD_DATA(pkt->type))
			{
				const auto point = pkt->en_xyz_points;
				auto inserted = insertHelper(pkt, nc, point);
				fmt::print("Inserted {:d} datapoints\n", inserted);
			}
			else
			{
				fmt::print(stderr, "Unsupported PointCloudData {:d}\n",pkt->type);
			}
		}
		fmt::print("Received frame {:d}, subframe {:d} with {:d} entries\n", pkt->idx, pkt->sub_idx, pkt->item_number);
		return 0;
	}

	int statusPacketCallback(int /*lidar_handle*/, void* /*ctx*/, const InnoStatusPacket* pkt)
	{
		fmt::print("Got status from LiDAR with S/N {}\n", pkt->sn);
		return 0;
	}

	int setupCallbacks(const int handle, void* ctx)
	{
		return inno_lidar_set_callbacks(handle, messageCallback, dataPacketCallback, statusPacketCallback, nullptr,
		                                ctx);
	}
}// inno