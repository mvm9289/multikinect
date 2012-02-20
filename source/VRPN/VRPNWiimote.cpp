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

#include "VRPN/VRPNWiimote.h"

#include "Tools/Log.h"

using namespace MultiKinect;
using namespace Tools;
using namespace VRPN;


VRPNWiimote::VRPNWiimote(uint32 moteID, const std::string& name, vrpn_Connection* c) : vrpn_WiiMote(name.c_str(), c, moteID, false, false, false)
{
	std::string message = "Wiimote " + basic_cast<std::string>(moteID) + " initialized on " + name;
	Log::write("[VRPNWiimote] VRPNWiimote()", message);
}

VRPNWiimote::~VRPNWiimote()
{}

#endif
