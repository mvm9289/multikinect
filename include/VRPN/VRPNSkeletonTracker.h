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


#ifndef __VRPNSKELETONTRACKER_H__
#define __VRPNSKELETONTRACKER_H__

#include "Globals/Include.h"
#include <vrpn/vrpn_Tracker.h>


namespace MultiKinect
{
	namespace VRPN
	{
		class VRPNSkeletonTracker : public vrpn_Tracker
		{
		private:
			uint32								skeletonID_;
			int32								kinectID_;
			std::vector<VRPNSkeletonTracker*>	subtrackers_;

			VRPNSkeletonTracker(const std::string& name, vrpn_Connection* c = 0);

			void init(uint32 skeletonID, uint32 kinectID, const std::string& name);

		public:
			VRPNSkeletonTracker(uint32 skeletonID, const std::string& name, vrpn_Connection* c = 0);
			virtual ~VRPNSkeletonTracker();

			virtual void mainloop();
		};
	}
}

#endif
