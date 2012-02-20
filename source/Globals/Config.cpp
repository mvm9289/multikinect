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


#include "Globals/Config.h"

#include "Files/Filesystem.h"
#include "Tools/Log.h"


const Config::ApplicationMode	Config::DEFAULT_APP_MODE					=	KINECT_MASTER;
const std::string				Config::DEFAULT_APPDATA_PATH				=	".";
const std::string				Config::DEFAULT_CONFIG_PATH					=	"MultiKinect/Data/config";
const std::string				Config::DEFAULT_LOG_PATH					=	"MultiKinect/Data/log";
const Config::RenderLoopMode	Config::DEFAULT_RENDER_LOOP_MODE			=	RENDER_TIMER;
const uint32					Config::DEFAULT_CANVAS_WIDTH				=	320;
const uint32					Config::DEFAULT_CANVAS_HEIGHT				=	240;
const bool						Config::DEFAULT_SHOW_RGB_IMAGE				=	true;
const bool						Config::DEFAULT_SHOW_DEPTH_MAP				=	true;
const bool						Config::DEFAULT_SHOW_SKELETON_TRACKING		=	true;
const bool						Config::DEFAULT_SHOW_SKELETONS				=	true;
const bool						Config::DEFAULT_SHOW_ORIENTATIONS			=	false;
const bool						Config::DEFAULT_COMBINE_SKELETONS			=	true;
const bool						Config::DEFAULT_ENABLED_RGB_IMAGE			=	true;
const bool						Config::DEFAULT_ENABLED_DEPTH_MAP			=	true;
const bool						Config::DEFAULT_ENABLED_SKELETON_TRACKING	=	true;
const bool						Config::DEFAULT_ENABLED_NEAR_MODE			=	false;
const bool						Config::DEFAULT_ENABLED_SEATED_MODE			=	false;
const bool						Config::DEFAULT_ENABLED_HIERARCHICAL_ORI	=	false;
const float32					Config::DEFAULT_KINECT_TRANSLATION_X		=	0.0f;
const float32					Config::DEFAULT_KINECT_TRANSLATION_Y		=	0.0f;
const float32					Config::DEFAULT_KINECT_TRANSLATION_Z		=	0.0f;
const float32					Config::DEFAULT_KINECT_ROTATION_X			=	0.0f;
const float32					Config::DEFAULT_KINECT_ROTATION_Y			=	0.0f;
const float32					Config::DEFAULT_KINECT_ROTATION_Z			=	0.0f;
const float32					Config::DEFAULT_ROOM_ORIGIN_X				=	0.0f;
const float32					Config::DEFAULT_ROOM_ORIGIN_Y				=	0.0f;
const float32					Config::DEFAULT_ROOM_ORIGIN_Z				=	0.0f;
const float32					Config::DEFAULT_ROOM_WIDTH					=	3.0f;
const float32					Config::DEFAULT_ROOM_HEIGHT					=	3.0f;
const float32					Config::DEFAULT_ROOM_DEPTH					=	3.0f;
const std::string				Config::DEFAULT_VRPN_SKELETON_BASE_ADDR		=	"KinectSkeleton";
const bool						Config::DEFAULT_VRPN_SEND_ORIENTATIONS			=	true;

#ifdef _WIIMOTE_SUPPORT_
const std::string				Config::DEFAULT_VRPN_WIIMOTE_BASE_ADDR		=	"WiiMote";
#endif

const std::string				Config::DEFAULT_VRPN_SERVER_ADDR			=	"localhost";
const float32					Config::DEFAULT_CONFIDENCE_MARGIN			=	2.0f*DEG2RAD32(10.0f)/PI32;
const std::string				Config::DEFAULT_CONFIG_FILE					=	"MultiKinect_default.conf";

bool							Config::initialized_ = false;
Config::SystemSettings			Config::system;
Config::CanvasSettingsMap		Config::canvas;
Config::KinectSettingsMap		Config::kinect;
Config::VirtualRoomSettings		Config::room;
Config::VRPNSkeletonSettings	Config::localVRPNSkeletons[KINECT_SKELETON_COUNT];
Config::VRPNSkeletonSettings	Config::remoteVRPNSkeletons[KINECT_SKELETON_COUNT];

#ifdef _WIIMOTE_SUPPORT_
Config::VRPNWiimoteSettings		Config::localVRPNWiimotes[WIIMOTE_COUNT];
Config::VRPNWiimoteSettings		Config::remoteVRPNWiimotes[WIIMOTE_COUNT];
#endif

Config::OtherSettings			Config::other;

