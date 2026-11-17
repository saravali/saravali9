/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/YogaEditorWidget.h
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

#ifndef YOGAEDITORWIDGET_H_
#define YOGAEDITORWIDGET_H_

#include "BasicWidget.h"
#include "ConfigListItems.h"

class Rule;
class YogaEditorPanel;
class YogaExpert;

/*************************************************//**
*
*  Editor tool for Yoga configurations
*
******************************************************/
class YogaEditorWidget: public BasicWidget
{
public:

	YogaEditorWidget( wxWindow*, ContextProps_ptr );

	~YogaEditorWidget();

	virtual wxString getTitle( const bool shortname = false ) override;

  wxString getFilename() { return filename; }

  bool isDirty();

	bool saveFile();

	virtual bool queryClose() override;


	// File menu and toolbar
	void OnNewFile( wxCommandEvent& );
	void OnOpenFile( wxCommandEvent& );
	void OnSaveFile( wxCommandEvent& ) { saveFile(); }
	void OnSaveFileAs( wxCommandEvent& ) { saveFileAs(); }

private:
	void OnCloseFile( wxCommandEvent& ) { closeFile(); }
	void OnCloseCommand( wxCommandEvent& );

	void OnClose( wxCloseEvent& );

	// command buttons in dialog
	void OnNewSource( wxCommandEvent& );
	void OnNewGroup( wxCommandEvent& );
	void OnChoice( wxCommandEvent& );

	void OnParseRule( wxCommandEvent& ) { parseRule(); }
	void OnEvaluateRule( wxCommandEvent& );
	void OnSelectObjects( wxCommandEvent& );
	void OnClearChart( wxCommandEvent& );

	void loadFile( wxString filename );
	bool closeFile();
	bool saveFileAs();
	void parseRule();
	void OnIdle( wxIdleEvent& );

	void updateUiItems();

	void echo( const wxString message, const int field = 0 );

	wxString filename;

	YogaEditorPanel *panel = nullptr;

	YogaExpert *expert = nullptr;

	std::vector<YogaConfig> yogas;
};

#endif // YOGAEDITORWIDGET_H_

