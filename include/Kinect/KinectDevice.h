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


#ifndef __KINECTDEVICE_H__
#define __KINECTDEVICE_H__

#include "Globals/Include.h"
#include "Geom/Color.h"
#include <vector>
#include <Windows.h>
#include <NuiApi.h>


namespace MultiKinect
{
	namespace Kinect
	{
		class KinectDevice
		{
		public:
			enum KinectUsage
			{
				K_USE_COLOR = 0x00000002, /*NUI_INITIALIZE_FLAG_USES_COLOR*/
				K_USE_DEPTH = 0x00000020, /*NUI_INITIALIZE_FLAG_USES_DEPTH*/
				K_USE_DEPTH_AND_PLAYER = 0x00000001, /*NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX*/
				K_USE_SKELETON = 0x00000008 /*NUI_INITIALIZE_FLAG_USES_SKELETON*/
			};

			enum KinectStatusCode
			{
				K_DEVICE_OK = 0,
				K_DEVICE_IN_USE,
				K_DEVICE_INITIALIZING,
				K_DEVICE_NOT_CONNECTED,
				K_DEVICE_NOT_VALID,
				K_DEVICE_UNSUPPORTED,
				K_DEVICE_INSUFFICIENT_BANDWIDTH,
				K_DEVICE_NOT_POWERED,
				K_DEVICE_NOT_READY,
				K_DEVICE_UNKNOWN
			};

			enum KinectError
			{
				K_ERROR_SKELETON_IN_USE = 0x0FFFFFFF,
				K_ERROR_SKELETON_TRACKING = 0x1FFFFFFF,
				K_ERROR_DEVICE_IN_USE = 0x2FFFFFFF,
				K_ERROR_INVALID_DEVICE = 0x3FFFFFFF,
				K_ERROR_DEVICE_ALREADY_INITIALIZED = 0x4FFFFFFF,
				K_ERROR_UNKNOWN = 0xFFFFFFFF
			};

		private:
			bool valid_;
			bool initialized_;
			INuiSensor* instance_;
			bool colorStreamOpened_;
			bool depthStreamOpened_;
			int32 depthFlags_;
			bool skeletonEnabled_;
			int32 skeletonFlags_;
			HANDLE colorStreamHandle_;
			HANDLE depthStreamHandle_;
			HANDLE colorFrameEvent_;
			HANDLE depthFrameEvent_;
			HANDLE skeletonFrameEvent_;
			HANDLE processStopEvent_;
			HANDLE processThread_;
			uint8* colorFrame_;
			uint8* depthFrame_;
			uint32* nSkeletons_;
			std::vector<int32> skeletonMap_;
			KinectSkeleton* skeletons_;
			int32 elevationAngle_;
			float32* rotationX_;
			float32* rotationY_;
			float32* rotationZ_;
			float32* translationX_;
			float32* translationY_;
			float32* translationZ_;
			float32 colorFPS_;
			float32 depthFPS_;
			float32 skeletonFPS_;
			bool hierarchicalOri_;

			void create();
			void obtainColorFrame();
			void obtainDepthFrame();
			void obtainSkeletonsFrame();
			Color depthToColor(uint16 depthValue, bool usesPlayer);

		public:
			KinectDevice(uint32 index);
			KinectDevice(const std::string& deviceID);
			virtual ~KinectDevice();

			bool isValid();
			bool isInitialized();
			KinectStatusCode getStatus();
			std::string getID();
			int32 initialize(int32 flags, bool seatedMode = false, bool nearMode = false);
			void shutDown();
			void setSeatedMode(bool seated);
			void setNearMode(bool nearMode);
			void setHierarchicalOri(bool hierarchical);
			uint8* getColorFrame();
			uint8* getDepthFrame();
			uint32 getNumberOfSkeletons();
			KinectSkeleton* getSkeletons();
			bool getRotation(float32& rotationX, float32& rotationY, float32& rotationZ);
			bool getTranslation(float32& translationX, float32& translationY, float32& translationZ);
			int32 getElevationAngle();
			int32 getMinElevationAngle();
			int32 getMaxElevationAngle();
			void recomputeElevationAngleLimits();
			void setElevationAngle(int32 angle);
			float32 getFPS();

			static DWORD WINAPI processThread(LPVOID param);
			void processThread();
		};
	}
}

#endif
