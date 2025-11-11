//
// Created by pbe on 07.11.2025.
//

#ifndef CULIDAR_PROCESSOR_HPP
#define CULIDAR_PROCESSOR_HPP
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <string>

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>

namespace inno
{
	class Processor
	{
	public:
		Processor(const std::string& ip, std::uint16_t commsPort, std::uint16_t udpPort );
		Processor( const Processor& processor ) = delete;
		Processor( Processor&& processor) noexcept;
		Processor& operator=( const Processor& processor ) = delete;
		Processor& operator=( Processor&& processor) noexcept ;
		~Processor();

		using PointCloud = pcl::PointCloud<pcl::PointXYZ>;
		using CloudPointer = PointCloud::Ptr;
		CloudPointer getPointCloud(std::chrono::milliseconds waitTime = std::chrono::seconds(1));
	protected:
		int handle{};
		CloudPointer pointCloud{};
		CloudPointer nextCloud{};
		std::condition_variable cloudAvailable;
		std::mutex pointCloudMutex;
	};
} // inno

#endif //CULIDAR_PROCESSOR_HPP