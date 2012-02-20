/*
	MultiKinect: Skeleton tracking based on multiple Microsoft Kinect cameras
	Copyright (C) 2012-2013  Miguel Angel Vico Moya

	This file is part of MultiKinect.

	MultiKinect is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __SHAREDMEMORYMANAGER_H__
#define __SHAREDMEMORYMANAGER_H__

#include "Globals/Include.h"
#include <vector>
#include <utility>
#include <boost/interprocess/managed_shared_memory.hpp>


namespace MultiKinect
{
	namespace Interprocess
	{
		class SharedMemoryManager
		{
		private:
			static bool initialized_;
			static uint32 nCreatedSegments_;
			static std::vector< std::pair<std::string, boost::interprocess::managed_shared_memory*> > createdSegments_;
			static uint32 nOpenedSegments_;
			static std::vector< std::pair<std::string, boost::interprocess::managed_shared_memory*> > openedSegments_;

			static boost::interprocess::managed_shared_memory* openSegmentIfNeeded(const std::string& segmentID);

		public:
			static void initialize();
			static void destroy();

			static bool isInitialized();

			static void createSharedSegment(const std::string& segmentID, uint32 segmentSize = 5242880);
			static void removeSharedSegment(const std::string& segmentID);

			template <typename T> static T* createSharedObject(const std::string& segmentID, const std::string& objectID, uint32 numElements = 1)
			{
				boost::interprocess::managed_shared_memory* segment = openSegmentIfNeeded(segmentID);
				if (numElements > 1) return segment->construct<T>(objectID.c_str())[numElements]();
				else return segment->construct<T>(objectID.c_str())();
			}

			template <typename T> static T* getSharedObject(const std::string& segmentID, const std::string& objectID)
			{
				boost::interprocess::managed_shared_memory* segment = openSegmentIfNeeded(segmentID);
				std::pair<T*, std::size_t> result = segment->find<T>(objectID.c_str());
				return result.first;
			}

			template <typename T> static void removeSharedObject(const std::string& segmentID, const std::string& objectID)
			{
				boost::interprocess::managed_shared_memory* segment = openSegmentIfNeeded(segmentID);
				segment->destroy<T>(objectID.c_str());
			}
		};
	}
}

#endif