void Config::initialize()
{
	if (!initialized_)
	{
		initialized_ = true;

		Filesystem::createDirectory(system.configPath);
		Filesystem::createDirectory(system.logPath);

		if (Globals::LAST_CONFIGURATION != "" && Filesystem::fileExists(Globals::LAST_CONFIGURATION))
			load(Globals::LAST_CONFIGURATION);
		else if (Filesystem::fileExists(system.configPath + "/" + DEFAULT_CONFIG_FILE))
			load(system.configPath + "/" + DEFAULT_CONFIG_FILE);

		system.currentMode = system.appMode;
		if (Globals::INSTANCE_ID != "") system.currentMode = KINECT_SINGLE_DEVICE;
	}
	else Log::write("[Config] initialize()", "ERROR: Config already initialized.");
}

void Config::destroy()
{
	if (initialized_) initialized_ = false;
	else Log::write("[Config] destroy()", "ERROR: Config not initialized.");
}

bool Config::isInitialized()
{
	return initialized_;
}

void Config::load(const std::string& filename)
{
	if (initialized_)
	{
		tinyxml2::XMLDocument xmlDocument;
		if (xmlDocument.LoadFile(filename.c_str()) == tinyxml2::XML_SUCCESS)
		{
			Globals::LAST_CONFIGURATION = filename;

			tinyxml2::XMLElement* rootElem = xmlDocument.RootElement();
			loadSystemSettings(rootElem);
			loadCanvasSettings(rootElem);
			loadKinectSettings(rootElem);
			loadRoomSettings(rootElem);
			loadLocalVRPNSkeletonsSettings(rootElem);
			loadRemoteVRPNSkeletonsSettings(rootElem);

#ifdef _WIIMOTE_SUPPORT_
			loadLocalVRPNWiimotesSettings(rootElem);
			loadRemoteVRPNWiimotesSettings(rootElem);
#endif
		}
		else Log::write("[Config] load()", "ERROR: Unable to open configuration file.");
	}
	else Log::write("[Config] load()", "ERROR: Config not initialized.");
}

void Config::save(const std::string& filename)
{
	if (initialized_)
	{
		Globals::LAST_CONFIGURATION = filename;

		tinyxml2::XMLDocument	xmlDocument;
		tinyxml2::XMLElement*	rootElem = xmlDocument.NewElement("MultiKinect_Configuration");
		saveSystemSettings(&xmlDocument, rootElem);
		saveCanvasSettings(&xmlDocument, rootElem);
		saveKinectSettings(&xmlDocument, rootElem);
		saveRoomSettings(&xmlDocument, rootElem);
		saveLocalVRPNSkeletonsSettings(&xmlDocument, rootElem);
		saveRemoteVRPNSkeletonsSettings(&xmlDocument, rootElem);

#ifdef _WIIMOTE_SUPPORT_
		saveLocalVRPNWiimotesSettings(&xmlDocument, rootElem);
		saveRemoteVRPNWiimotesSettings(&xmlDocument, rootElem);
#endif

		xmlDocument.InsertFirstChild(rootElem);
		xmlDocument.SaveFile(filename.c_str());
	}
	else Log::write("[Config] save()", "ERROR: Config not initialized.");
}

void Config::loadSystemSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* appModeElem = parentElement->FirstChildElement("application_mode");
	if (appModeElem) system.appMode = static_cast<ApplicationMode>(string_cast<uint32>(appModeElem->GetText()));

	const tinyxml2::XMLElement* renderModeElem = parentElement->FirstChildElement("render_loop_mode");
	if (renderModeElem) system.renderMode = static_cast<RenderLoopMode>(string_cast<uint32>(renderModeElem->GetText()));
}

void Config::loadCanvasSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* canvasElem = parentElement->FirstChildElement("canvas");
	while (canvasElem)
	{
		std::string id(canvasElem->Attribute("id"));

		canvas[id].alias = std::string(canvasElem->Attribute("alias"));

		const tinyxml2::XMLElement* sizeElem = canvasElem->FirstChildElement("size");
		while (sizeElem)
		{
			if (sizeElem->Attribute("dimension", "width"))
				canvas[id].width = string_cast<uint32>(std::string(sizeElem->GetText()));
			else if (sizeElem->Attribute("dimension", "height"))
				canvas[id].height = string_cast<uint32>(std::string(sizeElem->GetText()));
			sizeElem = sizeElem->NextSiblingElement("size");
		}

		const tinyxml2::XMLElement* dataStreamElem = canvasElem->FirstChildElement("show_data_stream");
		while (dataStreamElem)
		{
			if (dataStreamElem->Attribute("type", "rgb_image"))
				canvas[id].rgbImage = string_cast<bool>(std::string(dataStreamElem->GetText()));
			else if (dataStreamElem->Attribute("type", "depth_map"))
				canvas[id].depthMap = string_cast<bool>(std::string(dataStreamElem->GetText()));
			else if (dataStreamElem->Attribute("type", "skeleton_tracking"))
				canvas[id].skeletonTracking = string_cast<bool>(std::string(dataStreamElem->GetText()));
			dataStreamElem = dataStreamElem->NextSiblingElement("show_data_stream");
		}

		const tinyxml2::XMLElement* showSkeletonsElem = canvasElem->FirstChildElement("show_skeletons");
		if (showSkeletonsElem) canvas[id].showSkeletons = string_cast<bool>(std::string(showSkeletonsElem->GetText()));

		const tinyxml2::XMLElement* showOrientationsElem = canvasElem->FirstChildElement("show_orientations");
		if (showOrientationsElem) canvas[id].showOrientations = string_cast<bool>(std::string(showOrientationsElem->GetText()));

		const tinyxml2::XMLElement* combineSkeletonsElem = canvasElem->FirstChildElement("combine_skeletons");
		if (combineSkeletonsElem) canvas[id].combineSkeletons = string_cast<bool>(std::string(combineSkeletonsElem->GetText()));

		canvasElem = canvasElem->NextSiblingElement("canvas");
	}
}

