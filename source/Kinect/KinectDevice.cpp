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


#include "Kinect/KinectDevice.h"

#include "Globals/Definitions.h"
#include "Globals/Vars.h"
#include "Geom/Point.h"
#include "Globals/Config.h"
#include "Kinect/KinectManager.h"
#include "Kinect/KinectSkeleton.h"
#include "Interprocess/SharedMemoryManager.h"
#include "Tools/Log.h"
#include "Tools/Timer.h"

using namespace MultiKinect;
using namespace Geom;
using namespace Globals;
using namespace Kinect;
using namespace Interprocess;
using namespace Tools;


KinectDevice::KinectDevice(uint32 index)
{
	instance_ = 0;
	valid_ = SUCCEEDED(NuiCreateSensorByIndex(index, &instance_));
	create();
}

KinectDevice::KinectDevice(const std::string& deviceID)
{
	instance_				=	0;
	std::wstring wDeviceID	=	string_cast<std::wstring>(deviceID);

	if (wDeviceID != L"")	valid_ = SUCCEEDED(NuiCreateSensorById(&wDeviceID[0], &instance_));
	else					valid_ = false;

	create();
}

KinectDevice::~KinectDevice()
{
	if (initialized_) shutDown();
	if (valid_)
	{
		instance_->Release();
		instance_ = 0;
		valid_ = false;
	}
}

void KinectDevice::create()
{
	initialized_ = colorStreamOpened_ = depthStreamOpened_ = skeletonEnabled_ = false;
	depthFlags_ = skeletonFlags_ = 0;
	colorStreamHandle_ = depthStreamHandle_ = 0;
	colorFrameEvent_ = depthFrameEvent_ = skeletonFrameEvent_ = 0;
	processStopEvent_ = processThread_ = 0;
	colorFrame_ = 0;
	depthFrame_ = 0;
	nSkeletons_ = 0;
	skeletons_ = 0;
	elevationAngle_ = 0;
	rotationX_ = 0;
	rotationY_ = 0;
	rotationZ_ = 0;
	translationX_ = 0;
	translationY_ = 0;
	translationZ_ = 0;
	hierarchicalOri_ = false;
}

void KinectDevice::obtainColorFrame()
{
	if (initialized_)
	{
		if (!colorStreamOpened_) Log::write("[KinectDevice] obtainColorFrame()", "ERROR: Color stream not opened.");
		else
		{
			NUI_IMAGE_FRAME* colorFrame = new NUI_IMAGE_FRAME;
			if (SUCCEEDED(instance_->NuiImageStreamGetNextFrame(colorStreamHandle_, 200, colorFrame)))
			{
				NUI_LOCKED_RECT lockedRect;
				colorFrame->pFrameTexture->LockRect(0, &lockedRect, 0, 0);
				if(lockedRect.Pitch != 0)
				{
					uint8* buffer = basic_cast<uint8*>(lockedRect.pBits);
					for(uint32 y = 0; y < Config::canvas[Globals::INSTANCE_ID].height; y++)
					{
						for(uint32 x = 0; x < Config::canvas[Globals::INSTANCE_ID].width; x++)
						{
							uint32 stepX = (lockedRect.size/lockedRect.Pitch)/Config::canvas[Globals::INSTANCE_ID].height;
							RGBQUAD* winRGB = reinterpret_cast<RGBQUAD*>(buffer) + x*stepX;
							uint32 offset = (Config::canvas[Globals::INSTANCE_ID].width*y + x)*3;
							colorFrame_[offset + 0] = winRGB->rgbRed;
							colorFrame_[offset + 1] = winRGB->rgbGreen;
							colorFrame_[offset + 2] = winRGB->rgbBlue;
						}

						uint32 stepY = (lockedRect.Pitch/4)/Config::canvas[Globals::INSTANCE_ID].width;
						buffer += lockedRect.Pitch*stepY;
					}
				}
				colorFrame->pFrameTexture->UnlockRect(0);
				instance_->NuiImageStreamReleaseFrame(colorStreamHandle_, colorFrame);
			}
			else Log::write("[KinectDevice] obtainColorFrame()", "ERROR: Unable to get color frame.");
			delete colorFrame;
		}
	}
	else Log::write("[KinectDevice] obtainColorFrame()", "ERROR: Device not initialized.");
}

