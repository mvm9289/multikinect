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


#include "GUI/MasterFrame.h"

#include "GUI/GLCanvas.h"
#include "Kinect/KinectSkeleton.h"
#include "Render/RenderThread.h"
#include "Render/RenderTimer.h"
#include <wx/stattext.h>
#include <wx/sizer.h>

using namespace MultiKinect;
using namespace GUI;
using namespace Kinect;
using namespace Render;


MasterFrame::MasterFrame(wxWindow* parent) : RenderFrame(
												parent,
												wxID_ANY,
												wxString(Config::system.appTitle.c_str(), wxConvUTF8),
												wxDefaultPosition,
												wxDefaultSize,
												wxSYSTEM_MENU|wxMINIMIZE_BOX|wxCLOSE_BOX|wxCAPTION|wxCLIP_CHILDREN|wxTAB_TRAVERSAL)
{
	// Status bar initialization
	statusBar_ = CreateStatusBar(1, wxST_NO_AUTORESIZE, wxID_ANY);


	// Menu bar initialization
	menuBar_ = new wxMenuBar(0);

	fileMenu_ = new wxMenu();

	wxMenuItem* restartItem;
	restartItem = new wxMenuItem(fileMenu_, wxID_ANY, wxString(wxT("&Restart")), wxEmptyString, wxITEM_NORMAL);
	fileMenu_->Append(restartItem);

	wxMenuItem* exitItem;
	exitItem = new wxMenuItem(fileMenu_, wxID_ANY, wxString(wxT("&Exit")), wxEmptyString, wxITEM_NORMAL);
	fileMenu_->Append(exitItem);

	menuBar_->Append(fileMenu_, wxT("&File"));


	viewMenu_ = new wxMenu();

	viewOptionsItem_ = new wxMenuItem(viewMenu_, wxID_ANY, wxString(wxT("Options panel")), wxEmptyString, wxITEM_CHECK);
	viewOptionsItem_->Check(false);
	viewMenu_->Append(viewOptionsItem_);
	wxMenuItem* viewLogItem;
	viewLogItem = new wxMenuItem(viewMenu_, wxID_ANY, wxString(wxT("View log...")), wxEmptyString, wxITEM_NORMAL);
	viewMenu_->Append(viewLogItem);
	wxMenuItem* viewREADMEItem;
	viewREADMEItem = new wxMenuItem(viewMenu_, wxID_ANY, wxString(wxT("View README...")), wxEmptyString, wxITEM_NORMAL);
	viewMenu_->Append(viewREADMEItem);

	menuBar_->Append(viewMenu_, wxT("&View"));


	toolsMenu_ = new wxMenu();

	wxMenuItem* editConfigItem = new wxMenuItem(toolsMenu_, wxID_ANY, wxString(wxT("Edit configuration file...")), wxEmptyString, wxITEM_NORMAL);
	toolsMenu_->Append(editConfigItem);

	menuBar_->Append(toolsMenu_, wxT("&Tools"));


	// Main panel initialization
	mainPanel_ = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTAB_TRAVERSAL);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* cameraOptionsLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Skeleton Viewer Options:"), wxDefaultPosition, wxDefaultSize, 0);
	wxFont cameraFont = cameraOptionsLabel->GetFont();
	cameraFont.SetUnderlined(true);
	cameraOptionsLabel->SetFont(cameraFont);
	panelSizer->Add(cameraOptionsLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);
	panelSizer->Add(0, 0, wxEXPAND, 0);

	wxStaticText* resolutionLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Stream resolution:"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(resolutionLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	wxString resolutionChoices[] = { wxT("320x240"), wxT("640x480"), wxT("800x600"), wxT("1024x768") };
	uint32 resolutionNChoices = sizeof(resolutionChoices)/sizeof(wxString);
	resolutionChoice_ = new wxChoice(mainPanel_, wxID_ANY, wxDefaultPosition, wxDefaultSize, resolutionNChoices, resolutionChoices, 0);
	switch (Config::canvas[Globals::INSTANCE_ID].width)
	{
	case 320:
		resolutionChoice_->SetSelection(0);
		break;
	case 640:
		resolutionChoice_->SetSelection(1);
		break;
	case 800:
		resolutionChoice_->SetSelection(2);
		break;
	case 1024:
		resolutionChoice_->SetSelection(3);
		break;
	default:
		break;
	}
	KinectSkeleton::setResolution(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height);
	panelSizer->Add(resolutionChoice_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	showSkeletonsCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Show skeletons"), wxDefaultPosition, wxDefaultSize, 0);
	showSkeletonsCheck_->SetValue(Config::canvas[Globals::INSTANCE_ID].showSkeletons);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(showSkeletonsCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	showOrientationsCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Show joints orientation"), wxDefaultPosition, wxDefaultSize, 0);
	showOrientationsCheck_->SetValue(Config::canvas[Globals::INSTANCE_ID].showOrientations);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(showOrientationsCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	if (Config::system.currentMode == Config::KINECT_MASTER)
	{
		combineModeCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Combine skeletons"), wxDefaultPosition, wxDefaultSize, 0);
		combineModeCheck_->SetValue(Config::canvas[Globals::INSTANCE_ID].combineSkeletons);
		panelSizer->Add(0, 0, wxEXPAND, 0);
		panelSizer->Add(combineModeCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	}
	else combineModeCheck_ = 0;

	wxStaticText* roomOrigXLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Room origin X (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomOrigXLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	roomOrigXCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.origin.x*100.0f)), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomOrigXCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	wxStaticText* roomOrigYLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Room origin Y (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomOrigYLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	roomOrigYCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.origin.y*100.0f)), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomOrigYCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	wxStaticText* roomOrigZLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Room origin Z (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomOrigZLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	roomOrigZCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.origin.z*100.0f)), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomOrigZCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	wxStaticText* roomWidthLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Room width (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomWidthLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	roomWidthCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.height*100.0f)), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomWidthCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	wxStaticText* roomHeightLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Room height (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomHeightLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	roomHeightCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.height*100.0f)), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomHeightCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	wxStaticText* roomDepthLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Room depth (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomDepthLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	roomDepthCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.depth*100.0f)), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(roomDepthCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	loadButton_ = new wxButton(mainPanel_, wxID_ANY, wxT("Load..."), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(loadButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 0);

	saveButton_ = new wxButton(mainPanel_, wxID_ANY, wxT("Save..."), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(saveButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 10);

	saveAsDefaultButton_ = new wxButton(mainPanel_, wxID_ANY, wxT("Save as default"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(saveAsDefaultButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 10);

	mainPanel_->SetSizer(panelSizer);
	mainPanel_->Layout();
	panelSizer->Fit(mainPanel_);


	// Canvas initialization
	int32 attribList[] =
	{
		WX_GL_RGBA,
		WX_GL_DOUBLEBUFFER,
		WX_GL_SAMPLE_BUFFERS, GL_TRUE,
		WX_GL_SAMPLES, 16,
		WX_GL_DEPTH_SIZE, 24,
		0, 0
	};
	glCanvas_ = new GLCanvas(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER, wxT("GLCanvas"), attribList);


	// Render Timer initialization
	renderTimer_ = new RenderTimer();


	// Render Thread initialization
	renderThread_ = new RenderThread();


	// Frame Settings
	SetMenuBar(menuBar_);
	SetSizer(new wxBoxSizer(wxHORIZONTAL));
	updateCanvas();
	Centre(wxBOTH);


	// Connect Events
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MasterFrame::onClose));
	Connect(exitItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onExit));
	Connect(restartItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onRestart));
	Connect(viewOptionsItem_->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onOptionsPanel));
	Connect(viewLogItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onViewLog));
	Connect(viewREADMEItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onViewREADME));
	Connect(editConfigItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onEditConfigFile));
	resolutionChoice_->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MasterFrame::onResolutionChoice), NULL, this);
	showSkeletonsCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MasterFrame::onShowSkeletonsCheck), NULL, this);
	showOrientationsCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MasterFrame::onShowOrientationsCheck), NULL, this);
	if (Config::system.currentMode == Config::KINECT_MASTER)
		combineModeCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MasterFrame::onCombineModeCheck), NULL, this);
	roomOrigXCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomOrigYCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomOrigZCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomWidthCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomHeightCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomDepthCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	loadButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MasterFrame::onLoad), NULL, this);
	saveButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MasterFrame::onSave), NULL, this);
	saveAsDefaultButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MasterFrame::onSaveAsDefault), NULL, this);
}