void Config::loadKinectSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* kinectElem = parentElement->FirstChildElement("kinect_device");
	while (kinectElem)
	{
		std::string id(kinectElem->Attribute("id"));

		kinect[id].alias = std::string(kinectElem->Attribute("alias"));

		const tinyxml2::XMLElement* dataStreamElem = kinectElem->FirstChildElement("enable_data_stream");
		while (dataStreamElem)
		{
			if (dataStreamElem->Attribute("type", "rgb_image"))
				kinect[id].rgbImage = string_cast<bool>(std::string(dataStreamElem->GetText()));
			else if (dataStreamElem->Attribute("type", "depth_map"))
				kinect[id].depthMap = string_cast<bool>(std::string(dataStreamElem->GetText()));
			else if (dataStreamElem->Attribute("type", "skeleton_tracking"))
				kinect[id].skeletonTracking = string_cast<bool>(std::string(dataStreamElem->GetText()));
			dataStreamElem = dataStreamElem->NextSiblingElement("enable_data_stream");
		}

		const tinyxml2::XMLElement* modeElem = kinectElem->FirstChildElement("enable_mode");
		while (modeElem)
		{
			if (modeElem->Attribute("type", "near"))
				kinect[id].nearMode = string_cast<bool>(std::string(modeElem->GetText()));
			else if (modeElem->Attribute("type", "seated"))
				kinect[id].seatedMode = string_cast<bool>(std::string(modeElem->GetText()));
			else if (modeElem->Attribute("type", "hierarchical_orientation"))
				kinect[id].hierarchicalOri = string_cast<bool>(std::string(modeElem->GetText()));
			modeElem = modeElem->NextSiblingElement("enable_mode");
		}

		const tinyxml2::XMLElement* translationElem = kinectElem->FirstChildElement("translation");
		while (translationElem)
		{
			if (translationElem->Attribute("axis", "X"))
				kinect[id].translation.x = string_cast<float32>(std::string(translationElem->GetText()));
			else if (translationElem->Attribute("axis", "Y"))
				kinect[id].translation.y = string_cast<float32>(std::string(translationElem->GetText()));
			else if (translationElem->Attribute("axis", "Z"))
				kinect[id].translation.z = string_cast<float32>(std::string(translationElem->GetText()));
			translationElem = translationElem->NextSiblingElement("translation");
		}

		const tinyxml2::XMLElement* rotationElem = kinectElem->FirstChildElement("rotation");
		while (rotationElem)
		{
			if (rotationElem->Attribute("axis", "X"))
				kinect[id].rotation.x = DEG2RAD32(string_cast<float32>(std::string(rotationElem->GetText())));
			else if (rotationElem->Attribute("axis", "Y"))
				kinect[id].rotation.y = DEG2RAD32(string_cast<float32>(std::string(rotationElem->GetText())));
			else if (rotationElem->Attribute("axis", "Z"))
				kinect[id].rotation.z = DEG2RAD32(string_cast<float32>(std::string(rotationElem->GetText())));
			rotationElem = rotationElem->NextSiblingElement("rotation");
		}

		const tinyxml2::XMLElement* angleElem = kinectElem->FirstChildElement("elevation_angle");
		while (angleElem)
		{
			if (angleElem->Attribute("limit", "min"))
				kinect[id].minElevationAngle = string_cast<int32>(std::string(angleElem->GetText()));
			if (angleElem->Attribute("limit", "max"))
				kinect[id].maxElevationAngle = string_cast<int32>(std::string(angleElem->GetText()));
			angleElem = angleElem->NextSiblingElement("elevation_angle");
		}

		kinectElem = kinectElem->NextSiblingElement("kinect_device");
	}
}

