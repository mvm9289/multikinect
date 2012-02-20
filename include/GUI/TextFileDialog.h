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


#ifndef __TEXTFILEDIALOG_H__
#define __TEXTFILEDIALOG_H__

#include "Globals/Include.h"
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/button.h>


namespace MultiKinect
{
	namespace GUI
	{
		class TextFileDialog : public wxDialog 
		{
		protected:
			wxTextCtrl*	textCtrl_;
			bool		readOnly_;
			wxButton*	okButton_;
			wxButton*	saveButton_;
			wxButton*	saveAsDefaultButton_;

		public:

			TextFileDialog(wxString filename,
			               wxWindow* parent,
						   wxWindowID id = wxID_ANY,
						   const wxString& title = wxT("MultiKinect Log file"),
						   bool readOnly = true,
						   const wxPoint& pos = wxDefaultPosition,
						   const wxSize& size = wxSize(640, 320),
						   long style = wxDEFAULT_FRAME_STYLE|wxSTATIC_BORDER|wxTAB_TRAVERSAL);
			virtual ~TextFileDialog();

			void onOK(wxCommandEvent& event);
			void onSave(wxCommandEvent& event);
			void onSaveAsDefault(wxCommandEvent& event);
		};
	}
}

#endif

