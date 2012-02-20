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


#ifndef __MASTERFRAME_H__
#define __MASTERFRAME_H__

#include "Globals/Include.h"
#include "GUI/RenderFrame.h"
#include <wx/statusbr.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>


namespace MultiKinect
{
	namespace GUI
	{
		class MasterFrame : public RenderFrame
		{
		protected:
			wxStatusBar* statusBar_;
			wxMenuBar* menuBar_;
			wxMenu* fileMenu_;
			wxMenu* viewMenu_;
			wxMenu* toolsMenu_;
			wxMenuItem* viewOptionsItem_;
			wxPanel* mainPanel_;
			wxChoice* resolutionChoice_;
			wxCheckBox* showSkeletonsCheck_;
			wxCheckBox* showOrientationsCheck_;
			wxCheckBox* combineModeCheck_;
			wxTextCtrl* roomOrigXCtrl_;
			wxTextCtrl* roomOrigYCtrl_;
			wxTextCtrl* roomOrigZCtrl_;
			wxTextCtrl* roomWidthCtrl_;
			wxTextCtrl* roomHeightCtrl_;
			wxTextCtrl* roomDepthCtrl_;
			wxButton* loadButton_;
			wxButton* saveButton_;
			wxButton* saveAsDefaultButton_;
			GLCanvas* glCanvas_;
			RenderTimer* renderTimer_;
			RenderThread* renderThread_;

			void updateCanvas();

			// Virtual event handlers, overide them in your derived class
			virtual void onClose(wxCloseEvent& event) { event.Skip(); }
			virtual void onExit(wxCommandEvent& event) { event.Skip(); }
			virtual void onRestart(wxCommandEvent& event) { event.Skip(); }
			virtual void onIdle(wxIdleEvent& event) { event.Skip(); }
			virtual void onOptionsPanel(wxCommandEvent& event) { event.Skip(); }
			virtual void onViewLog(wxCommandEvent& event) { event.Skip(); }
			virtual void onViewREADME(wxCommandEvent& event) { event.Skip(); }
			virtual void onResolutionChoice(wxCommandEvent& event) { event.Skip(); }
			virtual void onShowSkeletonsCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onShowOrientationsCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onCombineModeCheck(wxCommandEvent& event) { event.Skip(); }
			virtual void onRoomText(wxCommandEvent& event) { event.Skip(); }
			virtual void onLoad(wxCommandEvent& event) { event.Skip(); }
			virtual void onSave(wxCommandEvent& event) { event.Skip(); }
			virtual void onSaveAsDefault(wxCommandEvent& event) { event.Skip(); }
			virtual void onEditConfigFile(wxCommandEvent& event) { event.Skip(); }

		public:
			MasterFrame(wxWindow* parent);
			virtual ~MasterFrame();

			virtual void render() = 0;
		};
	}
}

#endif
