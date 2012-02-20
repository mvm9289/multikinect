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


#include "Interprocess/SharedMemoryManager.h"

#include "Tools/Log.h"

using namespace MultiKinect;
using namespace Interprocess;
using namespace Tools;
using namespace boost::interprocess;


bool SharedMemoryManager::initialized_ = false;
uint32 SharedMemoryManager::nCreatedSegments_ = 0;
std::vector< std::pair<std::string, managed_shared_memory*> > SharedMemoryManager::createdSegments_;
uint32 SharedMemoryManager::nOpenedSegments_ = 0;
std::vector< std::pair<std::string, managed_shared_memory*> > SharedMemoryManager::openedSegments_;

void SharedMemoryManager::initialize()
{
	if (!initialized_)
	{
		nCreatedSegments_ = 0;
		createdSegments_.clear();
		nOpenedSegments_ = 0;
		openedSegments_.clear();
		initialized_ = true;
	}
	else Log::write("[SharedMemoryManager] initialize()", "ERROR: SharedMemoryManager already initialized.");
}

void SharedMemoryManager::destroy()
{
	if (initialized_)
	{
		for (uint32 i = 0; i < nCreatedSegments_; i++)
		{
			shared_memory_object::remove(createdSegments_[i].first.c_str());
			delete createdSegments_[i].second;
		}
		nCreatedSegments_ = 0;
		createdSegments_.clear();

		for (uint32 i = 0; i < nOpenedSegments_; i++)
			delete openedSegments_[i].second;
		nOpenedSegments_ = 0;
		openedSegments_.clear();
		initialized_ = false;
	}
	else Log::write("[SharedMemoryManager] destroy()", "ERROR: SharedMemoryManager not initialized.");
}

bool SharedMemoryManager::isInitialized()
{
	return initialized_;
}

managed_shared_memory* SharedMemoryManager::openSegmentIfNeeded(const std::string& segmentID)
{
	if (initialized_)
	{
		boost::interprocess::managed_shared_memory* segment = 0;
		bool found = false;
		for (uint32 i = 0; i < nOpenedSegments_ && !found; i++)
		{
			if (openedSegments_[i].first == segmentID)
			{
				segment = openedSegments_[i].second;
				found = true;
			}
		}
		if (!found)
		{
			segment = new managed_shared_memory(open_only, segmentID.c_str());
			nOpenedSegments_++;
			openedSegments_.push_back(std::pair<std::string, managed_shared_memory*>(segmentID, segment));
		}
		return segment;
	}
	else
	{
		Log::write("[SharedMemoryManager] openSegmentIfNeeded()", "ERROR: SharedMemoryManager not initialized.");
		return 0;
	}
}

void SharedMemoryManager::createSharedSegment(const std::string& segmentID, uint32 segmentSize)
{
	if (initialized_)
	{
		shared_memory_object::remove(segmentID.c_str());
		managed_shared_memory* segment = new managed_shared_memory(create_only, segmentID.c_str(), segmentSize);
		nCreatedSegments_++;
		createdSegments_.push_back(std::pair<std::string, managed_shared_memory*>(segmentID, segment));
	}
	else Log::write("[SharedMemoryManager] createSharedSegment()", "ERROR: SharedMemoryManager not initialized.");
}

void SharedMemoryManager::removeSharedSegment(const std::string& segmentID)
{
	if (initialized_)
	{
		bool found = false;
		for (int32 i = nCreatedSegments_ - 1; i >= 0 && !found; i--)
		{
			if (createdSegments_[i].first == segmentID)
			{
				shared_memory_object::remove(segmentID.c_str());
				delete createdSegments_[i].second;
				nCreatedSegments_--;
				createdSegments_.erase(createdSegments_.begin() + i);
				found = true;
			}
		}
	}
	else Log::write("[SharedMemoryManager] removeSharedSegment()", "ERROR: SharedMemoryManager not initialized.");
}
