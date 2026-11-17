/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/MiscellaneousConflet.h
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

#ifndef MISCELLANEOUSCONFLET_H_
#define MISCELLANEOUSCONFLET_H_

#include "Conflet.h"
#include "gdiconfig.h"

class LogLevelChoice;
class wxCheckBox;
class wxChoice;
class wxSpinCtrl;
class wxStaticText;

// TEMP
class DateSpin;
class MDateSpin;
class MDegreeSpin;
class DegreeSpin;
class CombinedDegreeSpin;
class CombinedDegreeSpin1;

// begin wxGlade: ::dependencies
#include <wx/spinctrl.h>
// end wxGlade

// begin wxGlade: ::extracode
// end wxGlade

/*************************************************//**
*
* 
*
******************************************************/
class MiscellaneousConflet : public ConfletIncarnation<ViewConfig>
{
public:
	// begin wxGlade: MiscellaneousConflet::ids
	// end wxGlade

	MiscellaneousConflet( wxWindow* parent );

	virtual void updateUi() override;

	virtual void model2config() override;

private:

protected:
	// begin wxGlade: MiscellaneousConflet::attributes
	wxCheckBox* check_responsive_events;
	wxCheckBox* check_showstatusinfo;
	wxStaticText* label_file_backup;
	wxChoice* choice_file_backup;
	wxStaticText* label_recent_files;
	wxSpinCtrl* spin_recent_files;
	wxStaticText* label_frame_width;
	wxSpinCtrl* spin_frame_width;
	wxCheckBox* check_textwidget_native;
	wxCheckBox* check_validator_feedback;
	wxStaticText* label_graphic_size_dialog;
	wxChoice* choice_graphic_size_dialog;
	wxCheckBox* check_overwrite;
	wxStaticText* label_ui_loglevel;
	wxChoice* choice_ui_loglevel;
	wxCheckBox* check_notification;
	wxStaticText* label_notification_level;
	wxChoice* choice_notification_level;
	wxStaticText* label_notification_timeout;
	wxSpinCtrl* spin_notification_timeout;
	wxCheckBox* check_stdout_logging;
	wxStaticText* label_stdout_loglevel;
	wxChoice* choice_stdout_loglevel;
	DateSpin* spin_date;
	DegreeSpin* spin_time;
	CombinedDegreeSpin* degree_longitude;
	// end wxGlade
}; // wxGlade: end class


#endif // MISCELLANEOUSCONFLET_H_
