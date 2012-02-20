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


#include "GUI/MainFrame.h"

#include "Globals/Definitions.h"
#include "GUI/KinectCanvas.h"
#include "Kinect/KinectManager.h"
#include "Render/RenderThread.h"
#include "Render/RenderTimer.h"
#include "Render/RenderSystem.h"
#include <wx/stattext.h>
#include <wx/sizer.h>

using namespace MultiKinect;
using namespace GUI;
using namespace Kinect;
using namespace Render;


MainFrame::MainFrame(wxWindow* parent) : RenderFrame(parent)
{
	// Status bar initialization
	statusBar_ = CreateStatusBar(1, wxST_SIZEGRIP, wxID_ANY);


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

	wxStaticText* cameraOptionsLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Kinect Camera Options:"), wxDefaultPosition, wxDefaultSize, 0);
	wxFont cameraOptionsLabelFont = cameraOptionsLabel->GetFont();
	cameraOptionsLabelFont.SetUnderlined(true);
	cameraOptionsLabel->SetFont(cameraOptionsLabelFont);
	panelSizer->Add(cameraOptionsLabel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 10);
	panelSizer->Add(0, 0, wxEXPAND, 0);

	if (Config::system.currentMode == Config::KINECT_SWITCHER)
	{
		wxStaticText* kinectDeviceLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Kinect device:"), wxDefaultPosition, wxDefaultSize, 0);
		panelSizer->Add(kinectDeviceLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

		uint32 kinectDeviceNChoices = KinectManager::getNumberOfDevices();
		kinectDeviceChoices_ = new wxString[kinectDeviceNChoices];
		for (uint32 i = 0; i < kinectDeviceNChoices; i++) kinectDeviceChoices_[i] = wxString::Format(wxT("Kinect %i"), i);
		kinectDeviceChoice_ = new wxChoice(mainPanel_, wxID_ANY, wxDefaultPosition, wxDefaultSize, kinectDeviceNChoices, kinectDeviceChoices_, 0);
		kinectDeviceChoice_->SetSelection(0);
		panelSizer->Add(kinectDeviceChoice_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);
	}
	else
	{
		kinectDeviceChoices_ = 0;
		kinectDeviceChoice_ = 0;
	}

	useColorCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Enable color image"), wxDefaultPosition, wxDefaultSize, 0);
	useColorCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].rgbImage);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(useColorCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	useDepthCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Enable depth map"), wxDefaultPosition, wxDefaultSize, 0);
	useDepthCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].depthMap);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(useDepthCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	useSkeletonCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Enable skeleton tracking"), wxDefaultPosition, wxDefaultSize, 0);
	useSkeletonCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].skeletonTracking);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(useSkeletonCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	nearModeCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Enable near mode"), wxDefaultPosition, wxDefaultSize, 0);
	nearModeCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].nearMode);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(nearModeCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	seatedModeCheck_ = new wxCheckBox(mainPanel_, wxID_ANY, wxT("Enable seated mode"), wxDefaultPosition, wxDefaultSize, 0);
	seatedModeCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].seatedMode);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(seatedModeCheck_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* jointOrientationLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Joint orientation mode:"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(jointOrientationLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	wxString jointOrientationChoices[] = { wxT("Absolute"), wxT("Hierarchical")};
	uint32 jointOrientationNChoices = sizeof(jointOrientationChoices)/sizeof(wxString);
	jointOrientationChoice_ = new wxChoice(mainPanel_, wxID_ANY, wxDefaultPosition, wxDefaultSize, jointOrientationNChoices, jointOrientationChoices, 0);
	jointOrientationChoice_->SetSelection(0);
	if (Config::kinect[Globals::INSTANCE_ID].hierarchicalOri) jointOrientationChoice_->SetSelection(1);
	panelSizer->Add(jointOrientationChoice_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* resolutionLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Stream resolution:"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(resolutionLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	wxString resolutionChoices[] = { wxT("80x60"), wxT("320x240"), wxT("640x480") };
	uint32 resolutionNChoices = sizeof(resolutionChoices)/sizeof(wxString);
	resolutionChoice_ = new wxChoice(mainPanel_, wxID_ANY, wxDefaultPosition, wxDefaultSize, resolutionNChoices, resolutionChoices, 0);
	switch (Config::canvas[Globals::INSTANCE_ID].width)
	{
	case 80:
		resolutionChoice_->SetSelection(0);
		break;
	case 320:
		resolutionChoice_->SetSelection(1);
		break;
	case 640:
		resolutionChoice_->SetSelection(2);
		break;
	default:
		break;
	}
	panelSizer->Add(resolutionChoice_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* rotationXLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Rotation X (deg):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(rotationXLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
	
	rotationXCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	rotationXCtrl_->SetEditable(false);
	rotationXCtrl_->Enable(false);
	panelSizer->Add(rotationXCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* rotationYLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Rotation Y (deg):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(rotationYLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	rotationYCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(rotationYCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* rotationZLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Rotation Z (deg):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(rotationZLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	rotationZCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(rotationZCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* translationXLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Translation X (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(translationXLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	translationXCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(translationXCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* translationYLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Translation Y (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(translationYLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	translationYCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(translationYCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* translationZLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Translation Z (cm):"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(translationZLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	translationZCtrl_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(translationZCtrl_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	wxStaticText* elevationLabel = new wxStaticText(mainPanel_, wxID_ANY, wxT("Elevation angle:"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(elevationLabel, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);

	wxBoxSizer* elevationSizer = new wxBoxSizer(wxVERTICAL);
	elevationSlider_ = new wxSlider(mainPanel_, wxID_ANY, 0, KINECT_MIN_TILT_ANGLE, KINECT_MAX_TILT_ANGLE, wxDefaultPosition, wxDefaultSize, 0);
	elevationSizer->Add(elevationSlider_, 0, wxALL, 0);

	angleText_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	angleText_->SetEditable(false);
	angleText_->Enable(false);
	elevationSizer->Add(angleText_, 0, wxALL, 0);
	panelSizer->Add(elevationSizer, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5);

	recomputeAngleLimitsButton_ = new wxButton(mainPanel_, wxID_ANY, wxT("Recompute angle limits"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(recomputeAngleLimitsButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxBOTTOM, 10);

	loadButton_ = new wxButton(mainPanel_, wxID_ANY, wxT("Load..."), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(loadButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 0);

	saveButton_ = new wxButton(mainPanel_, wxID_ANY, wxT("Save..."), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(saveButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 10);

	saveAsDefaultButton_ = new wxButton(mainPanel_, wxID_ANY, wxT("Save as default"), wxDefaultPosition, wxDefaultSize, 0);
	panelSizer->Add(0, 0, wxEXPAND, 0);
	panelSizer->Add(saveAsDefaultButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 10);

	mainPanel_->SetSizer(panelSizer);
	wxSize fittedSize = panelSizer->Fit(mainPanel_);
	mainPanel_->SetSizeHints(fittedSize, wxDefaultSize);
	mainPanel_->SetSize(fittedSize);
	mainPanel_->Layout();


	// Canvas initialization
	colorCanvas_ = new KinectCanvas(KinectCanvas::COLOR_CANVAS, this);
	depthCanvas_ = new KinectCanvas(KinectCanvas::DEPTH_CANVAS, this);
	skeletonCanvas_ = new KinectCanvas(KinectCanvas::SKELETON_CANVAS, this);


	// Render Timer initialization
	renderTimer_ = new RenderTimer();


	// Render Thread initialization
	renderThread_ = new RenderThread();


	// Frame Settings
	SetMenuBar(menuBar_);
	SetSizer(new wxGridSizer(1));
	updateCanvas();
	Centre(wxBOTH);


	// Connect Events
	Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::onClose));
	Connect(exitItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onExit));
	Connect(restartItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onRestart));
	Connect(viewOptionsItem_->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onOptionsPanel));
	Connect(viewLogItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onViewLog));
	Connect(viewREADMEItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onViewREADME));
	Connect(editConfigItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onEditConfigFile));
	if (Config::system.currentMode == Config::KINECT_SWITCHER)
		kinectDeviceChoice_->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::onKinectDeviceChoice), NULL, this);
	useColorCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onUseColorCheck), NULL, this);
	useDepthCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onUseDepthCheck), NULL, this);
	useSkeletonCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onUseSkeletonCheck), NULL, this);
	nearModeCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onNearModeCheck), NULL, this);
	seatedModeCheck_->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onSeatedModeCheck), NULL, this);
	jointOrientationChoice_->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::onJointOrientationChoice), NULL, this);
	resolutionChoice_->Connect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::onResolutionChoice), NULL, this);
	rotationYCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	rotationZCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	translationXCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	translationYCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	translationZCtrl_->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	recomputeAngleLimitsButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onRecomputeAngleLimits), NULL, this);
	loadButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onLoad), NULL, this);
	saveButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onSave), NULL, this);
	saveAsDefaultButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onSaveAsDefault), NULL, this);
	elevationSlider_->Connect(wxEVT_SCROLL_CHANGED, wxCommandEventHandler(MainFrame::onSetElevationAngle), NULL, this);
	elevationSlider_->Connect(wxEVT_SCROLL_THUMBTRACK, wxCommandEventHandler(MainFrame::onUpdateElevationAngle), NULL, this);
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::onClose));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onExit));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onRestart));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onOptionsPanel));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onViewLog));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onViewREADME));
	Disconnect(wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onEditConfigFile));
	if (Config::system.currentMode == Config::KINECT_SWITCHER)
		kinectDeviceChoice_->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::onKinectDeviceChoice), NULL, this);
	useColorCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onUseColorCheck), NULL, this);
	useDepthCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onUseDepthCheck), NULL, this);
	useSkeletonCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onUseSkeletonCheck), NULL, this);
	nearModeCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onNearModeCheck), NULL, this);
	seatedModeCheck_->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrame::onSeatedModeCheck), NULL, this);
	jointOrientationChoice_->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::onJointOrientationChoice), NULL, this);
	resolutionChoice_->Disconnect(wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(MainFrame::onResolutionChoice), NULL, this);
	rotationYCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	rotationZCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	translationXCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	translationYCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	translationZCtrl_->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(MainFrame::onKinectMatrix), NULL, this);
	recomputeAngleLimitsButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onRecomputeAngleLimits), NULL, this);
	loadButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onLoad), NULL, this);
	saveButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onSave), NULL, this);
	saveAsDefaultButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrame::onSaveAsDefault), NULL, this);
	elevationSlider_->Disconnect(wxEVT_SCROLL_CHANGED, wxCommandEventHandler(MainFrame::onSetElevationAngle), NULL, this);
	elevationSlider_->Disconnect(wxEVT_SCROLL_THUMBTRACK, wxCommandEventHandler(MainFrame::onUpdateElevationAngle), NULL, this);

	if (kinectDeviceChoices_) delete[] kinectDeviceChoices_;
	delete renderTimer_;
	delete renderThread_;
}

