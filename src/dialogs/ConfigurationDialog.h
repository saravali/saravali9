/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ConfigurationDialog.h
 Release    9.0

 This is free and unencumbered software released into the public domain.

 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a compiled
 binary, for any purpose, commercial or non-commercial, and by any
 means.

 In jurisdictions that recognize copyright laws, the author or authors
 of this software dedicate any and all copyright interest in the
 software to the public domain. We make this dedication for the benefit
 of the public at large and to the detriment of our heirs and
 successors. We intend this dedication to be an overt act of
 relinquishment in perpetuity of all present and future rights to this
 software under copyright law.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

 For more information, please refer to <http://unlicense.org/>

************************************************************************/

#ifndef CONFIGURATION_DIALOG_H
#define CONFIGURATION_DIALOG_H

#include "bootstrap.h"
#include "Conflet.h"
#include "ConfletFactory.h"

#include <wx/dialog.h>

#include <list>
#include <vector>

class wxButton;
class wxStaticLine;
class wxTreebook;

enum class ConfletSet;

/*************************************************//**
*
* Data holder for a single page of ConfigurationDialog
*
******************************************************/
struct ConfigdialogPage
{
	ConfigdialogPage( const ConfletIdTreenode &node, wxPanel *wxpanel )
		: node( node ),
		wxpanel( wxpanel ),
		conflet( nullptr )
	{
	}

	const ConfletIdTreenode node;
	wxPanel *wxpanel;
	Conflet *conflet;
};

/*************************************************//**
*
* 
*
******************************************************/
class ConfigurationDialog : public wxDialog
{
public:

	ConfigurationDialog( wxWindow* parent, const ConfletSet& );

	~ConfigurationDialog();

protected:

	Conflet *showPanel( const int& );

	void OnOK( wxCommandEvent& );
	void OnCancel( wxCommandEvent& ) { EndModal( wxID_CANCEL ); }
	void OnApply( wxCommandEvent& ) { doApply(); }
	void OnRestoreDefaults( wxCommandEvent& );

	void saveData();
	void doApply();

	void OnTreebookChanging( class wxBookCtrlEvent& );
	void OnTreebookChanged( class wxBookCtrlEvent& );
	void OnConfletChanged( wxCommandEvent& );
	void OnToolbarConfigChanged( wxCommandEvent& );

	wxString configkey;
	wxTreebook* notebook;
	int activePanelIndex;

	/*
	* Tree containing the required conflet IDs
	*/
	std::list<ConfletIdTreenode> nodes;

	/*
	* Inventory of pages with subtree IDs (if relevant) and  pointers to wxPanel and Conflet instance
	*/
	std::vector<ConfigdialogPage> toc;

	wxButton *ok_button;
	wxButton *apply_button;
	wxButton *restore_button;
	wxButton *cancel_button;
};


#endif // CONFIGURATION_DIALOG_H