void Config::loadRoomSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* roomElem = parentElement->FirstChildElement("virtual_room");
	if (roomElem)
	{
		const tinyxml2::XMLElement* sizeElem = roomElem->FirstChildElement("size");
		while (sizeElem)
		{
			if (sizeElem->Attribute("dimension", "width"))
				room.width = string_cast<float32>(std::string(sizeElem->GetText()));
			else if (sizeElem->Attribute("dimension", "height"))
				room.height = string_cast<float32>(std::string(sizeElem->GetText()));
			else if (sizeElem->Attribute("dimension", "depth"))
				room.depth = string_cast<float32>(std::string(sizeElem->GetText()));
			sizeElem = sizeElem->NextSiblingElement("size");
		}

		const tinyxml2::XMLElement* originElem = roomElem->FirstChildElement("origin");
		while (originElem)
		{
			if (originElem->Attribute("axis", "X"))
				room.origin.x = string_cast<float32>(std::string(originElem->GetText()));
			else if (originElem->Attribute("axis", "Y"))
				room.origin.y = string_cast<float32>(std::string(originElem->GetText()));
			else if (originElem->Attribute("axis", "Z"))
				room.origin.z = string_cast<float32>(std::string(originElem->GetText()));
			originElem = originElem->NextSiblingElement("origin");
		}
	}
}

void Config::loadLocalVRPNSkeletonsSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* localVRPNSkeletonElem = parentElement->FirstChildElement("vrpn_local_skeleton");
	while (localVRPNSkeletonElem)
	{
		uint32 id = string_cast<uint32>(std::string(localVRPNSkeletonElem->Attribute("id")));
		localVRPNSkeletons[id].enabled = true;
		
		const tinyxml2::XMLElement* addressElem = localVRPNSkeletonElem->FirstChildElement("address");
		if (addressElem) localVRPNSkeletons[id].address = std::string(addressElem->GetText());

		const tinyxml2::XMLElement* sendOrientationsElem = localVRPNSkeletonElem->FirstChildElement("send_orientations");
		if (sendOrientationsElem) localVRPNSkeletons[id].sendOrientations = string_cast<bool>(sendOrientationsElem->GetText());

		const tinyxml2::XMLElement* sendOriginalSkeletonsElem = localVRPNSkeletonElem->FirstChildElement("send_original_skeletons");
		if (sendOriginalSkeletonsElem) localVRPNSkeletons[id].sendOriginalSkeletons = string_cast<bool>(sendOriginalSkeletonsElem->GetText());

		localVRPNSkeletonElem = localVRPNSkeletonElem->NextSiblingElement("vrpn_local_skeleton");
	}
}

void Config::loadRemoteVRPNSkeletonsSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* remoteVRPNSkeletonElem = parentElement->FirstChildElement("vrpn_remote_skeleton");
	while (remoteVRPNSkeletonElem)
	{
		uint32 id = string_cast<uint32>(std::string(remoteVRPNSkeletonElem->Attribute("id")));

		std::string address	= DEFAULT_VRPN_SKELETON_BASE_ADDR;
		std::string server	= DEFAULT_VRPN_SERVER_ADDR;

		const tinyxml2::XMLElement* addressElem = remoteVRPNSkeletonElem->FirstChildElement("address");
		if (addressElem) address = std::string(addressElem->GetText());

		int32 atPosition = basic_cast<int32>(address.find("@"));
		if (atPosition != std::string::npos)
		{
			server	= address.substr(atPosition + 1);
			address	= address.substr(0, atPosition);
		}

		remoteVRPNSkeletons[id].enabled			=	true;
		remoteVRPNSkeletons[id].address			=	address;
		remoteVRPNSkeletons[id].serverAddress	=	server;
		remoteVRPNSkeletonElem					=	remoteVRPNSkeletonElem->NextSiblingElement("vrpn_remote_skeleton");
	}
}

#ifdef _WIIMOTE_SUPPORT_
void Config::loadLocalVRPNWiimotesSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* localVRPNWiimoteElem = parentElement->FirstChildElement("vrpn_local_wiimote");
	while (localVRPNWiimoteElem)
	{
		uint32 id = string_cast<uint32>(std::string(localVRPNWiimoteElem->Attribute("id")));
		localVRPNWiimotes[id].enabled = true;

		const tinyxml2::XMLElement* addressElem = localVRPNWiimoteElem->FirstChildElement("address");
		if (addressElem) localVRPNWiimotes[id].address = std::string(addressElem->GetText());

		localVRPNWiimoteElem = localVRPNWiimoteElem->NextSiblingElement("vrpn_local_wiimote");
	}
}