MasterFrame::~MasterFrame()
{
	// Disconnect Events
	Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MasterFrame::onClose));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onExit));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onRestart));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onOptionsPanel));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onViewLog));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onViewREADME));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MasterFrame::onEditConfigFile));
	resolutionChoice_->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MasterFrame::onResolutionChoice), NULL, this);
	showSkeletonsCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MasterFrame::onShowSkeletonsCheck), NULL, this);
	showOrientationsCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MasterFrame::onShowOrientationsCheck), NULL, this);
	if (Config::system.currentMode == Config::KINECT_MASTER)
		combineModeCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MasterFrame::onCombineModeCheck), NULL, this);
	roomOrigXCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomOrigYCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomOrigZCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomWidthCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomHeightCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	roomDepthCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MasterFrame::onRoomText), NULL, this);
	loadButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MasterFrame::onLoad), NULL, this);
	saveButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MasterFrame::onSave), NULL, this);
	saveAsDefaultButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MasterFrame::onSaveAsDefault), NULL, this);

	delete renderTimer_;
	delete renderThread_;
}

void MasterFrame::updateCanvas()
{
	glCanvas_->SetSizeHints(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height);

	wxBoxSizer* frameSizer = reinterpret_cast<wxBoxSizer*>(GetSizer());
	frameSizer->Clear();
	frameSizer->Add(glCanvas_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 0);

	switch (Config::canvas[Globals::INSTANCE_ID].width)
	{
	case 320:
		resolutionChoice_->SetSelection(0);
		break;
	case 640:
		resolutionChoice_->SetSelection(1);
		break;
	case 800:
		resolutionChoice_->SetSelection(2);
		break;
	case 1024:
		resolutionChoice_->SetSelection(3);
		break;
	default:
		break;
	}
	KinectSkeleton::setResolution(Config::canvas[Globals::INSTANCE_ID].width, Config::canvas[Globals::INSTANCE_ID].height);

	showSkeletonsCheck_->SetValue(Config::canvas[Globals::INSTANCE_ID].showSkeletons);
	showOrientationsCheck_->SetValue(Config::canvas[Globals::INSTANCE_ID].showOrientations);
	if (combineModeCheck_) combineModeCheck_->SetValue(Config::canvas[Globals::INSTANCE_ID].combineSkeletons);

	roomOrigXCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.origin.x*100.0f)));
	roomOrigYCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.origin.y*100.0f)));
	roomOrigZCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.origin.z*100.0f)));
	roomWidthCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.width*100.0f)));
	roomHeightCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.height*100.0f)));
	roomDepthCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::room.depth*100.0f)));

	if (viewOptionsItem_->IsChecked())
	{
		mainPanel_->Show();
		frameSizer->Add(mainPanel_, 0, wxEXPAND, 5);
	}
	else mainPanel_->Hide();
	wxSize fittedSize = frameSizer->Fit(this);
	SetSizeHints(fittedSize, fittedSize);
	SetSize(fittedSize);
	Layout();
}
