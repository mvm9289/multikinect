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


#include "GUI/App.h"

#include "Globals/Types.h"
#include "Globals/Config.h"
#include "GUI/MainFrameLogic.h"
#include "GUI/MasterFrameLogic.h"
#include "Kinect/KinectDevice.h"
#include "Kinect/KinectManager.h"
#include "Interprocess/SharedMemoryManager.h"
#include "Render/RenderSystem.h"
#include "Tools/Log.h"
#include "Tools/Timer.h"
#include "VRPN/VRPNServer.h"
#include <cstdlib>
#include <ctime>

using namespace MultiKinect;
using namespace Globals;
using namespace GUI;
using namespace Kinect;
using namespace Interprocess;
using namespace Render;
using namespace Tools;
using namespace VRPN;


IMPLEMENT_APP(App)

bool App::OnInit()
{
	srand(time(0));
	Timer::startCount("executionStart");

	// Parse the arguments list
	for (int32 i = 0; i < argc; i++)
	{
		std::string arg(wxString(*argv).mb_str());
		if		(arg.find("-D") == 0)	Globals::INSTANCE_ID		= arg.substr(2);
		else if	(arg.find("-LC") == 0)	Globals::LAST_CONFIGURATION	= arg.substr(3);
		argv++;
	}

	// Initialize the application
	Config::initialize();

	switch(Config::system.currentMode)
	{
	case Config::KINECT_MASTER:
		Globals::INSTANCE_ID = "master";
		Log::initialize();
		KinectManager::initialize();
		SharedMemoryManager::initialize();

		if (KinectManager::getNumberOfDevices())
		{
			for (uint32 i = 0; i < KinectManager::getNumberOfDevices(); i++)
			{
				KinectDevice	device			=	KinectManager::getDeviceObject(i);
				std::string		slaveDeviceID	=	KinectManager::reformatDeviceID(device.getID());

				SharedMemoryManager::createSharedSegment(slaveDeviceID);
				slavesIDs.push_back(slaveDeviceID);

				std::string command = "MultiKinect.exe -LC\"" + Globals::LAST_CONFIGURATION + "\" -D" + device.getID();
				WinExec(command.c_str(), SW_SHOW);

				Log::write("[App] onInit()", "Execute: " + command);

				Timer::wait(1000);
			}
		}
		else Log::write("[App] onInit()", "ERROR: There are not available devices.");

		RenderSystem::initialize(RenderSystem::RS_INTERPROCESS);
		VRPNServer::initialize();
		break;

	case Config::KINECT_SWITCHER:
		Log::initialize("Switcher");
		KinectManager::initialize();
		if (KinectManager::getNumberOfDevices())
		{
			KinectDevice* device = KinectManager::getDevicePointer(0);
			Globals::INSTANCE_ID = device->getID();
			RenderSystem::initialize(RenderSystem::RS_LOCAL_DEVICE, device);
		}
		else Log::write("[App] onInit()", "ERROR: There are not connected devices.");
		VRPNServer::initialize();
		break;

	case Config::KINECT_SINGLE_DEVICE:
		Log::initialize(KinectManager::reformatDeviceID(Globals::INSTANCE_ID));
		KinectManager::initialize();
		SharedMemoryManager::initialize();
		if (KinectManager::isValidDeviceID(Globals::INSTANCE_ID))
			RenderSystem::initialize(RenderSystem::RS_LOCAL_DEVICE, KinectManager::getDevicePointer(Globals::INSTANCE_ID));
		else Log::write("[App] onInit()", "ERROR: Invalid device ID.");
		break;

	case Config::KINECT_CLIENT:
		Globals::INSTANCE_ID = "client";
		Log::initialize("Client");
		RenderSystem::initialize(RenderSystem::RS_REMOTE_DEVICE);
		break;

	default: break;
	}

	RenderFrame* frame = 0;
	switch(Config::system.currentMode)
	{
	case Config::KINECT_MASTER:
	case Config::KINECT_CLIENT:
		frame = new MasterFrameLogic((wxFrame*)0);
		break;
	case Config::KINECT_SWITCHER:
	case Config::KINECT_SINGLE_DEVICE:
		frame = new MainFrameLogic((wxFrame*)0);
		break;
	default: break;
	}

	if (frame)
	{
		frame->Show();
		return true;
	}
	else return false;
}

int App::OnExit()
{
	// In case of KINECT_MASTER mode, it must close the slave processes
	if (Config::system.currentMode == Config::KINECT_MASTER)
	{
		// Send exit message to every slave process
		uint32 nSlaves = basic_cast<uint32>(slavesIDs.size());
		for (uint32 i = 0; i < nSlaves; i++)
		{
			SharedMemoryManager::createSharedObject<bool>(slavesIDs[i], "CLOSE_SIGNAL");

			// Wait for slave exit
			bool* flag = 0;
			Timer::startCount("timeout");
			do flag = SharedMemoryManager::getSharedObject<bool>(slavesIDs[i], "CLOSE_SIGNAL");
			while (flag && Timer::getCountTime("timeout") <= 1000);

			if (flag)
			{
				SharedMemoryManager::removeSharedObject<bool>(slavesIDs[i], "CLOSE_SIGNAL");
				std::string message = "ERROR: Exit of slave process " + slavesIDs[i] + " timeout.";
				Log::write("[App] OnExit()", message);
			}
		}
	}

	// Destroy the application
	if (VRPNServer::isInitialized())			VRPNServer::destroy();
	if (RenderSystem::isInitialized())			RenderSystem::destroy();
	if (SharedMemoryManager::isInitialized())	SharedMemoryManager::destroy();
	if (KinectManager::isInitialized())			KinectManager::destroy();
	if (Log::isInitialized())					Log::destroy();
	if (Config::isInitialized())				Config::destroy();

	Timer::endCount("executionStart");

	// Restart the process if user have ordered it
	if (Config::system.restartApp)
	{
		Timer::wait(1000);
		std::string command = "MultiKinect.exe -LC\"" + Globals::LAST_CONFIGURATION + "\"";
		if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE) command += (" -D" + Globals::INSTANCE_ID);
		WinExec(command.c_str(), SW_SHOW);
	}

	return 0;
}
