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

#include "VRPN/VRPNWiimoteRemote.h"

#include "Tools/Log.h"

using namespace MultiKinect;
using namespace Tools;
using namespace VRPN;


void VRPN_CALLBACK handle_analog(void* userData, const vrpn_ANALOGCB a)
{
	// Do nothing
}

void VRPN_CALLBACK handle_button(void* userData, const vrpn_BUTTONCB b)
{
	uint32 moteID = *basic_cast<uint32*>(userData);
	std::string message1 = "[VRPNWiimoteRemote] Wiimote " + basic_cast<std::string>(moteID);
	std::string message2 = "Button " + basic_cast<std::string>(b.button) + (b.state ? " pressed" : " released");
	Log::write(message1, message2);
}

VRPNWiimoteRemote::VRPNWiimoteRemote(uint32 moteID, const std::string& name)
{
	moteID_ = moteID;
	analogs_ = new vrpn_Analog_Remote(name.c_str());
	buttons_ = new vrpn_Button_Remote(name.c_str());
	analogs_->register_change_handler(&moteID_, handle_analog);
	buttons_->register_change_handler(&moteID_, handle_button);
	std::string message = "Wiimote " + basic_cast<std::string>(moteID_) + " initialized on " + name;
	Log::write("[VRPNWiimoteRemote] VRPNWiimoteRemote()", message);
}

VRPNWiimoteRemote::~VRPNWiimoteRemote()
{
	analogs_->unregister_change_handler(&moteID_, handle_analog);
	buttons_->unregister_change_handler(&moteID_, handle_button);
	delete analogs_;
	delete buttons_;
}

void VRPNWiimoteRemote::mainloop()
{
	analogs_->mainloop();
	buttons_->mainloop();
}

#endif