void Config::loadRemoteVRPNWiimotesSettings(const tinyxml2::XMLElement* parentElement)
{
	const tinyxml2::XMLElement* remoteVRPNWiimoteElem = parentElement->FirstChildElement("vrpn_remote_wiimote");
	while (remoteVRPNWiimoteElem)
	{
		uint32 id = string_cast<uint32>(std::string(remoteVRPNWiimoteElem->Attribute("id")));

		std::string address	= DEFAULT_VRPN_WIIMOTE_BASE_ADDR;
		std::string server	= DEFAULT_VRPN_SERVER_ADDR;

		const tinyxml2::XMLElement* addressElem = remoteVRPNWiimoteElem->FirstChildElement("address");
		if (addressElem) address = std::string(addressElem->GetText());

		int32 atPosition = basic_cast<int32>(address.find("@"));
		if (atPosition != std::string::npos)
		{
			server	= address.substr(atPosition + 1);
			address	= address.substr(0, atPosition);
		}

		remoteVRPNWiimotes[id].enabled			=	true;
		remoteVRPNWiimotes[id].address			=	address;
		remoteVRPNWiimotes[id].serverAddress	=	server;
		remoteVRPNWiimoteElem					=	remoteVRPNWiimoteElem->NextSiblingElement("vrpn_remote_wiimote");
	}
}
#endif

void Config::saveSystemSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	parentElement->InsertEndChild(xmlDocument->NewComment(" APPLICATION BEHAVIOR SETTINGS "));

	tinyxml2::XMLElement* appModeElem = xmlDocument->NewElement("application_mode");
	appModeElem->InsertEndChild(xmlDocument->NewText(enum_cast<std::string>(system.appMode).c_str()));
	parentElement->InsertEndChild(appModeElem);

	tinyxml2::XMLElement* renderModeElem = xmlDocument->NewElement("render_loop_mode");
	renderModeElem->InsertEndChild(xmlDocument->NewText(enum_cast<std::string>(system.renderMode).c_str()));
	parentElement->InsertEndChild(renderModeElem);

	parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}

void Config::saveCanvasSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	bool first = true;
	for (CanvasSettingsMapIterator i = canvas.begin(); i != canvas.end(); i++)
	{
		if (i->first != "")
		{
			if (first)
			{
				parentElement->InsertEndChild(xmlDocument->NewComment(" CANVAS SETTINGS OF EACH PROCESS "));
				first = false;
			}

			tinyxml2::XMLElement* canvasElem = xmlDocument->NewElement("canvas");
			canvasElem->SetAttribute("id", i->first.c_str());
			canvasElem->SetAttribute("alias", i->second.alias.c_str());

			tinyxml2::XMLElement* sizeElem = xmlDocument->NewElement("size");
			sizeElem->SetAttribute("dimension", "width");
			sizeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.width).c_str()));
			canvasElem->InsertEndChild(sizeElem);

			sizeElem = xmlDocument->NewElement("size");
			sizeElem->SetAttribute("dimension", "height");
			sizeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.height).c_str()));
			canvasElem->InsertEndChild(sizeElem);

			if (i->first != "master" && i->first != "client")
			{
				tinyxml2::XMLElement* dataStreamElem = xmlDocument->NewElement("show_data_stream");
				dataStreamElem->SetAttribute("type", "rgb_image");
				dataStreamElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.rgbImage).c_str()));
				canvasElem->InsertEndChild(dataStreamElem);

				dataStreamElem = xmlDocument->NewElement("show_data_stream");
				dataStreamElem->SetAttribute("type", "depth_map");
				dataStreamElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.depthMap).c_str()));
				canvasElem->InsertEndChild(dataStreamElem);

				dataStreamElem = xmlDocument->NewElement("show_data_stream");
				dataStreamElem->SetAttribute("type", "skeleton_tracking");
				dataStreamElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.skeletonTracking).c_str()));
				canvasElem->InsertEndChild(dataStreamElem);
			}
			else
			{
				tinyxml2::XMLElement* showSkeletonsElem = xmlDocument->NewElement("show_skeletons");
				showSkeletonsElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.showSkeletons).c_str()));
				canvasElem->InsertEndChild(showSkeletonsElem);

				tinyxml2::XMLElement* showOrientationsElem = xmlDocument->NewElement("show_orientations");
				showOrientationsElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.showOrientations).c_str()));
				canvasElem->InsertEndChild(showOrientationsElem);

				if (i->first == "master")
				{
					tinyxml2::XMLElement* combineSkeletonsElem = xmlDocument->NewElement("combine_skeletons");
					combineSkeletonsElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.combineSkeletons).c_str()));
					canvasElem->InsertEndChild(combineSkeletonsElem);
				}
			}

			parentElement->InsertEndChild(canvasElem);
		}
	}

	if (!first) parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}

