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


#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "Globals/Include.h"
#include "Geom/Point.h"
#include "Geom/Vector.h"
#include <tinyxml2.h>
#include <map>


namespace MultiKinect
{
	namespace Globals
	{
		class Config
		{
		public:
			/*
			** Useful constants
			*/
			enum ApplicationMode
			{
				KINECT_MASTER = 0,
				KINECT_SWITCHER,
				KINECT_CLIENT,
				KINECT_SINGLE_DEVICE
			};

			enum RenderLoopMode
			{
				RENDER_TIMER = 0,
				IDLE_EVENTS,
				RENDER_THREAD
			};
			
			static const ApplicationMode	DEFAULT_APP_MODE;
			static const std::string		DEFAULT_APPDATA_PATH;
			static const std::string		DEFAULT_CONFIG_PATH;
			static const std::string		DEFAULT_LOG_PATH;
			static const RenderLoopMode		DEFAULT_RENDER_LOOP_MODE;
			static const uint32				DEFAULT_CANVAS_WIDTH;
			static const uint32				DEFAULT_CANVAS_HEIGHT;
			static const bool				DEFAULT_SHOW_RGB_IMAGE;
			static const bool				DEFAULT_SHOW_DEPTH_MAP;
			static const bool				DEFAULT_SHOW_SKELETON_TRACKING;
			static const bool				DEFAULT_SHOW_SKELETONS;
			static const bool				DEFAULT_SHOW_ORIENTATIONS;
			static const bool				DEFAULT_COMBINE_SKELETONS;
			static const bool				DEFAULT_ENABLED_RGB_IMAGE;
			static const bool				DEFAULT_ENABLED_DEPTH_MAP;
			static const bool				DEFAULT_ENABLED_SKELETON_TRACKING;
			static const bool				DEFAULT_ENABLED_NEAR_MODE;
			static const bool				DEFAULT_ENABLED_SEATED_MODE;
			static const bool				DEFAULT_ENABLED_HIERARCHICAL_ORI;
			static const float32			DEFAULT_KINECT_TRANSLATION_X;
			static const float32			DEFAULT_KINECT_TRANSLATION_Y;
			static const float32			DEFAULT_KINECT_TRANSLATION_Z;
			static const float32			DEFAULT_KINECT_ROTATION_X;
			static const float32			DEFAULT_KINECT_ROTATION_Y;
			static const float32			DEFAULT_KINECT_ROTATION_Z;
			static const float32			DEFAULT_ROOM_ORIGIN_X;
			static const float32			DEFAULT_ROOM_ORIGIN_Y;
			static const float32			DEFAULT_ROOM_ORIGIN_Z;
			static const float32			DEFAULT_ROOM_WIDTH;
			static const float32			DEFAULT_ROOM_HEIGHT;
			static const float32			DEFAULT_ROOM_DEPTH;
			static const std::string		DEFAULT_VRPN_SKELETON_BASE_ADDR;
			static const bool				DEFAULT_VRPN_SEND_ORIENTATIONS;

#ifdef _WIIMOTE_SUPPORT_
			static const std::string		DEFAULT_VRPN_WIIMOTE_BASE_ADDR;
#endif

			static const std::string		DEFAULT_VRPN_SERVER_ADDR;
			static const float32			DEFAULT_CONFIDENCE_MARGIN;
			static const std::string		DEFAULT_CONFIG_FILE;

			/*
			** Useful structures
			*/
			struct SystemSettings
			{
				std::string		appTitle;
				ApplicationMode	appMode;
				ApplicationMode	currentMode;
				std::string		appDataPath;
				std::string		configPath;
				std::string		logPath;
				RenderLoopMode	renderMode;
				bool			restartApp;

				SystemSettings();
			};

			struct CanvasSettings
			{
				std::string	alias;
				uint32		width;
				uint32		height;
				bool		rgbImage;
				bool		depthMap;
				bool		skeletonTracking;
				bool		showSkeletons;
				bool		showOrientations;
				bool		combineSkeletons;

				CanvasSettings();
			};

			struct KinectSettings
			{
				std::string		alias;
				bool			rgbImage;
				bool			depthMap;
				bool			skeletonTracking;
				bool			nearMode;
				bool			seatedMode;
				bool            hierarchicalOri;
				Vector			translation;
				Vector			rotation;
				int32			minElevationAngle;
				int32			maxElevationAngle;

				KinectSettings();
			};

			struct VirtualRoomSettings
			{
				Point		origin;
				float32		width;
				float32		height;
				float32		depth;

				VirtualRoomSettings();
			};

			struct VRPNSkeletonSettings
			{
				bool		enabled;
				std::string	address;
				std::string	serverAddress;
				bool		sendOrientations;
				bool		sendOriginalSkeletons;

				VRPNSkeletonSettings();
			};

#ifdef _WIIMOTE_SUPPORT_
			struct VRPNWiimoteSettings
			{
				bool		enabled;
				std::string	address;
				std::string	serverAddress;

				VRPNWiimoteSettings();
			};
#endif

			struct OtherSettings
			{
				float32	confidenceMargin;

				OtherSettings();
			};

			typedef std::map<std::string, CanvasSettings>			CanvasSettingsMap;
			typedef std::map<std::string, CanvasSettings>::iterator	CanvasSettingsMapIterator;

			typedef std::map<std::string, KinectSettings>			KinectSettingsMap;
			typedef std::map<std::string, KinectSettings>::iterator	KinectSettingsMapIterator;

		private:
			static bool initialized_;

			static void loadSystemSettings(const tinyxml2::XMLElement* parentElement);
			static void loadCanvasSettings(const tinyxml2::XMLElement* parentElement);
			static void loadKinectSettings(const tinyxml2::XMLElement* parentElement);
			static void loadRoomSettings(const tinyxml2::XMLElement* parentElement);
			static void loadLocalVRPNSkeletonsSettings(const tinyxml2::XMLElement* parentElement);
			static void loadRemoteVRPNSkeletonsSettings(const tinyxml2::XMLElement* parentElement);

#ifdef _WIIMOTE_SUPPORT_
			static void loadLocalVRPNWiimotesSettings(const tinyxml2::XMLElement* parentElement);
			static void loadRemoteVRPNWiimotesSettings(const tinyxml2::XMLElement* parentElement);
#endif
			
			static void saveSystemSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);
			static void saveCanvasSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);
			static void saveKinectSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);
			static void saveRoomSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);
			static void saveLocalVRPNSkeletonsSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);
			static void saveRemoteVRPNSkeletonsSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);

#ifdef _WIIMOTE_SUPPORT_
			static void saveLocalVRPNWiimotesSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);
			static void saveRemoteVRPNWiimotesSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement);
#endif

		public:
			/*
			** Settings
			*/
			static SystemSettings			system;
			static CanvasSettingsMap		canvas;
			static KinectSettingsMap		kinect;
			static VirtualRoomSettings		room;
			static VRPNSkeletonSettings		localVRPNSkeletons[KINECT_SKELETON_COUNT];
			static VRPNSkeletonSettings		remoteVRPNSkeletons[KINECT_SKELETON_COUNT];

#ifdef _WIIMOTE_SUPPORT_
			static VRPNWiimoteSettings		localVRPNWiimotes[WIIMOTE_COUNT];
			static VRPNWiimoteSettings		remoteVRPNWiimotes[WIIMOTE_COUNT];
#endif

			static OtherSettings			other;

			static void initialize();
			static void destroy();

			static bool isInitialized();
			static void load(const std::string& filename);
			static void save(const std::string& filename);
		};
	}
}

#endif
