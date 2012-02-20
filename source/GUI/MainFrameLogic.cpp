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


#include "GUI/MainFrameLogic.h"

#include <wx/app.h>

#include "Files/Filesystem.h"
#include "Globals/Definitions.h"
#include "GUI/KinectCanvas.h"
#include "GUI/TextFileDialog.h"
#include "Kinect/KinectDevice.h"
#include "Kinect/KinectManager.h"
#include "Interprocess/SharedMemoryManager.h"
#include "Render/RenderSystem.h"
#include "Render/RenderThread.h"
#include "Render/RenderTimer.h"
#include "Tools/Log.h"
#include "Tools/Timer.h"
#include "VRPN/VRPNServer.h"
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

using namespace MultiKinect;
using namespace GUI;
using namespace Kinect;
using namespace Interprocess;
using namespace Render;
using namespace Tools;
using namespace VRPN;


MainFrameLogic::MainFrameLogic(wxWindow* parent) : MainFrame(parent)
{
	appFrames_ = 0;
	appFPS_ = 0.0f;
	Timer::startCount("appFramerate");

	renderTimer_->setMainFrame(this);
	renderThread_->setMainFrame(this);
	switch (Config::system.renderMode)
	{
	case Config::RENDER_TIMER:
		renderTimer_->start();
		break;
	case Config::IDLE_EVENTS:
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MainFrameLogic::onIdle));
		break;
	case Config::RENDER_THREAD:
		renderThread_->start();
		break;
	default:
		break;
	}

	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
	{
		KinectDevice* device = RenderSystem::getDevice();
		setElevationAngle(device->getElevationAngle(), device->getMinElevationAngle(), device->getMaxElevationAngle());
	}
	else setElevationAngle(0, 0, 0);
}

MainFrameLogic::~MainFrameLogic()
{
	if (Config::system.renderMode == Config::IDLE_EVENTS)
		Disconnect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MainFrameLogic::onIdle));
}

void MainFrameLogic::setFPS()
{
	std::string text = "Application FPS: " + basic_cast<std::string>(appFPS_);
	if (RenderSystem::isInitialized())
		text += ("   Kinect FPS: " + basic_cast<std::string>(RenderSystem::getKinectFPS()));
	if (VRPNServer::isInitialized())
		text += ("   VRPN FPS: " + basic_cast<std::string>(VRPNServer::getFPS()));

	statusBar_->SetStatusText(wxString(text.c_str(), wxConvUTF8), 0);
}

void MainFrameLogic::onClose(wxCloseEvent& WXUNUSED(event))
{
	renderTimer_->stop();
	renderThread_->stop();
	Destroy();
	::wxExit();
}

void MainFrameLogic::onExit(wxCommandEvent& WXUNUSED(event))
{
	renderTimer_->stop();
	renderThread_->stop();
	Destroy();
	::wxExit();
}

void MainFrameLogic::onRestart(wxCommandEvent& event)
{
	renderTimer_->stop();
	renderThread_->stop();
	Config::system.restartApp = true;
	Destroy();
	::wxExit();
}

void MainFrameLogic::onIdle(wxIdleEvent& event)
{
	render();
	event.RequestMore();
}

void MainFrameLogic::onOptionsPanel(wxCommandEvent& event)
{
	updateCanvas();
}

void MainFrameLogic::onViewLog(wxCommandEvent&)
{
	wxString logFilename = wxString(Log::getLogFilename().c_str(), wxConvUTF8);
	TextFileDialog* logFrame = new TextFileDialog(logFilename, this);
	logFrame->ShowModal();
	delete logFrame;
}

void MainFrameLogic::onViewREADME(wxCommandEvent&)
{
	wxString readmeFilename = wxString((Filesystem::getExecutablePath() + "/README").c_str(), wxConvUTF8);
	TextFileDialog* readmeFrame = new TextFileDialog(readmeFilename, this, wxID_ANY, wxT("MultiKinect README file"));
	readmeFrame->ShowModal();
	delete readmeFrame;
}

void MainFrameLogic::onKinectDeviceChoice(wxCommandEvent& event)
{
	KinectDevice* newDevice = KinectManager::getDevicePointer(kinectDeviceChoice_->GetSelection());
	std::string newDeviceID = newDevice->getID();
	std::string oldDeviceID = RenderSystem::getDevice()->getID();
	if (newDeviceID != oldDeviceID)
	{
		Globals::INSTANCE_ID = newDeviceID;
		RenderSystem::destroy();
		RenderSystem::initialize(RenderSystem::RS_LOCAL_DEVICE, newDevice);
		reloadCanvas();
		if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
		{
			KinectDevice* device = RenderSystem::getDevice();
			setElevationAngle(device->getElevationAngle(), device->getMinElevationAngle(), device->getMaxElevationAngle());
		}
		else setElevationAngle(0, 0, 0);
	}
	else delete newDevice;
}