void MainFrame::reloadCanvas()
{
	KinectCanvas* oldColorCanvas = colorCanvas_;
	KinectCanvas* oldDepthCanvas = depthCanvas_;
	KinectCanvas* oldSkeletonCanvas = skeletonCanvas_;
	colorCanvas_ = new KinectCanvas(KinectCanvas::COLOR_CANVAS, this);
	depthCanvas_ = new KinectCanvas(KinectCanvas::DEPTH_CANVAS, this);
	skeletonCanvas_ = new KinectCanvas(KinectCanvas::SKELETON_CANVAS, this);
	SetSizer(new wxFlexGridSizer(1));
	updateCanvas();
	delete oldColorCanvas;
	delete oldDepthCanvas;
	delete oldSkeletonCanvas;
}

void MainFrame::updateCanvas()
{
	wxFlexGridSizer* frameSizer = reinterpret_cast<wxFlexGridSizer*>(GetSizer());
	frameSizer->Clear();
	frameSizer->SetCols(Config::canvas[Globals::INSTANCE_ID].rgbImage +
						Config::canvas[Globals::INSTANCE_ID].depthMap +
						Config::canvas[Globals::INSTANCE_ID].skeletonTracking +
						viewOptionsItem_->IsChecked());

	if (Config::canvas[Globals::INSTANCE_ID].rgbImage)
	{
		colorCanvas_->Show();
		frameSizer->Add(colorCanvas_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 0);
	}
	else colorCanvas_->Hide();
	if (Config::canvas[Globals::INSTANCE_ID].depthMap)
	{
		depthCanvas_->Show();
		frameSizer->Add(depthCanvas_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 0);
	}
	else depthCanvas_->Hide();
	if (Config::canvas[Globals::INSTANCE_ID].skeletonTracking)
	{
		skeletonCanvas_->Show();
		frameSizer->Add(skeletonCanvas_, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 0);
	}
	else skeletonCanvas_->Hide();

	useColorCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].rgbImage);
	useDepthCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].depthMap);
	useSkeletonCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].skeletonTracking);
	nearModeCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].nearMode);
	seatedModeCheck_->SetValue(Config::kinect[Globals::INSTANCE_ID].seatedMode);
	if (Config::kinect[Globals::INSTANCE_ID].hierarchicalOri) jointOrientationChoice_->SetSelection(1);
	else jointOrientationChoice_->SetSelection(0);

	switch (Config::canvas[Globals::INSTANCE_ID].width)
	{
	case 80:
		resolutionChoice_->SetSelection(0);
		break;
	case 320:
		resolutionChoice_->SetSelection(1);
		break;
	case 640:
		resolutionChoice_->SetSelection(2);
		break;
	default:
		break;
	}

	rotationYCtrl_->ChangeValue(wxString::Format(wxT("%f"), RAD2DEG32(Config::kinect[Globals::INSTANCE_ID].rotation.y)));
	rotationZCtrl_->ChangeValue(wxString::Format(wxT("%f"), RAD2DEG32(Config::kinect[Globals::INSTANCE_ID].rotation.z)));
	translationXCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::kinect[Globals::INSTANCE_ID].translation.x*100.0f)));
	translationYCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::kinect[Globals::INSTANCE_ID].translation.y*100.0f)));
	translationZCtrl_->ChangeValue(wxString::Format(wxT("%i"), basic_cast<int32>(Config::kinect[Globals::INSTANCE_ID].translation.z*100.0f)));

	if (viewOptionsItem_->IsChecked())
	{
		mainPanel_->Show();
		frameSizer->Add(mainPanel_, 0, wxEXPAND, 5);
	}
	else mainPanel_->Hide();

	wxSize fittedSize = frameSizer->Fit(this);
	SetSizeHints(fittedSize, wxDefaultSize);
	SetSize(fittedSize);
	Layout();
}

void MainFrame::setElevationAngle(int32 angle, int32 minAngle, int32 maxAngle)
{
	elevationSlider_->SetMin(minAngle);
	elevationSlider_->SetMax(maxAngle);
	elevationSlider_->SetValue(angle);
	angleText_->ChangeValue(wxString::Format(wxT("%i"), angle));
	rotationXCtrl_->ChangeValue(wxString::Format(wxT("%f"), -basic_cast<float32>(angle)));
}

void MainFrame::onUpdateElevationAngle(wxCommandEvent& event)
{
	angleText_->ChangeValue(wxString::Format(wxT("%i"), elevationSlider_->GetValue()));
}
