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


#ifdef _WIIMOTE_SUPPORT_

#ifndef __VRPNWIIMOTEREMOTE_H__
#define __VRPNWIIMOTEREMOTE_H__

#include "Globals/Include.h"
#include <vrpn/vrpn_Analog.h>
#include <vrpn/vrpn_Button.h>


namespace MultiKinect
{
	namespace VRPN
	{
		class VRPNWiimoteRemote
		{
		private:
			uint32 moteID_;
			vrpn_Analog_Remote* analogs_;
			vrpn_Button_Remote* buttons_;

		public:
			VRPNWiimoteRemote(uint32 moteID, const std::string& name);
			virtual ~VRPNWiimoteRemote();

			virtual void mainloop();
		};
	}
}

#endif

#endif