void MainFrameLogic::onUseColorCheck(wxCommandEvent& event)
{
	bool restartDevice = false;
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
	{
		restartDevice = true;
		RenderSystem::destroy();
	}
	Config::canvas[Globals::INSTANCE_ID].rgbImage = useColorCheck_->GetValue();
	if (restartDevice) RenderSystem::initialize(RenderSystem::RS_LOCAL_DEVICE, KinectManager::getDevicePointer(Globals::INSTANCE_ID));
	reloadCanvas();
}

void MainFrameLogic::onUseDepthCheck(wxCommandEvent& event)
{
	bool restartDevice = false;
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
	{
		restartDevice = true;
		RenderSystem::destroy();
	}
	Config::canvas[Globals::INSTANCE_ID].depthMap = useDepthCheck_->GetValue();
	if (restartDevice) RenderSystem::initialize(RenderSystem::RS_LOCAL_DEVICE, KinectManager::getDevicePointer(Globals::INSTANCE_ID));
	reloadCanvas();
}

void MainFrameLogic::onUseSkeletonCheck(wxCommandEvent& event)
{
	bool restartDevice = false;
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
	{
		restartDevice = true;
		RenderSystem::destroy();
	}
	Config::canvas[Globals::INSTANCE_ID].skeletonTracking = useSkeletonCheck_->GetValue();
	if (restartDevice) RenderSystem::initialize(RenderSystem::RS_LOCAL_DEVICE, KinectManager::getDevicePointer(Globals::INSTANCE_ID));
	reloadCanvas();
}

void MainFrameLogic::onNearModeCheck(wxCommandEvent& event)
{
	Config::kinect[Globals::INSTANCE_ID].nearMode = nearModeCheck_->GetValue();
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
		RenderSystem::getDevice()->setNearMode(Config::kinect[Globals::INSTANCE_ID].nearMode);
	reloadCanvas();
}

void MainFrameLogic::onSeatedModeCheck(wxCommandEvent& event)
{
	Config::kinect[Globals::INSTANCE_ID].seatedMode = seatedModeCheck_->GetValue();
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
		RenderSystem::getDevice()->setSeatedMode(Config::kinect[Globals::INSTANCE_ID].seatedMode);
	reloadCanvas();
}

void MainFrameLogic::onJointOrientationChoice(wxCommandEvent& event)
{
	Config::kinect[Globals::INSTANCE_ID].hierarchicalOri = (jointOrientationChoice_->GetSelection() == 1);
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
		RenderSystem::getDevice()->setHierarchicalOri(Config::kinect[Globals::INSTANCE_ID].hierarchicalOri);
	reloadCanvas();
}

void MainFrameLogic::onResolutionChoice(wxCommandEvent& event)
{
	bool restartDevice = false;
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
	{
		restartDevice = true;
		RenderSystem::destroy();
	}
	std::string resolution(resolutionChoice_->GetStringSelection().ToAscii());
	if (resolution == "80x60")
	{
		Config::canvas[Globals::INSTANCE_ID].width = 80;
		Config::canvas[Globals::INSTANCE_ID].height = 60;
	}
	else if (resolution == "320x240")
	{
		Config::canvas[Globals::INSTANCE_ID].width = 320;
		Config::canvas[Globals::INSTANCE_ID].height = 240;
	}
	else if (resolution == "640x480")
	{
		Config::canvas[Globals::INSTANCE_ID].width = 640;
		Config::canvas[Globals::INSTANCE_ID].height = 480;
	}
	if (restartDevice) RenderSystem::initialize(RenderSystem::RS_LOCAL_DEVICE, KinectManager::getDevicePointer(Globals::INSTANCE_ID));
	reloadCanvas();
}

void MainFrameLogic::onKinectMatrix(wxCommandEvent& event)
{
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
	{
		float64 aux;
		if (rotationYCtrl_->GetValue().ToDouble(&aux)) Config::kinect[Globals::INSTANCE_ID].rotation.y = basic_cast<float32>(DEG2RAD64(aux));
		if (rotationZCtrl_->GetValue().ToDouble(&aux)) Config::kinect[Globals::INSTANCE_ID].rotation.z = basic_cast<float32>(DEG2RAD64(aux));
		if (translationXCtrl_->GetValue().ToDouble(&aux)) Config::kinect[Globals::INSTANCE_ID].translation.x = basic_cast<float32>(aux*0.01);
		if (translationYCtrl_->GetValue().ToDouble(&aux)) Config::kinect[Globals::INSTANCE_ID].translation.y = basic_cast<float32>(aux*0.01);
		if (translationZCtrl_->GetValue().ToDouble(&aux)) Config::kinect[Globals::INSTANCE_ID].translation.z = basic_cast<float32>(aux*0.01);
	}
}

