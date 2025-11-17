//
// Created by pbe on 07.11.2025.
//

#include "ProcessorInvasive.hpp"
#include "ProcessorCallbacks.hpp"

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <sdk_client/lidar_client.h>

#include <span>


namespace inno
{
	Processor::Processor(const std::string& ip, const std::uint16_t commsPort, const std::uint16_t udpPort)
	{
		handle = inno_lidar_open_live("cuLiDAR", ip.c_str(), commsPort, INNO_LIDAR_PROTOCOL_PCS_UDP, udpPort);
		setupCallbacks(handle, this);
		// This should probably be a parameter, if not set, seems like it uses spherical coordinates instead
		inno_lidar_set_callbacks_data_type(handle, INNO_CALLBACK_XYZ_FRAME);
		if (inno_lidar_start(handle) != 0)
		{
			fmt::println(stderr,"Could not start lidar \"{:s}\"\nStart returned {:d}", "cuLiDAR", handle);
			if (handle > 0)
			{
				inno_lidar_close(handle);
			}
			throw std::runtime_error("Could not start LiDAR");
		}
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
		//fmt::print("Message for you sir: ({}) {}\n", static_cast<int>(level), errorMessage);
	}

	template<typename PointerType>
	uint32_t insertHelper(const InnoDataPacket* const pkt, const Processor::CloudPointer& pc, PointerType* point)
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
		auto nc = context->getNextCloud();
		if (pkt->is_last_sub_frame == 1)
		{
			if (nc)
			{
				context->notify();
			}
		}
		if (pkt->is_first_sub_frame == 1)
		{
			const ProcessorInvasive::PointCloud pc;
			// We should probably make sure it is not engaged
			context->getNextCloud() = pc.makeShared();
			nc = context->getNextCloud();
		}
		if (nc)
		{
			switch (pkt->type)
			{
				case INNO_ITEM_TYPE_XYZ_POINTCLOUD:
				{
					const auto point = pkt->xyz_points;
					auto inserted = insertHelper(pkt, nc, point);
				}
				break;

				case INNO_ROBINW_ITEM_TYPE_XYZ_POINTCLOUD:
				case INNO_FALCONII_DOT_1_ITEM_TYPE_XYZ_POINTCLOUD:
				case INNO_ROBINELITE_ITEM_TYPE_XYZ_POINTCLOUD:
				case INNO_ROBINE2_ITEM_TYPE_XYZ_POINTCLOUD:
				{
					const auto point = pkt->en_xyz_points;
					auto inserted = insertHelper(pkt, nc, point);
				}
				break;

				case INNO_ITEM_TYPE_SPHERE_POINTCLOUD:
				{
					auto inno_blocks = pkt->inno_block1s;
					for (uint32_t i = 0; i < pkt->item_number; i++)
					{
						const auto blk = inno_blocks[i];
						auto v = blk.header.v_angle * kRadPerInnoAngleUnit;
						auto h = blk.header.h_angle * kRadPerInnoAngleUnit;
						auto r = blk.points[0].radius;
						pcl::PointXYZ point{};
						point.x = r * sin(v) * cos(h);
						point.y = r * sin(v) * sin(h);
						point.z = r * cos(v);
						nc->push_back(point);
					}
				}
				break;

				default:
				{
					fmt::print(stderr, "Unsupported PointCloudData {:d}\n", pkt->type);
				}
			}
		}
		return 0;
	}

	int statusPacketCallback(int /*lidar_handle*/, void* /*ctx*/, const InnoStatusPacket* pkt)
	{
		//fmt::print("Got status from LiDAR with S/N {}\n", pkt->sn);
		return 0;
	}

	int setupCallbacks(const int handle, void* ctx)
	{
		return inno_lidar_set_callbacks(handle, messageCallback, dataPacketCallback, statusPacketCallback, nullptr,
		                                ctx);
	}
}// inno