void Config::saveKinectSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	bool first = true;
	for (KinectSettingsMapIterator i = kinect.begin(); i != kinect.end(); i++)
	{
		if (i->first != "")
		{
			if (first)
			{
				parentElement->InsertEndChild(xmlDocument->NewComment(" KINECT DEVICES SETTINGS "));
				first = false;
			}

			tinyxml2::XMLElement* kinectElem = xmlDocument->NewElement("kinect_device");
			kinectElem->SetAttribute("id", i->first.c_str());
			kinectElem->SetAttribute("alias", i->second.alias.c_str());

			tinyxml2::XMLElement* dataStreamElem = xmlDocument->NewElement("enable_data_stream");
			dataStreamElem->SetAttribute("type", "rgb_image");
			dataStreamElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.rgbImage).c_str()));
			kinectElem->InsertEndChild(dataStreamElem);

			dataStreamElem = xmlDocument->NewElement("enable_data_stream");
			dataStreamElem->SetAttribute("type", "depth_map");
			dataStreamElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.depthMap).c_str()));
			kinectElem->InsertEndChild(dataStreamElem);

			dataStreamElem = xmlDocument->NewElement("enable_data_stream");
			dataStreamElem->SetAttribute("type", "skeleton_tracking");
			dataStreamElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.skeletonTracking).c_str()));
			kinectElem->InsertEndChild(dataStreamElem);

			tinyxml2::XMLElement* modeElem = xmlDocument->NewElement("enable_mode");
			modeElem->SetAttribute("type", "near");
			modeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.nearMode).c_str()));
			kinectElem->InsertEndChild(modeElem);

			modeElem = xmlDocument->NewElement("enable_mode");
			modeElem->SetAttribute("type", "seated");
			modeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.seatedMode).c_str()));
			kinectElem->InsertEndChild(modeElem);

			modeElem = xmlDocument->NewElement("enable_mode");
			modeElem->SetAttribute("type", "hierarchical_orientation");
			modeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.hierarchicalOri).c_str()));
			kinectElem->InsertEndChild(modeElem);

			tinyxml2::XMLElement* translationElem = xmlDocument->NewElement("translation");
			translationElem->SetAttribute("axis", "X");
			translationElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.translation.x).c_str()));
			kinectElem->InsertEndChild(translationElem);

			translationElem = xmlDocument->NewElement("translation");
			translationElem->SetAttribute("axis", "Y");
			translationElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.translation.y).c_str()));
			kinectElem->InsertEndChild(translationElem);

			translationElem = xmlDocument->NewElement("translation");
			translationElem->SetAttribute("axis", "Z");
			translationElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.translation.z).c_str()));
			kinectElem->InsertEndChild(translationElem);

			tinyxml2::XMLElement* rotationElem = xmlDocument->NewElement("rotation");
			rotationElem->SetAttribute("axis", "X");
			rotationElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(RAD2DEG32(i->second.rotation.x)).c_str()));
			kinectElem->InsertEndChild(rotationElem);

			rotationElem = xmlDocument->NewElement("rotation");
			rotationElem->SetAttribute("axis", "Y");
			rotationElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(RAD2DEG32(i->second.rotation.y)).c_str()));
			kinectElem->InsertEndChild(rotationElem);

			rotationElem = xmlDocument->NewElement("rotation");
			rotationElem->SetAttribute("axis", "Z");
			rotationElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(RAD2DEG32(i->second.rotation.z)).c_str()));
			kinectElem->InsertEndChild(rotationElem);

			tinyxml2::XMLElement* angleElem = xmlDocument->NewElement("elevation_angle");
			angleElem->SetAttribute("limit", "min");
			angleElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.minElevationAngle).c_str()));
			kinectElem->InsertEndChild(angleElem);

			angleElem = xmlDocument->NewElement("elevation_angle");
			angleElem->SetAttribute("limit", "max");
			angleElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(i->second.maxElevationAngle).c_str()));
			kinectElem->InsertEndChild(angleElem);

			parentElement->InsertEndChild(kinectElem);
		}
	}

	if (!first) parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}

void Config::saveRoomSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	parentElement->InsertEndChild(xmlDocument->NewComment(" VIRTUAL ROOM SETTINGS "));

	tinyxml2::XMLElement* roomElem = xmlDocument->NewElement("virtual_room");

	tinyxml2::XMLElement* sizeElem = xmlDocument->NewElement("size");
	sizeElem->SetAttribute("dimension", "width");
	sizeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(room.width).c_str()));
	roomElem->InsertEndChild(sizeElem);

	sizeElem = xmlDocument->NewElement("size");
	sizeElem->SetAttribute("dimension", "height");
	sizeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(room.height).c_str()));
	roomElem->InsertEndChild(sizeElem);

	sizeElem = xmlDocument->NewElement("size");
	sizeElem->SetAttribute("dimension", "depth");
	sizeElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(room.depth).c_str()));
	roomElem->InsertEndChild(sizeElem);

	tinyxml2::XMLElement* originElem = xmlDocument->NewElement("origin");
	originElem->SetAttribute("axis", "X");
	originElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(room.origin.x).c_str()));
	roomElem->InsertEndChild(originElem);

	originElem = xmlDocument->NewElement("origin");
	originElem->SetAttribute("axis", "Y");
	originElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(room.origin.y).c_str()));
	roomElem->InsertEndChild(originElem);

	originElem = xmlDocument->NewElement("origin");
	originElem->SetAttribute("axis", "Z");
	originElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(room.origin.z).c_str()));
	roomElem->InsertEndChild(originElem);

	parentElement->InsertEndChild(roomElem);

	parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}

void Config::saveLocalVRPNSkeletonsSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	bool first = true;
	for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
	{
		if (localVRPNSkeletons[i].enabled)
		{
			if (first)
			{
				parentElement->InsertEndChild(xmlDocument->NewComment(" LOCAL VRPN KINECT SKELETONS SETTINGS "));
				first = false;
			}

			tinyxml2::XMLElement* localVRPNSkeletonElem = xmlDocument->NewElement("vrpn_local_skeleton");
			localVRPNSkeletonElem->SetAttribute("id", basic_cast<std::string>(i).c_str());

			tinyxml2::XMLElement* addressElem = xmlDocument->NewElement("address");
			addressElem->InsertEndChild(xmlDocument->NewText(localVRPNSkeletons[i].address.c_str()));
			localVRPNSkeletonElem->InsertEndChild(addressElem);

			tinyxml2::XMLElement* sendOrientationsElem = xmlDocument->NewElement("send_orientations");
			sendOrientationsElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(localVRPNSkeletons[i].sendOrientations).c_str()));
			localVRPNSkeletonElem->InsertEndChild(sendOrientationsElem);

			tinyxml2::XMLElement* sendOriginalSkeletonsElem = xmlDocument->NewElement("send_original_skeletons");
			sendOriginalSkeletonsElem->InsertEndChild(xmlDocument->NewText(basic_cast<std::string>(localVRPNSkeletons[i].sendOriginalSkeletons).c_str()));
			localVRPNSkeletonElem->InsertEndChild(sendOriginalSkeletonsElem);

			parentElement->InsertEndChild(localVRPNSkeletonElem);
		}
	}

	if (!first) parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}

void Config::saveRemoteVRPNSkeletonsSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	bool first = true;
	for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
	{
		if (remoteVRPNSkeletons[i].enabled)
		{
			if (first)
			{
				parentElement->InsertEndChild(xmlDocument->NewComment(" REMOTE VRPN KINECT SKELETONS SETTINGS "));
				first = false;
			}

			tinyxml2::XMLElement* remoteVRPNSkeletonElem = xmlDocument->NewElement("vrpn_remote_skeleton");
			remoteVRPNSkeletonElem->SetAttribute("id", basic_cast<std::string>(i).c_str());

			tinyxml2::XMLElement* addressElem = xmlDocument->NewElement("address");
			std::string address = remoteVRPNSkeletons[i].address + "@" + remoteVRPNSkeletons[i].serverAddress;
			addressElem->InsertEndChild(xmlDocument->NewText(address.c_str()));
			remoteVRPNSkeletonElem->InsertEndChild(addressElem);

			parentElement->InsertEndChild(remoteVRPNSkeletonElem);
		}
	}

	if (!first) parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}

#ifdef _WIIMOTE_SUPPORT_
void Config::saveLocalVRPNWiimotesSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	bool first = true;
	for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
	{
		if (localVRPNWiimotes[i].enabled)
		{
			if (first)
			{
				parentElement->InsertEndChild(xmlDocument->NewComment(" LOCAL VRPN WIIMOTES SETTINGS "));
				first = false;
			}

			tinyxml2::XMLElement* localVRPNWiimotesElem = xmlDocument->NewElement("vrpn_local_wiimote");
			localVRPNWiimotesElem->SetAttribute("id", basic_cast<std::string>(i).c_str());

			tinyxml2::XMLElement* buttonsAddressElem = xmlDocument->NewElement("address");
			buttonsAddressElem->InsertEndChild(xmlDocument->NewText(localVRPNWiimotes[i].address.c_str()));
			localVRPNWiimotesElem->InsertEndChild(buttonsAddressElem);

			parentElement->InsertEndChild(localVRPNWiimotesElem);
		}
	}

	if (!first) parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}