void KinectDevice::obtainDepthFrame()
{
	if (initialized_)
	{
		if (!depthStreamOpened_) Log::write("[KinectDevice] obtainDepthFrame()", "ERROR: Depth stream not opened.");
		else
		{
			NUI_IMAGE_FRAME* depthFrame = new NUI_IMAGE_FRAME;
			if (SUCCEEDED(instance_->NuiImageStreamGetNextFrame(depthStreamHandle_, 200, depthFrame)))
			{
				NUI_LOCKED_RECT lockedRect;
				depthFrame->pFrameTexture->LockRect(0, &lockedRect, 0, 0);
				if(lockedRect.Pitch != 0)
				{
					uint8* buffer = basic_cast<uint8*>(lockedRect.pBits);
					uint16* bufferRun = reinterpret_cast<uint16*>(buffer);
					for(uint32 y = 0; y < Config::canvas[Globals::INSTANCE_ID].height; y++)
					{
						for(uint32 x = 0; x < Config::canvas[Globals::INSTANCE_ID].width; x++)
						{
							Color rgbColor = depthToColor(*bufferRun, HasSkeletalEngine(instance_));
							uint32 offset = (Config::canvas[Globals::INSTANCE_ID].width*y + x)*3;
							depthFrame_[offset + 0] = basic_cast<uint8>(rgbColor.r*255.0f);
							depthFrame_[offset + 1] = basic_cast<uint8>(rgbColor.g*255.0f);
							depthFrame_[offset + 2] = basic_cast<uint8>(rgbColor.b*255.0f);

							bufferRun++;
						}
					}

				}
				depthFrame->pFrameTexture->UnlockRect(0);
				instance_->NuiImageStreamReleaseFrame(depthStreamHandle_, depthFrame);
			}
			else Log::write("[KinectDevice] obtainDepthFrame()", "ERROR: Unable to get depth frame.");
			delete depthFrame;
		}
	}
	else Log::write("[KinectDevice] obtainDepthFrame()", "ERROR: Device not initialized.");
}

