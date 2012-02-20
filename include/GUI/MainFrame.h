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


#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#include "Globals/Include.h"
#include "GUI/RenderFrame.h"
#include <wx/statusbr.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/slider.h>
#include <wx/textctrl.h>


namespace MultiKinect
{
	namespace GUI
	{
		class MainFrame : public RenderFrame
		{
		protected:
			wxStatusBar* statusBar_;
			wxMenuBar* menuBar_;
			wxMenu* fileMenu_;
			wxMenu* viewMenu_;
			wxMenu* toolsMenu_;
			wxMenuItem* viewOptionsItem_;
			wxPanel* mainPanel_;
			wxString* kinectDeviceChoices_;
			wxChoice* kinectDeviceChoice_;
			wxCheckBox* useColorCheck_;
			wxCheckBox* useDepthCheck_;
			wxCheckBox* useSkeletonCheck_;
			wxCheckBox* nearModeCheck_;
			wxCheckBox* seatedModeCheck_;
			wxChoice* jointOrientationChoice_;
			wxChoice* resolutionChoice_;
			wxTextCtrl* rotationXCtrl_;
			wxTextCtrl* rotationYCtrl_;
			wxTextCtrl* rotationZCtrl_;
			wxTextCtrl* translationXCtrl_;
			wxTextCtrl* translationYCtrl_;
			wxTextCtrl* translationZCtrl_;
			wxSlider* elevationSlider_;
			wxTextCtrl* angleText_;
			wxButton* recomputeAngleLimitsButton_;
			wxButton* loadButton_;
			wxButton* saveButton_;
			wxButton* saveAsDefaultButton_;
			KinectCanvas* colorCanvas_;
			KinectCanvas* depthCanvas_;
			KinectCanvas* skeletonCanvas_;
			RenderTimer* renderTimer_;
			RenderThread* renderThread_;

			void reloadCanvas();
			void updateCanvas();

			// Virtual event handlers, overide them in your derived class
			virtual void onClose(wxCloseEvent& event) { event.Skip(); }
			virtual void onExit(wxCommandEvent& event) { event.Skip(); }
			virtual void onRestart(wxCommandEvent& event) { event.Skip(); }
			virtual void onIdle(wxIdleEvent& event) { event.Skip(); }
			virtual void onOptionsPanel(wxCommandEvent& event) { event.Skip(); }
			virtual void onViewLog(wxCommandEvent& event) { event.Skip(); }
			virtual void onViewREADME(wxCommandEvent& event) { event.Skip(); }
			virtual void onKinectDeviceChoice(wxCommandEvent& event) { event.Skip(); }
			virtual void onUseColorCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onUseDepthCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onUseSkeletonCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onNearModeCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onSeatedModeCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onJointOrientationChoice(wxCommandEvent& event) { event.Skip(); }
			virtual void onResolutionChoice(wxCommandEvent& event) { event.Skip(); }
			virtual void onKinectMatrix(wxCommandEvent& event) { event.Skip(); }
			virtual void onLoad(wxCommandEvent& event) { event.Skip(); }
			virtual void onSave(wxCommandEvent& event) { event.Skip(); }
			virtual void onSaveAsDefault(wxCommandEvent& event) { event.Skip(); }
			virtual void onSetElevationAngle(wxCommandEvent& event) { event.Skip(); }
			virtual void onUpdateElevationAngle(wxCommandEvent& event);
			virtual void onRecomputeAngleLimits(wxCommandEvent& event) { event.Skip(); }
			virtual void onEditConfigFile(wxCommandEvent& event) { event.Skip(); }

		public:
			MainFrame(wxWindow* parent);
			virtual ~MainFrame();

			void setElevationAngle(int32 angle, int32 minAngle, int32 maxAngle);
			virtual void render() = 0;
		};
	}
}

#endif