void Config::saveRemoteVRPNWiimotesSettings(tinyxml2::XMLDocument* xmlDocument, tinyxml2::XMLElement* parentElement)
{
	bool first = true;
	for (uint32 i = 0; i < WIIMOTE_COUNT; i++)
	{
		if (remoteVRPNWiimotes[i].enabled)
		{
			if (first)
			{
				parentElement->InsertEndChild(xmlDocument->NewComment(" REMOTE VRPN WIIMOTES SETTINGS "));
				first = false;
			}

			tinyxml2::XMLElement* remoteVRPNWiimotesElem = xmlDocument->NewElement("vrpn_remote_wiimote");
			remoteVRPNWiimotesElem->SetAttribute("id", basic_cast<std::string>(i).c_str());

			tinyxml2::XMLElement* buttonsAddressElem = xmlDocument->NewElement("address");
			std::string buttonsAddress = remoteVRPNWiimotes[i].address + "@" + remoteVRPNWiimotes[i].serverAddress;
			buttonsAddressElem->InsertEndChild(xmlDocument->NewText(buttonsAddress.c_str()));
			remoteVRPNWiimotesElem->InsertEndChild(buttonsAddressElem);

			parentElement->InsertEndChild(remoteVRPNWiimotesElem);
		}
	}

	if (!first) parentElement->InsertEndChild(xmlDocument->NewComment(" "));
}
#endif

Config::SystemSettings::SystemSettings()
{
	appTitle	= "MultiKinect";
	appMode		= DEFAULT_APP_MODE;
	currentMode	= DEFAULT_APP_MODE;
	appDataPath	= Filesystem::getLocalDataPath();
	configPath	= appDataPath + "/" + DEFAULT_CONFIG_PATH;
	logPath		= appDataPath + "/" + DEFAULT_LOG_PATH;
	renderMode	= DEFAULT_RENDER_LOOP_MODE;
	restartApp	= false;
}

Config::CanvasSettings::CanvasSettings()
{
	alias				=	"";
	width				=	DEFAULT_CANVAS_WIDTH;
	height				=	DEFAULT_CANVAS_HEIGHT;
	rgbImage			=	DEFAULT_SHOW_RGB_IMAGE;
	depthMap			=	DEFAULT_SHOW_DEPTH_MAP;
	skeletonTracking	=	DEFAULT_SHOW_SKELETON_TRACKING;
	showSkeletons		=	DEFAULT_SHOW_SKELETONS;
	showOrientations	=	DEFAULT_SHOW_ORIENTATIONS;
	combineSkeletons	=	DEFAULT_COMBINE_SKELETONS;
}

Config::KinectSettings::KinectSettings()
{
	alias				=	"";
	rgbImage			=	DEFAULT_ENABLED_RGB_IMAGE;
	depthMap			=	DEFAULT_ENABLED_DEPTH_MAP;
	skeletonTracking	=	DEFAULT_ENABLED_SKELETON_TRACKING;
	nearMode			=	DEFAULT_ENABLED_NEAR_MODE;
	seatedMode			=	DEFAULT_ENABLED_SEATED_MODE;
	hierarchicalOri		=	DEFAULT_ENABLED_HIERARCHICAL_ORI;
	translation.x		=	DEFAULT_KINECT_TRANSLATION_X;
	translation.y		=	DEFAULT_KINECT_TRANSLATION_Y;
	translation.z		=	DEFAULT_KINECT_TRANSLATION_Z;
	rotation.x			=	DEFAULT_KINECT_ROTATION_X;
	rotation.y			=	DEFAULT_KINECT_ROTATION_Y;
	rotation.z			=	DEFAULT_KINECT_ROTATION_Z;
	minElevationAngle	=	KINECT_MIN_TILT_ANGLE;
	maxElevationAngle	=	KINECT_MAX_TILT_ANGLE;
}

Config::VirtualRoomSettings::VirtualRoomSettings()
{
	origin.x			=	DEFAULT_ROOM_ORIGIN_X;
	origin.y			=	DEFAULT_ROOM_ORIGIN_Y;
	origin.z			=	DEFAULT_ROOM_ORIGIN_Z;
	width				=	DEFAULT_ROOM_WIDTH;
	height				=	DEFAULT_ROOM_HEIGHT;
	depth				=	DEFAULT_ROOM_DEPTH;
}

Config::VRPNSkeletonSettings::VRPNSkeletonSettings()
{
	enabled					=	false;
	address					=	DEFAULT_VRPN_SKELETON_BASE_ADDR;
	serverAddress			=	DEFAULT_VRPN_SERVER_ADDR;
	sendOrientations		=	DEFAULT_VRPN_SEND_ORIENTATIONS;
	sendOriginalSkeletons	=	true;
}

#ifdef _WIIMOTE_SUPPORT_
Config::VRPNWiimoteSettings::VRPNWiimoteSettings()
{
	enabled			=	false;
	address			=	DEFAULT_VRPN_WIIMOTE_BASE_ADDR;
	serverAddress	=	DEFAULT_VRPN_SERVER_ADDR;
}
#endif

Config::OtherSettings::OtherSettings()
{
	confidenceMargin			=	DEFAULT_CONFIDENCE_MARGIN;
}