void MainFrameLogic::onLoad(wxCommandEvent& event)
{
	wxString filename = wxFileSelector(
		wxT("Select a CONF file"),
		wxString(Config::system.configPath.c_str(), wxConvUTF8),
		0,
		wxT("*.conf"),
		wxT("MultiKinet configuration file (*.conf)|*.conf"),
		wxFD_OPEN|wxFD_FILE_MUST_EXIST,
		this);

	if(!filename.empty())
	{
		Config::load(std::string(filename.mb_str()));
		reloadCanvas();
	}
}

void MainFrameLogic::onSave(wxCommandEvent& event)
{
	wxString filename = wxFileSelector(
		wxT("Select a CONF file"),
		wxString(Config::system.configPath.c_str(), wxConvUTF8),
		0,
		wxT("*.conf"),
		wxT("MultiKinet configuration file (*.conf)|*.conf"),
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT,
		this);

	if(!filename.empty()) Config::save(std::string(filename.mb_str()));
}

void MainFrameLogic::onSaveAsDefault(wxCommandEvent& event)
{
	Config::save(Config::system.configPath + "/" + Config::DEFAULT_CONFIG_FILE);
}

void MainFrameLogic::onSetElevationAngle(wxCommandEvent& event)
{
	angleText_->SetValue(wxString::Format(wxT("%i"), elevationSlider_->GetValue()));
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
		RenderSystem::getDevice()->setElevationAngle(elevationSlider_->GetValue());
	rotationXCtrl_->SetValue(wxString::Format(wxT("%f"), -basic_cast<float32>(elevationSlider_->GetValue())));
}

void MainFrameLogic::onRecomputeAngleLimits(wxCommandEvent& event)
{
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice())
	{
		KinectDevice* device = RenderSystem::getDevice();
		device->recomputeElevationAngleLimits();
		setElevationAngle(device->getElevationAngle(), device->getMinElevationAngle(), device->getMaxElevationAngle());
	}
	else setElevationAngle(0, 0, 0);
}

void MainFrameLogic::onEditConfigFile(wxCommandEvent& event)
{
	if (Globals::LAST_CONFIGURATION != "" && Filesystem::fileExists(Globals::LAST_CONFIGURATION))
	{
		wxString configFilename = wxString(Globals::LAST_CONFIGURATION.c_str(), wxConvUTF8);
		TextFileDialog* configFrame = new TextFileDialog(configFilename, this, wxID_ANY, wxT("MultiKinect Configuration file"), false);
		configFrame->ShowModal();
		updateCanvas();
		delete configFrame;
	}
	else
	{
		std::string errmsg = std::string("The configuration file you are trying to edit does not exist or is "
			"no longer available. Please, save your current configuration through "
			"the options panel ('View' menu).");
		wxMessageDialog(this, wxString(errmsg.c_str(), wxConvUTF8), wxT("File not found"), wxOK|wxICON_ERROR|wxCENTER).ShowModal();
	}
}

void MainFrameLogic::render()
{
	// Check if exit message has been received
	if (RenderSystem::isInitialized() && RenderSystem::hasDevice() && (Config::system.currentMode == Config::KINECT_SINGLE_DEVICE))
	{
		std::string segmentID = KinectManager::reformatDeviceID(Globals::INSTANCE_ID);
		bool* flag = SharedMemoryManager::getSharedObject<bool>(segmentID, "CLOSE_SIGNAL");
		if (flag)
		{
			SharedMemoryManager::removeSharedObject<bool>(segmentID, "CLOSE_SIGNAL");
			wxPostEvent(this, wxCloseEvent(wxEVT_CLOSE_WINDOW));
		}
	}

	if (Config::canvas[Globals::INSTANCE_ID].rgbImage) colorCanvas_->render();
	if (Config::canvas[Globals::INSTANCE_ID].depthMap) depthCanvas_->render();
	if (Config::canvas[Globals::INSTANCE_ID].skeletonTracking) skeletonCanvas_->render();

	appFrames_++;
	if (Timer::getCountTime("appFramerate") >= 1000)
	{
		appFPS_ = basic_cast<float32>(appFrames_)*1000.0f/basic_cast<float32>(Timer::resetCount("appFramerate"));
		appFrames_ = 0;
	}
	setFPS();
}
