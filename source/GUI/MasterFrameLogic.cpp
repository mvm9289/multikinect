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


#include "GUI/MasterFrameLogic.h"

#include <wx/app.h>

#include "Files/Filesystem.h"
#include "GUI/GLCanvas.h"
#include "GUI/TextFileDialog.h"
#include "Kinect/KinectManager.h"
#include "Kinect/KinectSkeleton.h"
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


MasterFrameLogic::MasterFrameLogic(wxWindow* parent) : MasterFrame(parent)
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
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MasterFrameLogic::onIdle));
		break;
	case Config::RENDER_THREAD:
		renderThread_->start();
		break;
	default:
		break;
	}
}

MasterFrameLogic::~MasterFrameLogic()
{
	if (Config::system.renderMode == Config::IDLE_EVENTS)
		Disconnect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MasterFrameLogic::onIdle));
}

void MasterFrameLogic::setFPS()
{
	std::string text = "Application FPS: " + basic_cast<std::string>(appFPS_);
	if (VRPNServer::isInitialized())
		text += ("   VRPN FPS: " + basic_cast<std::string>(VRPNServer::getFPS()));

	statusBar_->SetStatusText(wxString(text.c_str(), wxConvUTF8), 0);
}

void MasterFrameLogic::onClose(wxCloseEvent& WXUNUSED(event))
{
	renderTimer_->stop();
	renderThread_->stop();
	Destroy();
	::wxExit();
}

void MasterFrameLogic::onExit(wxCommandEvent& WXUNUSED(event))
{
	renderTimer_->stop();
	renderThread_->stop();
	Destroy();
	::wxExit();
}

void MasterFrameLogic::onRestart(wxCommandEvent& event)
{
	renderTimer_->stop();
	renderThread_->stop();
	Config::system.restartApp = true;
	Destroy();
	::wxExit();
}

void MasterFrameLogic::onIdle(wxIdleEvent& event)
{
	render();
	event.RequestMore();
}

void MasterFrameLogic::onOptionsPanel(wxCommandEvent& event)
{
	updateCanvas();
}

void MasterFrameLogic::onViewLog(wxCommandEvent&)
{
	wxString logFilename = wxString(Log::getLogFilename().c_str(), wxConvUTF8);
	TextFileDialog* logFrame = new TextFileDialog(logFilename, this);
	logFrame->ShowModal();
	delete logFrame;
}

void MasterFrameLogic::onViewREADME(wxCommandEvent&)
{
	wxString readmeFilename = wxString((Filesystem::getExecutablePath() + "/README").c_str(), wxConvUTF8);
	TextFileDialog* readmeFrame = new TextFileDialog(readmeFilename, this, wxID_ANY, wxT("MultiKinect README file"));
	readmeFrame->ShowModal();
	delete readmeFrame;
}

void MasterFrameLogic::onResolutionChoice(wxCommandEvent& event)
{
	std::string resolution(resolutionChoice_->GetStringSelection().ToAscii());
	if (resolution == "320x240")
	{
		Config::canvas[Globals::INSTANCE_ID].width = 320;
		Config::canvas[Globals::INSTANCE_ID].height = 240;
	}
	else if (resolution == "640x480")
	{
		Config::canvas[Globals::INSTANCE_ID].width = 640;
		Config::canvas[Globals::INSTANCE_ID].height = 480;
	}
	else if (resolution == "800x600")
	{
		Config::canvas[Globals::INSTANCE_ID].width = 800;
		Config::canvas[Globals::INSTANCE_ID].height = 600;
	}
	else if (resolution == "1024x768")
	{
		Config::canvas[Globals::INSTANCE_ID].width = 1024;
		Config::canvas[Globals::INSTANCE_ID].height = 768;
	}
	KinectSkeleton::setResolution(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height);
	updateCanvas();
}

void MasterFrameLogic::onShowSkeletonsCheck(wxCommandEvent& event)
{
	Config::canvas[Globals::INSTANCE_ID].showSkeletons = showSkeletonsCheck_->GetValue();
}

void MasterFrameLogic::onShowOrientationsCheck(wxCommandEvent& event)
{
	Config::canvas[Globals::INSTANCE_ID].showOrientations = showOrientationsCheck_->GetValue();
}

void MasterFrameLogic::onCombineModeCheck(wxCommandEvent& event)
{
	Config::canvas[Globals::INSTANCE_ID].combineSkeletons = combineModeCheck_->GetValue();
}

void MasterFrameLogic::onRoomText(wxCommandEvent& event)
{
	float64 aux;
	if (roomOrigXCtrl_->GetValue().ToDouble(&aux)) Config::room.origin.x = basic_cast<float32>(aux*0.01);
	if (roomOrigYCtrl_->GetValue().ToDouble(&aux)) Config::room.origin.y = basic_cast<float32>(aux*0.01);
	if (roomOrigZCtrl_->GetValue().ToDouble(&aux)) Config::room.origin.z = basic_cast<float32>(aux*0.01);
	if (roomWidthCtrl_->GetValue().ToDouble(&aux)) Config::room.width = basic_cast<float32>(aux*0.01);
	if (roomHeightCtrl_->GetValue().ToDouble(&aux)) Config::room.height = basic_cast<float32>(aux*0.01);
	if (roomDepthCtrl_->GetValue().ToDouble(&aux)) Config::room.depth = basic_cast<float32>(aux*0.01);
}

void MasterFrameLogic::onLoad(wxCommandEvent& event)
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
		updateCanvas();
	}
}

void MasterFrameLogic::onSave(wxCommandEvent& event)
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

void MasterFrameLogic::onSaveAsDefault(wxCommandEvent& event)
{
	Config::save(Config::system.configPath + "/" + Config::DEFAULT_CONFIG_FILE);
}

void MasterFrameLogic::onEditConfigFile(wxCommandEvent& event)
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

void MasterFrameLogic::render()
{
	glCanvas_->render();

	appFrames_++;
	if (Timer::getCountTime("appFramerate") >= 1000)
	{
		appFPS_ = basic_cast<float32>(appFrames_)*1000.0f/basic_cast<float32>(Timer::resetCount("appFramerate"));
		appFrames_ = 0;
	}
	setFPS();
}
