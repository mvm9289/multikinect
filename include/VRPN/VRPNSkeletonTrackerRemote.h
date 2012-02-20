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


#ifndef __VRPNSKELETONTRACKERREMOTE_H__
#define __VRPNSKELETONTRACKERREMOTE_H__

#include "Globals/Include.h"
#include <vrpn/vrpn_Tracker.h>


namespace MultiKinect
{
	namespace VRPN
	{
		class VRPNSkeletonTrackerRemote : public vrpn_Tracker_Remote
		{
		private:
			uint32		skeletonID_;
			VRPNClient	*notifier_;
			std::string	timeoutId_;

		public:
			VRPNSkeletonTrackerRemote(uint32 skeletonID, const std::string& name, VRPNClient* notifier);
			virtual ~VRPNSkeletonTrackerRemote();

			void notify(uint32 sensor, const float64* pos, const float64* quat) const;
			bool isValid() const;
		};
	}
}

#endif
