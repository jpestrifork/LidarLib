//
// Created by pbe on 13.11.2025.
//

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/eigen/dense.h>
#include <Lidar.hpp>

using namespace nanobind::literals;
NB_MODULE( inno, m )
{
	m.def("api_version", &Lidar::api_version, "Return the API version");
	nanobind::class_<Lidar>(m, "Lidar")
		.def(nanobind::init<const std::string, uint16_t, uint16_t>(), "ip"_a, "comms_port"_a, "udp_port"_a)
		.def("get_point_cloud", &Lidar::getPointCloud, nanobind::rv_policy::reference);
}