void KinectDevice::obtainSkeletonsFrame()
{
	if (initialized_)
	{
		if (skeletonEnabled_)
		{
			float32* confidenceValue = 0;
			if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
			{
				std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
				confidenceValue = SharedMemoryManager::getSharedObject<float32>(segmentID, "confidenceValue");
				if (confidenceValue) *confidenceValue = 0.0f;
			}

			NUI_SKELETON_FRAME* skeletonFrame = new NUI_SKELETON_FRAME;
			if(SUCCEEDED(instance_->NuiSkeletonGetNextFrame(200, skeletonFrame)))
			{
				for(uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
				{
					skeletons_[i].clear();

					if(skeletonFrame->SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
					{
						if (skeletonMap_[i] == -1)
						{
							skeletonMap_[i] = *nSkeletons_;
							(*nSkeletons_)++;
						}
					}
					else
					{
						if (skeletonMap_[i] != -1)
						{
							for (uint32 j = 0; j < KINECT_SKELETON_COUNT; j++)
								if (skeletonMap_[j] > skeletonMap_[i]) skeletonMap_[j]--;
							skeletonMap_[i] = -1;
							(*nSkeletons_)--;
						}
					}
				}

				if(*nSkeletons_)
				{
					instance_->NuiTransformSmooth(skeletonFrame, 0);
					for (uint32 i = 0; i < KINECT_SKELETON_COUNT; i++)
					{
						if(skeletonFrame->SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED)
						{
							skeletons_[skeletonMap_[i]].setPlayerIndex(skeletonMap_[i] + 1);

							NUI_SKELETON_BONE_ORIENTATION boneOrientations[KINECT_SKELETON_JOINT_COUNT];
							std::vector<Quaternion> jointOrientations(KINECT_SKELETON_JOINT_COUNT, Quaternion());
							NuiSkeletonCalculateBoneOrientations(&(skeletonFrame->SkeletonData[i]), boneOrientations);
							for (uint32 b = 0; b < KINECT_SKELETON_JOINT_COUNT; b++)
							{
								Vector4 quat;
								if (Config::kinect[Globals::INSTANCE_ID].hierarchicalOri)
									quat = boneOrientations[b].hierarchicalRotation.rotationQuaternion;
								else quat = boneOrientations[b].absoluteRotation.rotationQuaternion;

								jointOrientations[b] = Quaternion(
									basic_cast<float32>(quat.w),
									basic_cast<float32>(quat.x),
									basic_cast<float32>(quat.y),
									basic_cast<float32>(quat.z));
							}

							for (uint32 j = 0; j < KINECT_SKELETON_JOINT_COUNT; j++)
							{
								if (skeletonFrame->SkeletonData[i].eSkeletonPositionTrackingState[j] == NUI_SKELETON_POSITION_TRACKED)
								{
									float32 jointX = skeletonFrame->SkeletonData[i].SkeletonPositions[j].x;
									float32 jointY = skeletonFrame->SkeletonData[i].SkeletonPositions[j].y;
									float32 jointZ = skeletonFrame->SkeletonData[i].SkeletonPositions[j].z;

									KinectSkeleton::KinectJoint joint;
									switch (j)
									{
									case NUI_SKELETON_POSITION_HEAD: joint = KinectSkeleton::K_HEAD; break;
									case NUI_SKELETON_POSITION_SHOULDER_LEFT: joint = KinectSkeleton::K_SHOULDER_LEFT; break;
									case NUI_SKELETON_POSITION_SHOULDER_CENTER: joint = KinectSkeleton::K_SHOULDER_CENTER; break;
									case NUI_SKELETON_POSITION_SHOULDER_RIGHT: joint = KinectSkeleton::K_SHOULDER_RIGHT; break;
									case NUI_SKELETON_POSITION_ELBOW_LEFT: joint = KinectSkeleton::K_ELBOW_LEFT; break;
									case NUI_SKELETON_POSITION_ELBOW_RIGHT: joint = KinectSkeleton::K_ELBOW_RIGHT; break;
									case NUI_SKELETON_POSITION_WRIST_LEFT: joint = KinectSkeleton::K_WRIST_LEFT; break;
									case NUI_SKELETON_POSITION_WRIST_RIGHT: joint = KinectSkeleton::K_WRIST_RIGHT; break;
									case NUI_SKELETON_POSITION_HAND_LEFT: joint = KinectSkeleton::K_HAND_LEFT; break;
									case NUI_SKELETON_POSITION_HAND_RIGHT: joint = KinectSkeleton::K_HAND_RIGHT; break;
									case NUI_SKELETON_POSITION_SPINE: joint = KinectSkeleton::K_SPINE; break;
									case NUI_SKELETON_POSITION_HIP_LEFT: joint = KinectSkeleton::K_HIP_LEFT; break;
									case NUI_SKELETON_POSITION_HIP_CENTER: joint = KinectSkeleton::K_HIP_CENTER; break;
									case NUI_SKELETON_POSITION_HIP_RIGHT: joint = KinectSkeleton::K_HIP_RIGHT; break;
									case NUI_SKELETON_POSITION_KNEE_LEFT: joint = KinectSkeleton::K_KNEE_LEFT; break;
									case NUI_SKELETON_POSITION_KNEE_RIGHT: joint = KinectSkeleton::K_KNEE_RIGHT; break;
									case NUI_SKELETON_POSITION_ANKLE_LEFT: joint = KinectSkeleton::K_ANKLE_LEFT; break;
									case NUI_SKELETON_POSITION_ANKLE_RIGHT: joint = KinectSkeleton::K_ANKLE_RIGHT; break;
									case NUI_SKELETON_POSITION_FOOT_LEFT: joint = KinectSkeleton::K_FOOT_LEFT; break;
									case NUI_SKELETON_POSITION_FOOT_RIGHT: joint = KinectSkeleton::K_FOOT_RIGHT; break;
									default: joint = KinectSkeleton::K_NONE; break;
									}

									if (joint != KinectSkeleton::K_NONE)
									{
										Quaternion jointOrientation = jointOrientations[j];
										if (!Config::kinect[Globals::INSTANCE_ID].hierarchicalOri)
											jointOrientation = jointOrientation*Quaternion(0.0f, PI32, 0.0f);
										skeletons_[skeletonMap_[i]].setJoint(
											joint,
											Point(jointX, jointY, jointZ),
											jointOrientation);
									}
								}
							}

							// Update confidence value
							if (confidenceValue) *confidenceValue += skeletons_[skeletonMap_[i]].getConfidenceValue();
						}
					}

					// Normalize confidence value
					if (confidenceValue) *confidenceValue /= *nSkeletons_;
				}
			}
			else Log::write("[KinectDevice] obtainSkeletonsFrame()", "ERROR: Unable to get skeletons.");
			delete skeletonFrame;
		}
		else Log::write("[KinectDevice] obtainSkeletonsFrame()", "ERROR: Skeleton tracking is disabled.");
	}
	else Log::write("[KinectDevice] obtainSkeletonsFrame()", "ERROR: Device not initialized.");
}

Color KinectDevice::depthToColor(uint16 depthValue, bool usesPlayer)
{
	uint16 realDepth = (depthValue&0xFFF8)>>3;
	realDepth = 255 - 256*realDepth/4095;
	uint16 playerIndex = 0;
	if (usesPlayer) playerIndex = (depthValue&0x0007);
	if (playerIndex && skeletonMap_[playerIndex - 1] != -1) playerIndex = skeletonMap_[playerIndex - 1] + 1;
	else playerIndex = 0x0007;

	Color depthColor(0.0f, 0.0f, 0.0f);
	if (playerIndex&0x0001) depthColor.r = ((float32)realDepth)/255.0f;
	if (playerIndex&0x0002) depthColor.g = ((float32)realDepth)/255.0f;
	if (playerIndex&0x0004) depthColor.b = ((float32)realDepth)/255.0f;

	return depthColor;
}

bool KinectDevice::isValid()
{
	return valid_;
}

bool KinectDevice::isInitialized()
{
	return initialized_;
}

KinectDevice::KinectStatusCode KinectDevice::getStatus()
{
	if (!instance_) return K_DEVICE_NOT_CONNECTED;

	switch (instance_->NuiStatus())
	{
	case S_OK:
		if (instance_->NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR) != S_OK)
			return K_DEVICE_IN_USE;
		instance_->NuiShutdown();
		return K_DEVICE_OK;
	case S_NUI_INITIALIZING:			return K_DEVICE_INITIALIZING;
	case E_NUI_NOTCONNECTED:			return K_DEVICE_NOT_CONNECTED;
	case E_NUI_NOTGENUINE:				return K_DEVICE_NOT_VALID;
	case E_NUI_NOTSUPPORTED:			return K_DEVICE_UNSUPPORTED;
	case E_NUI_INSUFFICIENTBANDWIDTH:	return K_DEVICE_INSUFFICIENT_BANDWIDTH;
	case E_NUI_NOTPOWERED:				return K_DEVICE_NOT_POWERED;
	case E_NUI_NOTREADY:				return K_DEVICE_NOT_READY;
	default:							return K_DEVICE_UNKNOWN;
	}
}

std::string KinectDevice::getID()
{
	if (valid_)
	{
		std::string id = wstring_cast<std::string>(std::wstring(instance_->NuiUniqueId()));
		if (id != "") return id;
	}
	
	return "Unknown";
}

int32 KinectDevice::initialize(int32 flags, bool seatedMode, bool nearMode)
{
	if (!initialized_)
	{
		if (valid_)
		{
			HRESULT result = instance_->NuiInitialize(flags);
			switch (result)
			{
			case E_NUI_SKELETAL_ENGINE_BUSY: return K_ERROR_SKELETON_IN_USE;
			case E_NUI_DEVICE_IN_USE: return K_ERROR_DEVICE_IN_USE;
			case S_OK:
				if (seatedMode) skeletonFlags_ = NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
				else skeletonFlags_ = 0;
				skeletonFrameEvent_ = CreateEvent(0, true, false, 0);
				if (HasSkeletalEngine(instance_))
				{
					skeletonEnabled_ = SUCCEEDED(instance_->NuiSkeletonTrackingEnable(skeletonFrameEvent_, skeletonFlags_));
					if (!skeletonEnabled_)
					{
						instance_->NuiShutdown();
						CloseHandle(skeletonFrameEvent_);
						skeletonFrameEvent_ = 0;
						return K_ERROR_SKELETON_TRACKING;
					}
					else KinectSkeleton::setResolution(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height);
				}
				hierarchicalOri_ = Config::kinect[Globals::INSTANCE_ID].hierarchicalOri;

				initialized_ = true;

				colorFrameEvent_ = CreateEvent(0, true, false, 0);
				depthFrameEvent_ = CreateEvent(0, true, false, 0);

				if (flags & K_USE_COLOR)
				{
					_NUI_IMAGE_TYPE type = NUI_IMAGE_TYPE_COLOR;
					_NUI_IMAGE_RESOLUTION resolution = NUI_IMAGE_RESOLUTION_INVALID;
					switch (Config::canvas[Globals::INSTANCE_ID].width)
					{
					case 80:
						if (Config::canvas[Globals::INSTANCE_ID].height == 60)
							resolution = NUI_IMAGE_RESOLUTION_640x480;
						break;
					case 320:
						if (Config::canvas[Globals::INSTANCE_ID].height == 240)
							resolution = NUI_IMAGE_RESOLUTION_640x480;
						break;
					case 640:
						if (Config::canvas[Globals::INSTANCE_ID].height == 480)
							resolution = NUI_IMAGE_RESOLUTION_640x480;
						break;
					default:
						break;
					}
					colorStreamOpened_ = SUCCEEDED(instance_->NuiImageStreamOpen(type, resolution, 0, 3, colorFrameEvent_, &colorStreamHandle_));
					if (!colorStreamOpened_) Log::write("[KinectDevice] initialize()", "ERROR: Unable to open color stream.");
				}

				if (flags & (K_USE_DEPTH|K_USE_DEPTH_AND_PLAYER))
				{
					if (nearMode) depthFlags_ = NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE;
					else depthFlags_ = 0;
					_NUI_IMAGE_TYPE type = HasSkeletalEngine(instance_)?NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX:NUI_IMAGE_TYPE_DEPTH;
					_NUI_IMAGE_RESOLUTION resolution = NUI_IMAGE_RESOLUTION_INVALID;
					switch (Config::canvas[Globals::INSTANCE_ID].width)
					{
					case 80:
						if (Config::canvas[Globals::INSTANCE_ID].height == 60)
							resolution = NUI_IMAGE_RESOLUTION_80x60;
						break;
					case 320:
						if (Config::canvas[Globals::INSTANCE_ID].height == 240)
							resolution = NUI_IMAGE_RESOLUTION_320x240;
						break;
					case 640:
						if (Config::canvas[Globals::INSTANCE_ID].height == 480)
							resolution = NUI_IMAGE_RESOLUTION_640x480;
						break;
					default:
						break;
					}
					depthStreamOpened_ = SUCCEEDED(instance_->NuiImageStreamOpen(type, resolution, 0, 3, depthFrameEvent_, &depthStreamHandle_));
					if (!depthStreamOpened_) Log::write("[KinectDevice] initialize()", "ERROR: Unable to open depth stream.");
					else instance_->NuiImageStreamSetImageFrameFlags(depthStreamHandle_, depthFlags_);
				}

				if (Config::kinect[Globals::INSTANCE_ID].rgbImage)
				{
					if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
					{
						std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
						uint32* pixels = SharedMemoryManager::createSharedObject<uint32>(segmentID, "colorPixels");
						*pixels = Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width;
						colorFrame_ = SharedMemoryManager::createSharedObject<uint8>(segmentID, "colorFrame", Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width*3);
					}
					else
					{
						colorFrame_ = new uint8[Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width*3];
						std::memset(colorFrame_, 255, Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width*3);
					}
				}
				else colorFrame_ = 0;
				if (Config::kinect[Globals::INSTANCE_ID].depthMap)
				{
					if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
					{
						std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
						uint32* pixels = SharedMemoryManager::createSharedObject<uint32>(segmentID, "depthPixels");
						*pixels = Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width;
						depthFrame_ = SharedMemoryManager::createSharedObject<uint8>(segmentID, "depthFrame", Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width*3);
					}
					else
					{
						depthFrame_ = new uint8[Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width*3];
						std::memset(depthFrame_, 255, Config::canvas[Globals::INSTANCE_ID].height*Config::canvas[Globals::INSTANCE_ID].width*3);
					}
				}
				else depthFrame_ = 0;
				if (Config::kinect[Globals::INSTANCE_ID].skeletonTracking)
				{
					if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
					{
						std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
						nSkeletons_ = SharedMemoryManager::createSharedObject<uint32>(segmentID, "nSkeletons");
						*nSkeletons_ = 0;
						skeletons_ = SharedMemoryManager::createSharedObject<KinectSkeleton>(segmentID, "skeletons", KINECT_SKELETON_COUNT);
						skeletonMap_ = std::vector<int32>(KINECT_SKELETON_COUNT, -1);

						float32* confidenceValue = SharedMemoryManager::createSharedObject<float32>(segmentID, "confidenceValue");
						*confidenceValue = 0.0f;
					}
					else
					{
						nSkeletons_ = new uint32(0);
						skeletons_ = new KinectSkeleton[KINECT_SKELETON_COUNT];
						skeletonMap_ = std::vector<int32>(KINECT_SKELETON_COUNT, -1);
					}
				}
				else
				{
					nSkeletons_ = 0;
					skeletons_ = 0;
				}

				LONG angle;
				if (SUCCEEDED(instance_->NuiCameraElevationGetAngle(&angle)))
					elevationAngle_ = basic_cast<int32>(angle);

				if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
				{
					std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
					rotationX_ = SharedMemoryManager::createSharedObject<float32>(segmentID, "rotationX");
					rotationY_ = SharedMemoryManager::createSharedObject<float32>(segmentID, "rotationY");
					rotationZ_ = SharedMemoryManager::createSharedObject<float32>(segmentID, "rotationZ");
					*rotationX_ = DEG2RAD32(basic_cast<float32>(-getElevationAngle()));
					*rotationY_ = Config::kinect[Globals::INSTANCE_ID].rotation.y;
					*rotationZ_ = Config::kinect[Globals::INSTANCE_ID].rotation.z;
					translationX_ = SharedMemoryManager::createSharedObject<float32>(segmentID, "translationX");
					translationY_ = SharedMemoryManager::createSharedObject<float32>(segmentID, "translationY");
					translationZ_ = SharedMemoryManager::createSharedObject<float32>(segmentID, "translationZ");
					*translationX_ = Config::kinect[Globals::INSTANCE_ID].translation.x;
					*translationY_ = Config::kinect[Globals::INSTANCE_ID].translation.y;
					*translationZ_ = Config::kinect[Globals::INSTANCE_ID].translation.z;
				}
				else
				{
					rotationX_ = new float32(DEG2RAD32(basic_cast<float32>(-getElevationAngle())));
					rotationY_ = new float32(Config::kinect[Globals::INSTANCE_ID].rotation.y);
					rotationZ_ = new float32(Config::kinect[Globals::INSTANCE_ID].rotation.z);
					translationX_ = new float32(Config::kinect[Globals::INSTANCE_ID].translation.x);
					translationY_ = new float32(Config::kinect[Globals::INSTANCE_ID].translation.y);
					translationZ_ = new float32(Config::kinect[Globals::INSTANCE_ID].translation.z);
				}

				processStopEvent_ = CreateEvent(0, true, false, 0);
				processThread_ = CreateThread(0, 0, processThread, this, 0, 0);

				return 0;
			default:
				return K_ERROR_UNKNOWN;
			}
		}
		else return K_ERROR_INVALID_DEVICE;
	}
	else return K_ERROR_DEVICE_ALREADY_INITIALIZED;
}

void KinectDevice::shutDown()
{
	if (initialized_)
	{
		if (processStopEvent_)
		{
			SetEvent(processStopEvent_);
			if (processThread_)
			{
				WaitForSingleObject(processThread_, INFINITE);
				CloseHandle(processThread_);
				processThread_ = 0;
			}
			CloseHandle(processStopEvent_);
			processStopEvent_ = 0;
		}

		instance_->NuiShutdown();

		if (colorFrameEvent_ && colorFrameEvent_ != INVALID_HANDLE_VALUE)
			CloseHandle(colorFrameEvent_);
		if (depthFrameEvent_ && depthFrameEvent_ != INVALID_HANDLE_VALUE)
			CloseHandle(depthFrameEvent_);
		if (skeletonFrameEvent_ && skeletonFrameEvent_ != INVALID_HANDLE_VALUE)
			CloseHandle(skeletonFrameEvent_);

		initialized_ = colorStreamOpened_ = depthStreamOpened_ = skeletonEnabled_ = false;
		colorStreamHandle_ = depthStreamHandle_ = 0;
		colorFrameEvent_ = depthFrameEvent_ = skeletonFrameEvent_ = 0;
		processStopEvent_ = processThread_ = 0;
		if (colorFrame_)
		{
			if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
			{
				std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
				SharedMemoryManager::removeSharedObject<uint32>(segmentID, "colorPixels");
				SharedMemoryManager::removeSharedObject<uint8>(segmentID, "colorFrame");
			}
			else delete[] colorFrame_;
		}
		if (depthFrame_)
		{
			if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
			{
				std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
				SharedMemoryManager::removeSharedObject<uint32>(segmentID, "depthPixels");
				SharedMemoryManager::removeSharedObject<uint8>(segmentID, "depthFrame");
			}
			else delete[] depthFrame_;
		}
		if (skeletons_)
		{
			if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
			{
				std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
				SharedMemoryManager::removeSharedObject<uint32>(segmentID, "nSkeletons");
				SharedMemoryManager::removeSharedObject<KinectSkeleton>(segmentID, "skeletons");
				SharedMemoryManager::removeSharedObject<float32>(segmentID, "confidenceValue");
			}
			else
			{
				delete nSkeletons_;
				delete[] skeletons_;
			}
		}
		colorFrame_ = 0;
		depthFrame_ = 0;
		nSkeletons_ = 0;
		skeletons_ = 0;

		if (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE)
		{
			std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
			SharedMemoryManager::removeSharedObject<float32>(segmentID, "rotationX");
			SharedMemoryManager::removeSharedObject<float32>(segmentID, "rotationY");
			SharedMemoryManager::removeSharedObject<float32>(segmentID, "rotationZ");
			SharedMemoryManager::removeSharedObject<float32>(segmentID, "translationX");
			SharedMemoryManager::removeSharedObject<float32>(segmentID, "translationY");
			SharedMemoryManager::removeSharedObject<float32>(segmentID, "translationZ");
		}
		else
		{
			delete rotationX_;
			delete rotationY_;
			delete rotationZ_;
			delete translationX_;
			delete translationY_;
			delete translationZ_;
		}
		rotationX_ = 0;
		rotationY_ = 0;
		rotationZ_ = 0;
		translationX_ = 0;
		translationY_ = 0;
		translationZ_ = 0;
	}
	else Log::write("[KinectDevice] shutDown()", "ERROR: Device not initialized.");
}

void KinectDevice::setSeatedMode(bool seated)
{
	if (initialized_)
	{
		int32 newFlags = skeletonFlags_;

		if (seated) newFlags |= NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
		else newFlags &= ~NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;

		if (newFlags != skeletonFlags_)
		{
			skeletonFlags_ = newFlags;
			if (skeletonEnabled_)
			{
				skeletonEnabled_ = SUCCEEDED(instance_->NuiSkeletonTrackingEnable(skeletonFrameEvent_, skeletonFlags_));
				if (!skeletonEnabled_)
				{
					instance_->NuiShutdown();
					CloseHandle(skeletonFrameEvent_);
					skeletonFrameEvent_ = 0;
					Log::write("[KinectDevice] setSeatedMode()", "ERROR: Unable to enable skeleton tracking.");
				}
			}
		}
	}
	else Log::write("[KinectDevice] setSeatedMode()", "ERROR: Device not initialized.");
}

void KinectDevice::setHierarchicalOri(bool hierarchical)
{
	if (initialized_) hierarchicalOri_ = hierarchical;
	else Log::write("[KinectDevice] setHierarchicalOri()", "ERROR: Device not initialized.");
}

void KinectDevice::setNearMode(bool nearMode)
{
	if (initialized_)
	{
		int32 newFlags = depthFlags_;

		if (nearMode) newFlags |= NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE;
		else newFlags &= ~NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE;

		if (newFlags != depthFlags_)
		{
			depthFlags_ = newFlags;
			if (depthStreamOpened_) instance_->NuiImageStreamSetImageFrameFlags(depthStreamHandle_, depthFlags_);
		}
	}
	else Log::write("[KinectDevice] setNearMode()", "ERROR: Device not initialized.");
}

uint8* KinectDevice::getColorFrame()
{
	if (initialized_) return colorFrame_;
	else
	{
		Log::write("[KinectDevice] getColorFrame()", "ERROR: Device not initialized.");
		return 0;
	}
}

uint8* KinectDevice::getDepthFrame()
{
	if (initialized_) return depthFrame_;
	else
	{
		Log::write("[KinectDevice] getDepthFrame()", "ERROR: Device not initialized.");
		return 0;
	}
}

uint32 KinectDevice::getNumberOfSkeletons()
{
	if (initialized_) return (nSkeletons_)?*nSkeletons_:0;
	else
	{
		Log::write("[KinectDevice] getNumberOfSkeletons()", "ERROR: Device not initialized.");
		return 0;
	}
}

KinectSkeleton* KinectDevice::getSkeletons()
{
	if (initialized_) return skeletons_;
	else
	{
		Log::write("[KinectDevice] getSkeletons()", "ERROR: Device not initialized.");
		return 0;
	}
}

bool KinectDevice::getRotation(float32& rotationX, float32& rotationY, float32& rotationZ)
{
	if (initialized_)
	{
		if (rotationX_ && rotationY_ && rotationZ_)
		{
			rotationX = *rotationX_;
			rotationY = *rotationY_;
			rotationZ = *rotationZ_;
			return true;
		}
		else return false;
	}
	else
	{
		Log::write("[KinectDevice] getRotation()", "ERROR: Device not initialized.");
		return 0;
	}
}

bool KinectDevice::getTranslation(float32& translationX, float32& translationY, float32& translationZ)
{
	if (initialized_)
	{
		if (translationX_ && translationY_ && translationZ_)
		{
			translationX = *translationX_;
			translationY = *translationY_;
			translationZ = *translationZ_;
			return true;
		}
		else return false;
	}
	else
	{
		Log::write("[KinectDevice] getTranslation()", "ERROR: Device not initialized.");
		return 0;
	}
}

int32 KinectDevice::getElevationAngle()
{
	if (initialized_) return elevationAngle_;
	else Log::write("[KinectDevice] getElevationAngle()", "ERROR: Device not initialized.");

	return 0;
}

int32 KinectDevice::getMinElevationAngle()
{
	if (initialized_)	return Config::kinect[Globals::INSTANCE_ID].minElevationAngle;
	else Log::write("[KinectDevice] getMinElevationAngle()", "ERROR: Device not initialized.");

	return 0;
}

int32 KinectDevice::getMaxElevationAngle()
{
	if (initialized_)	return Config::kinect[Globals::INSTANCE_ID].maxElevationAngle;
	else Log::write("[KinectDevice] getMaxElevationAngle()", "ERROR: Device not initialized.");

	return 0;
}

void KinectDevice::recomputeElevationAngleLimits()
{
	LONG angle;
	if (SUCCEEDED(instance_->NuiCameraElevationGetAngle(&angle)))
		elevationAngle_ = basic_cast<int32>(angle);
	if (SUCCEEDED(instance_->NuiCameraElevationSetAngle(KINECT_MIN_TILT_ANGLE)))
	{
		if (SUCCEEDED(instance_->NuiCameraElevationGetAngle(&angle)))
		{
			Config::kinect[Globals::INSTANCE_ID].minElevationAngle = basic_cast<int32>(angle);
			Config::kinect[Globals::INSTANCE_ID].maxElevationAngle = Config::kinect[Globals::INSTANCE_ID].minElevationAngle + KINECT_MAX_TILT_ANGLE - KINECT_MIN_TILT_ANGLE;
		}
	}
	if (SUCCEEDED(instance_->NuiCameraElevationSetAngle(Config::kinect[Globals::INSTANCE_ID].maxElevationAngle)))
	{
		if (SUCCEEDED(instance_->NuiCameraElevationGetAngle(&angle)))
		{
			Config::kinect[Globals::INSTANCE_ID].maxElevationAngle = basic_cast<int32>(angle);
			Config::kinect[Globals::INSTANCE_ID].minElevationAngle = Config::kinect[Globals::INSTANCE_ID].maxElevationAngle - KINECT_MAX_TILT_ANGLE + KINECT_MIN_TILT_ANGLE;
		}
	}
	if (SUCCEEDED(instance_->NuiCameraElevationSetAngle(Config::kinect[Globals::INSTANCE_ID].minElevationAngle)))
		if (SUCCEEDED(instance_->NuiCameraElevationGetAngle(&angle)))
			Config::kinect[Globals::INSTANCE_ID].minElevationAngle = basic_cast<int32>(angle);
	instance_->NuiCameraElevationSetAngle(elevationAngle_);
}

void KinectDevice::setElevationAngle(int32 angle)
{
	if (initialized_)
	{
		angle = std::max(std::min(angle, Config::kinect[Globals::INSTANCE_ID].maxElevationAngle), Config::kinect[Globals::INSTANCE_ID].minElevationAngle);
		if (SUCCEEDED(instance_->NuiCameraElevationSetAngle(basic_cast<LONG>(angle))))
			elevationAngle_ = angle;
		else Log::write("[KinectDevice] setElevationAngle()", "ERROR: Unable to set elevation angle.");
	}
	else Log::write("[KinectDevice] setElevationAngle()", "ERROR: Device not initialized.");
}

float32 KinectDevice::getFPS()
{
	if (initialized_)
	{
		if (depthStreamOpened_) return depthFPS_;
		if (skeletonEnabled_) return skeletonFPS_;
		return colorFPS_;
	}
	else
	{
		Log::write("[KinectDevice] getFPS()", "ERROR: Device not initialized.");
		return 0.0f;
	}
}

DWORD WINAPI KinectDevice::processThread(LPVOID param)
{
	KinectDevice* pThis = reinterpret_cast<KinectDevice*>(param);
	pThis->processThread();

	return 0;
}

void KinectDevice::processThread()
{
	const uint32 nEvents = 4;
	HANDLE events[nEvents] = {processStopEvent_, colorFrameEvent_, depthFrameEvent_, skeletonFrameEvent_};

	uint32 colorFrames = 0;
	uint32 depthFrames = 0;
	uint32 skeletonFrames = 0;
	colorFPS_ = 0.0f;
	depthFPS_ = 0.0f;
	skeletonFPS_ = 0.0f;
	Timer::startCount("colorFramerate");
	Timer::startCount("depthFramerate");
	Timer::startCount("skeletonFramerate");

	bool exit = false;
	while (!exit)
	{
		uint32 eventIndex = WaitForMultipleObjects(nEvents, events, false, 100);
		switch (eventIndex)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0:
			exit = true;
			break;
		case WAIT_OBJECT_0 + 1:
			obtainColorFrame();
			colorFrames++;
			break;
		case WAIT_OBJECT_0 + 2:
			obtainDepthFrame();
			depthFrames++;
			break;
		case WAIT_OBJECT_0 + 3:
			obtainSkeletonsFrame();
			skeletonFrames++;
			break;
		default:
			break;
		}

		if (rotationX_) *rotationX_ = DEG2RAD32(basic_cast<float32>(-getElevationAngle()));
		if (rotationY_) *rotationY_ = Config::kinect[Globals::INSTANCE_ID].rotation.y;
		if (rotationZ_) *rotationZ_ = Config::kinect[Globals::INSTANCE_ID].rotation.z;
		if (translationX_) *translationX_ = Config::kinect[Globals::INSTANCE_ID].translation.x;
		if (translationY_) *translationY_ = Config::kinect[Globals::INSTANCE_ID].translation.y;
		if (translationZ_) *translationZ_ = Config::kinect[Globals::INSTANCE_ID].translation.z;

		if (depthFrames)
		{
			if (Timer::getCountTime("depthFramerate") >= 1000)
			{
				depthFPS_ = basic_cast<float32>(depthFrames)*1000.0f/basic_cast<float32>(Timer::resetCount("depthFramerate"));
				depthFrames = 0;
			}
		}
		else if (skeletonFrames)
		{
			if (Timer::getCountTime("skeletonFramerate") >= 1000)
			{
				skeletonFPS_ = basic_cast<float32>(skeletonFrames)*1000.0f/basic_cast<float32>(Timer::resetCount("skeletonFramerate"));
				skeletonFrames = 0;
			}
		}
		else if (colorFrames)
		{
			if (Timer::getCountTime("colorFramerate") >= 1000)
			{
				colorFPS_ = basic_cast<float32>(colorFrames)*1000.0f/basic_cast<float32>(Timer::resetCount("colorFramerate"));
				colorFrames = 0;
			}
		}
	}

	Timer::endCount("colorFramerate");
	Timer::endCount("depthFramerate");
	Timer::endCount("skeletonFramerate");
}
