#include <Lidar.hpp>


Eigen::Map<Eigen::MatrixXf, Eigen::Aligned, Eigen::OuterStride<>> Lidar::getPointCloud()
{
	return processor.getPointCloud()->getMatrixXfMap(3,4,0);
}