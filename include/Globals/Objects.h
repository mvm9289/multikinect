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


#ifndef __OBJECTS_H__
#define __OBJECTS_H__

namespace MultiKinect
{
	namespace Globals
	{
		class Config;
	}

	namespace Files
	{
		class Filesystem;
	}

	namespace Geom
	{
		class Color;
		class Point;
		class Quaternion;
		class Matrix3x3;
		class Matrix4x4;
		class Vector;
	}

	namespace GUI
	{
		class App;
		class GLCanvas;
		class KinectCanvas;
		class TextFileDialog;
		class MainFrame;
		class MainFrameLogic;
		class MasterFrame;
		class MasterFrameLogic;
		class RenderFrame;
	}

	namespace Kinect
	{
		class KinectDevice;
		class KinectManager;
		class KinectSkeleton;
	}

	namespace Interprocess
	{
		class SharedMemoryManager;
	}

	namespace Render
	{
		class RenderSystem;
		class RenderSystemInterprocess;
		class RenderSystemLocal;
		class RenderSystemRemote;
		class RenderThread;
		class RenderTimer;
	}

	namespace Tools
	{
		class Log;
		class Timer;
	}

	namespace VRPN
	{
		class VRPNClient;
		class VRPNServer;
		class VRPNSkeletonTracker;
		class VRPNSkeletonTrackerRemote;
		class VRPNWiimote;
		class VRPNWiimoteRemote;
	}
}

#endif
