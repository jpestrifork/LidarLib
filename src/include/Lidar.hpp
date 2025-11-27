//
// Created by pbe on 14.11.2025.
//

#ifndef CULIDAR_LIDAR_HPP
#define CULIDAR_LIDAR_HPP
#include <Processor.hpp>

class Lidar
{
public:
	Lidar(std::string ip, uint16_t port, uint16_t udp)
		: processor(ip, port, udp)
	{}
	Eigen::Map<Eigen::MatrixXf, Eigen::Aligned, Eigen::OuterStride<> > getPointCloud();
	static std::string api_version()
	{
		return inno::Processor::getVersion();
	}
private:
	inno::Processor processor;
};


#endif //CULIDAR_LIDAR_HPP