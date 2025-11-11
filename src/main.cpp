//
// Created by pbe on 07.11.2025.
//

#include <argparse/argparse.hpp>
#include <fmt/format.h>
#include <Processor.hpp>
#include <csignal>

std::atomic_bool interrupted = false;

void interruptHandler(int /*signal*/)
{
	interrupted = true;
}

int main(int argc, char** argv)
{
	std::signal(SIGINT, interruptHandler);

	argparse::ArgumentParser parser;

	parser.add_argument("-i", "--ip")
	      .help("IPv4 address of the LiDAR")
	      .default_value("172.168.1.10");
	parser.add_argument("-p", "--port")
	      .help("UDP port of the LiDAR")
	      .default_value<uint16_t>(8010)
	      .scan<'i', uint16_t>();
	parser.add_argument("-c", "--comms")
	      .help("LiDAR communication port")
	      .default_value<uint16_t>(8010)
	      .scan<'i', uint16_t>();

	uint16_t comms{};
	uint16_t port{};

	try
	{
		parser.parse_args(argc, argv);
		port = parser.get<uint16_t>("--port");
		comms = parser.get<uint16_t>("--comms");
	}
	catch (const std::exception& e)
	{
		fmt::print(stderr, "Error: {} \n{}\n", e.what(), parser.usage());
	}

	inno::Processor processor(parser.get("--ip"), comms, port);

	while (not interrupted)
	{
		if (const auto points = processor.getPointCloud())
		{
			fmt::print("Got a point cloud with {:d} coordinates in it", "{:d}", points->size());
		}
		else
		{
			fmt::print(stderr, "Seems like we timed out, let's try again");
		}
	}
}