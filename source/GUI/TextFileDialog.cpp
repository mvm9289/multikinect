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


#include "GUI/TextFileDialog.h"

#include "Globals/Config.h"
#include "Tools/Log.h"
#include <wx/string.h>
#include <wx/ffile.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>

using namespace MultiKinect;
using namespace GUI;
using namespace Tools;


TextFileDialog::TextFileDialog(wxString filename,
                               wxWindow* parent,
	                           wxWindowID id,
							   const wxString& title,
							   bool readOnly,
							   const wxPoint& pos,
							   const wxSize& size,
							   long style) : wxDialog(parent, id, title, pos, size, style),
											 readOnly_(readOnly)
{
	SetSizeHints(wxSize(640, 320));

	wxBoxSizer* textFileSizer;
	textFileSizer = new wxBoxSizer(wxVERTICAL);

	wxString text;
	wxFFile file(filename, wxT("r"));
	file.ReadAll(&text, wxConvUTF8);

	textCtrl_ = new wxTextCtrl(this, wxID_ANY, text, wxDefaultPosition, wxSize(-1, 250),
							   (readOnly_ ? wxTE_READONLY : 0)|wxTE_MULTILINE);
	textCtrl_->SetFont(wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, false, wxT("Consolas")));
	textCtrl_->SetFocus();
	textCtrl_->ShowNativeCaret(!readOnly_);
	textCtrl_->SetSelection(textCtrl_->XYToPosition(0, 0), textCtrl_->XYToPosition(0, 0));
	textFileSizer->Add(textCtrl_, wxSizerFlags(1).Align(1).Expand().Border(wxALL, 10));

	if (readOnly_)
	{
		okButton_ = new wxButton(this, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0);
		textFileSizer->Add(okButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 10);

		okButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TextFileDialog::onOK), NULL, this);
	}
	else
	{
		wxFlexGridSizer* buttonsSizer = new wxFlexGridSizer(2);

		saveAsDefaultButton_ = new wxButton(this, wxID_ANY, wxT("Save as default"), wxDefaultPosition, wxDefaultSize, 0);
		buttonsSizer->Add(saveAsDefaultButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 10);

		saveButton_ = new wxButton(this, wxID_ANY, wxT("Save..."), wxDefaultPosition, wxDefaultSize, 0);
		buttonsSizer->Add(saveButton_, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 10);

		textFileSizer->Add(buttonsSizer, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 0);

		saveButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TextFileDialog::onSave), NULL, this);
		saveAsDefaultButton_->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TextFileDialog::onSaveAsDefault), NULL, this);
	}

	SetSizer(textFileSizer);
	Layout();

	Centre(wxBOTH);
}

TextFileDialog::~TextFileDialog()
{
	if (readOnly_)
	{
		okButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TextFileDialog::onOK), NULL, this);
	}
	else
	{
		saveButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TextFileDialog::onSave), NULL, this);
		saveAsDefaultButton_->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(TextFileDialog::onSaveAsDefault), NULL, this);
	}
}

void TextFileDialog::onOK(wxCommandEvent& event)
{
	Close();
}

// Temporary methods to manage Config files
void TextFileDialog::onSave(wxCommandEvent& event)
{
	wxString filename = wxFileSelector(
		wxT("Select a CONF file"),
		wxString(Config::system.configPath.c_str(), wxConvUTF8),
		0,
		wxT("*.conf"),
		wxT("MultiKinet configuration file (*.conf)|*.conf"),
		wxFD_SAVE|wxFD_OVERWRITE_PROMPT,
		this);

	if(!filename.empty())
	{
		wxFFile file(filename, wxT("w"));
		file.Write(textCtrl_->GetValue(), wxConvUTF8);
		file.Flush();
		file.Close();
		Config::load(std::string(filename.mb_str()));
		Close();
	}
}

void TextFileDialog::onSaveAsDefault(wxCommandEvent& event)
{
	wxString filename = wxString((Config::system.configPath + "/" + Config::DEFAULT_CONFIG_FILE).c_str(), wxConvUTF8);
	wxFFile file(filename, wxT("w"));
	file.Write(textCtrl_->GetValue(), wxConvUTF8);
	file.Flush();
	file.Close();
	Config::load(Config::system.configPath + "/" + Config::DEFAULT_CONFIG_FILE);
	Close();
}
