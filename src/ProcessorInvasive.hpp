//
// Created by pbe on 11.11.2025.
//

#ifndef CULIDAR_PROCESSORINVASIVE_HPP
#define CULIDAR_PROCESSORINVASIVE_HPP
#include <Processor.hpp>

namespace inno
{
	class ProcessorInvasive : public Processor
	{
	public:
		/**
		 * Get access to the data we are currently building up
		 *
		 * @return the shared pointer to the point cloud being built. May not be engaged
		 */
		CloudPointer& getNextCloud()
		{
			return nextCloud;
		}

		/**
		 * Move the new point cloud pointer to the old one and notify any listeners that a new point cloud is available
		 *
		 * This also resets the next point cloud, so it can be reused
		 */
		void notify()
		{
			// grab the lock
			{
				std::unique_lock lock(pointCloudMutex);
				pointCloud.swap(nextCloud);
				cloudAvailable.notify_all();
			}
			nextCloud.reset();
		}
	};

	static_assert(sizeof(Processor) == sizeof(ProcessorInvasive), "ProcessorInvasive size mismatch Risk of slicing!");
}
#endif //CULIDAR_PROCESSORINVASIVE_HPP