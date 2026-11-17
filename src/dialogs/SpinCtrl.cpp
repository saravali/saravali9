/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/SpinCtrl.cpp
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

#include "SpinCtrl.h"

#include "astroconfig.h"
#include "Calculator.h"
#include "Commands.h"
#include "CommonControls.h"
#include "DateFormatter.h"
#include "datelocation.h"
#include "Formatter.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "TextValidator.h"

#include <wx/bmpbuttn.h>
#include <wx/choice.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/spinbutt.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

#define DEBUG_SPINCTRL

enum { BDT_TEXT = wxID_HIGHEST + 1100, BDT_SPIN, BDT_LABEL,
	DI_DATE, DI_BACKWARD3, DI_BACKWARD2, DI_BACKWARD1,
	DI_FORWARD1, DI_FORWARD2, DI_FORWARD3, DI_TIME, DI_NOW, DI_TIMEZONE };

/*****************************************************
**
**   BaseSpin   ---   Constructor
**
******************************************************/
BaseSpin::BaseSpin( wxWindow *parent, const int& id, double *value, const wxPoint &pos, const wxSize &size, 
	const double &minvalue, const double &maxvalue, const bool &showLabel )
	: wxControl( parent, id, pos, size, wxBORDER_NONE | wxTAB_TRAVERSAL ),
	value( value ),
	minvalue( minvalue ),
	maxvalue( maxvalue )
{
	constexpr int maxScrollPos = 10000000;
	constexpr int charWidth = 10;

	const int h = wxTextCtrl(this, wxID_ANY, "").GetSize().y;

	//text = new wxTextCtrl(this, BDT_TEXT, "", wxDefaultPosition, wxSize(charCount * charWidth, -1),
	text = new wxTextCtrl( this, BDT_TEXT, "", wxDefaultPosition, wxSize( charCount * charWidth, h ),
		wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB );

	spin = new wxSpinButton( this, BDT_SPIN );
	spin->SetRange( -maxScrollPos, maxScrollPos );

	wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
	//sizer->Add( text, 1, wxRIGHT|wxEXPAND, 3 );
	sizer->Add(text, 1, wxTOP | wxBOTTOM | wxLEFT | wxALIGN_CENTRE_VERTICAL, 3);
	sizer->Add( spin, 0, wxTOP | wxBOTTOM | wxRIGHT | wxALIGN_CENTRE_VERTICAL, 3 );

	if ( showLabel )
	{
		label = new wxStaticText( this, BDT_LABEL, wxT( "--" ), wxDefaultPosition, wxSize( 3 * charWidth, -1 ));
		sizer->Add( label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	}

	SetSizer( sizer );
	sizer->SetSizeHints(this);
	Layout();

	textBgColor = text->GetBackgroundColour();

	Connect( wxEVT_SPIN_UP, wxSpinEventHandler( BaseSpin::OnSpinUp ));
	Connect( wxEVT_SPIN_DOWN, wxSpinEventHandler( BaseSpin::OnSpinDown ));
	Connect( BDT_TEXT, wxEVT_KILL_FOCUS, wxFocusEventHandler( BaseSpin::OnKillFocus ));
	Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( BaseSpin::OnKillFocus ));

	// TODO warum braucht man das hier? wohl wegen Validator, der das Event schickt
	Connect( BDT_TEXT, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( BaseSpin::OnTextChangeEvent ));
}

/*****************************************************
**
**   BaseSpin   ---   OnKillFocus
**
******************************************************/
void BaseSpin::OnKillFocus( wxFocusEvent& )
{
#ifdef DEBUG_SPINCTRL
	debuglog << "BaseSpin::OnKillFocus";
#endif
}

/*****************************************************
**
**   BaseSpin   ---   OnTextChangeEvent
**
******************************************************/
void BaseSpin::OnTextChangeEvent( wxCommandEvent &event )
{
	event.Skip();

	// TODO wird das gebraucht
	const bool b = Validate();

#ifdef DEBUG_SPINCTRL
	debuglog << "BaseSpin::OnTextChangeEvent valid " << b;
#endif

	TransferDataToWindow();
	propagateChangeEvent();
}

/*****************************************************
**
**   BaseSpin   ---   propagateChangeEvent
**
******************************************************/
void BaseSpin::propagateChangeEvent()
{
	wxCommandEvent e( COMMAND_DIALOGCTRL_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   BaseSpin   ---   OnSpinUp
**
******************************************************/
void BaseSpin::OnSpinUp( wxSpinEvent& )
{
#ifdef DEBUG_SPINCTRL
	debuglog << "BaseSpin::OnSpinUp value " << *value << " minvalue " << minvalue << " maxvalue " << maxvalue;
#endif
	(*value)++;
	if ( *value > maxvalue )
	{
		*value = *value - maxvalue;

		wxCommandEvent event( COMMAND_SPIN_WRAP, GetId() );
		event.SetInt( 1 );
		wxPostEvent( GetParent(), event );
	}
	TransferDataToWindow();
	propagateChangeEvent();
}

/*****************************************************
**
**   BaseSpin   ---   OnSpinDown
**
******************************************************/
void BaseSpin::OnSpinDown( wxSpinEvent& )
{
#ifdef DEBUG_SPINCTRL
	debuglog << "BaseSpin::OnSpinDown value " << *value << " minvalue " << minvalue << " maxvalue " << maxvalue;
#endif
	(*value)--;
	if ( *value < minvalue )
	{
		*value = *value + maxvalue;

#ifdef DEBUG_SPINCTRL
	debuglog << "BaseSpin::OnSpinDown NEW value " << *value;
#endif

		wxCommandEvent event( COMMAND_SPIN_WRAP, GetId() );
		event.SetInt( -1 );
		wxPostEvent( GetParent(), event );
	}
	TransferDataToWindow();
	propagateChangeEvent();
}

/*****************************************************
**
*
**
******************************************************/
class CombinedDegreeSpinValidator : public BaseValidator
{
public:
	CombinedDegreeSpinValidator( double *value, double *degreeValue, int *choiceValue )
		: value( value ),
		degreeValue( degreeValue ),
		choiceValue( choiceValue )
	{
	}

	CombinedDegreeSpinValidator( const CombinedDegreeSpinValidator &a )
		: CombinedDegreeSpinValidator( a.value, a.degreeValue, a.choiceValue ) {}

	virtual wxObject *Clone() const override { return new CombinedDegreeSpinValidator( *this ); }

	virtual bool TransferFromWindow() override
	{
		*value = *degreeValue;
		if ( *choiceValue > 0 )
		{
			*value *= -1;
		}
#ifdef DEBUG_SPINCTRL
	debuglog << "CombinedDegreeSpinValidator::TransferFromWindow degreeValue " << *degreeValue
		<< " choiceValue " << choiceValue << " new value " << *value;
#endif
		return true;
	}

	virtual bool TransferToWindow() override
	{
		*degreeValue = fabs( *value );
		*choiceValue = ( *value < 0 ? 1 : 0 );
		return true;
	}

protected:

#if 0
	CombinedDegreeSpin *getControl()
	{
		wxWindow *w = GetWindow();
		assert( w );
		CombinedDegreeSpin *spin = wxDynamicCast( GetWindow(), CombinedDegreeSpin );
		assert( spin );
		return spin;
	}
#endif

	double *value;

	double *degreeValue;

	int *choiceValue;
};

/*****************************************************
**
**   CombinedDegreeSpin   ---   Constructor
**
******************************************************/
CombinedDegreeSpin::CombinedDegreeSpin( wxWindow *parent, const int &id, double *value, const DegreeType &degreeType,
		const wxPoint pos, const wxSize size )
	: wxControl( parent, id, pos, size, wxBORDER_NONE | wxTAB_TRAVERSAL ),
	value( value ),
	degreeType( degreeType )
{
	choice = new wxChoice( this, -1 );

	double maxvalue = 360;
	switch( degreeType )
	{
		case DegreeType::Longitude:
			maxvalue = 180;
			choice->Append( tr( "East" ));
			choice->Append( tr( "West" ));
		break;
		case DegreeType::Latitude:
			maxvalue = 90;
			choice->Append( tr( "North" ));
			choice->Append( tr( "South" ));
		break;

		// New Sealand has tz 13
		case DegreeType::Timezone:
			maxvalue = 13;
			choice->Append( tr( "East" ));
			choice->Append( tr( "West" ));
		break;
		default:
			assert( false );
		break;

	}
	degreeCtrl = new DegreeSpin( this, -1, &degreeValue, 0, maxvalue );

	wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
	sizer->Add( degreeCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 3 );
	sizer->Add( choice, 0, wxALL| wxALIGN_CENTER_VERTICAL, 3 );

	degreeCtrl->text->SetValidator( DegreeValidator( &degreeValue, 0, maxvalue ));

	choice->SetValidator( wxGenericValidator( &choiceValue ));
	SetValidator( CombinedDegreeSpinValidator( value, &degreeValue, &choiceValue ));

	SetSizer( sizer );
	sizer->SetSizeHints(this);
	Layout();

	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CombinedDegreeSpin::OnChoice ));
}

/*****************************************************
**
**   CombinedDegreeSpin   ---   setValue
**
******************************************************/
void CombinedDegreeSpin::setValue( const double &v )
{
	*value = v;
	TransferDataToWindow();
}

/*****************************************************
**
**   CombinedDegreeSpin   ---   OnChoice
**
******************************************************/
void CombinedDegreeSpin::OnChoice( wxCommandEvent& )
{
	choice->TransferDataFromWindow();
	propagateChangeEvent();
}

/*****************************************************
**
**   CombinedDegreeSpin   ---   propagateChangeEvent
**
******************************************************/
void CombinedDegreeSpin::propagateChangeEvent()
{
	wxCommandEvent e( COMMAND_DIALOGCTRL_CHANGED, GetId());
	wxPostEvent( GetParent(), e );
}

/*****************************************************
**
**   DegreeSpin   ---   Constructor
**
******************************************************/
DegreeSpin::DegreeSpin( wxWindow *parent, const int& id, double *value, const double minvalue, const double maxvalue,
	const wxPoint pos, const wxSize size )
	: BaseSpin( parent, id, value, pos, size, minvalue, maxvalue, false )
{
	text->SetValidator( DegreeValidator( value, minvalue, maxvalue ));
}

/*****************************************************
**
**   DateSpin   ---   Constructor
**
******************************************************/
DateSpin::DateSpin( wxWindow *parent, const int& id, double *value, const wxPoint pos, const wxSize size )
	: BaseSpin( parent, id, value, pos, size,
	CalculatorFactory().getCalculator()->getMinJD(),
	CalculatorFactory().getCalculator()->getMaxJD(),
	/* showLabel */ true )
{
	text->SetValidator( DateValidator( value ));
}

/*****************************************************
**
**   DateSpin   ---   changeValuePointer
**
******************************************************/
void DateSpin::changeValuePointer( double *v )
{
	value = v;
	text->SetValidator( DateValidator( value ));
}

/*****************************************************
**
**   DateSpin   ---   TransferDataToWindow
**
******************************************************/
bool DateSpin::TransferDataToWindow()
{
	if ( value )
	{
		Lang lang;
		extern Astroconfig *astroconfig;
		assert( label );

		const double tmpjd = *value + ( astroconfig->standardLocation.tz + astroconfig->standardLocation.dst ) / 24.0;
#ifdef DEBUG_SPINCTRL
		debuglog << "DateSpin::TransferDataToWindow " << tmpjd << " value " << *value;
#endif

		label->SetLabel( lang.getWeekdayName( ((int)(tmpjd + 1.5 )) % 7).Left( 2 ));
	}
	text->TransferDataToWindow();
	return true;
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   Constructor
**
******************************************************/
DateInputCombiCtrl::DateInputCombiCtrl( wxWindow *parent, const int id )
	: wxControl( parent, id )
{
	constexpr int dborder = 3;
	jd = MDate::getCurrentJD();

	wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );

	// 1. Date
	dateCtrl = new DateSpin( this, DI_DATE, &thedate );
	sizer->Add( dateCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, dborder );

	// 2. Navigation buttons
	wxBoxSizer* button_sizer = new wxBoxSizer( wxHORIZONTAL );
	button_sizer->Add( addButton( DI_BACKWARD3, BitmapId::Backward3, tr( "-10 years" )));
	button_sizer->Add( addButton( DI_BACKWARD2, BitmapId::Backward2, tr( "-1 year" )));
	button_sizer->Add( addButton( DI_BACKWARD1, BitmapId::Backward1, tr( "-1 month" )));
	button_sizer->Add( addButton( DI_FORWARD1, BitmapId::Forward1, tr( "+1 month" )));
	button_sizer->Add( addButton( DI_FORWARD2, BitmapId::Forward2, tr( "+1 year" )));
	button_sizer->Add( addButton( DI_FORWARD3, BitmapId::Forward3, tr( "+10 years" )));
	sizer->Add( button_sizer, 0, wxALL|wxALIGN_CENTER_VERTICAL, dborder );

	// 3. Time Input
	timeCtrl = new DegreeSpin( this, DI_TIME, &thetime, 0.0, 24.0 );
	sizer->Add( timeCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, dborder );

	// 4. Button now
	sizer->Add( addButton( DI_NOW, BitmapId::Now, tr( "Set current date" )), 0, wxALL|wxALIGN_CENTER_VERTICAL, dborder );
	/*
	wxBitmapButton *now_button = new wxBitmapButton( this, DI_NOW,
		ImageProvider::get()->getBitmap( BitmapId::Now, IconSize::Medium ));
	sizer->Add( now_button, 0, wxALL|wxALIGN_CENTRE_VERTICAL, dborder );
	*/

	// 5. Choice Timezone
	tzChoice = new TimezoneChoice( this,  DI_TIMEZONE );
	sizer->Add( tzChoice, 0, wxALL | wxALIGN_CENTRE_VERTICAL, dborder );

	SetSizer( sizer );
	sizer->Fit( this );
	Layout();

	jdToDateControls();

	Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DateInputCombiCtrl::OnControl ));

	Connect( DI_DATE, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( DateInputCombiCtrl::OnControl ));
	Connect( DI_TIME, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( DateInputCombiCtrl::OnControl ));
	Connect( DI_BACKWARD3, DI_FORWARD3, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DateInputCombiCtrl::OnDateNavigation ));
	Connect( DI_NOW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DateInputCombiCtrl::OnNow ));
	Connect( COMMAND_SPIN_WRAP, wxCommandEventHandler( DateInputCombiCtrl::OnWrap ));
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   setIsLocaltime
**
******************************************************/
void DateInputCombiCtrl::setIsLocaltime( const bool b )
{
	tzChoice->SetSelection( b ? 1 : 0 );
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   getIsLocaltime
**
******************************************************/
bool DateInputCombiCtrl::getIsLocaltime() const
{
	return tzChoice->GetSelection() == 1;
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   dateControlsToJD
**
******************************************************/
void DateInputCombiCtrl::dateControlsToJD()
{
	DateFormatter *dh = DateFormatter::get();

	const double tzoffset = getIsLocaltime() ? dh->getTotalConfiguredTzAndDstOffset( jd ) : 0.0;
	jd = thedate + ( thetime - tzoffset ) / 24.0;

#ifdef DEBUG_SPINCTRL
	debuglog << "DateInputCombiCtrl::dateControlsToJD tzoffset " << tzoffset << " time " << thetime
		<< " new jd " << dh->formatUTDatetime( jd );
#endif
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   jdToDateControls
**
******************************************************/
void DateInputCombiCtrl::jdToDateControls()
{
	DateFormatter *dh = DateFormatter::get();
	const double tzoffset = getIsLocaltime() ? dh->getTotalConfiguredTzAndDstOffset( jd ) : 0.0;
	const double localjd = jd + tzoffset / 24.0;

	thedate = floor( localjd + .5 ) - .5;
	thetime = a_red( ( localjd - floor( localjd )) + .5, 1 ) * 24.0;

#ifdef DEBUG_SPINCTRL
	debuglog << "DateInputCombiCtrl::jdToDateControls jd " << dh->formatUTDatetime( jd )
		<< " thedate " << dh->formatUTDatetime( thedate )
		<< " thetime " << dh->formatTime( thetime, false );
#endif

	TransferDataToWindow();
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   OnControl
**
******************************************************/
void DateInputCombiCtrl::OnControl( wxCommandEvent& )
{
#ifdef DEBUG_SPINCTRL
	debuglog << "DateInputCombiCtrl::OnControl";
#endif
	TransferDataFromWindow();
	dateControlsToJD();
	propagateChangeEvent();
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   OnDateNavigation
**
******************************************************/
void DateInputCombiCtrl::OnDateNavigation( wxCommandEvent &event )
{
#ifdef DEBUG_SPINCTRL
	debuglog << "DateInputCombiCtrl::OnDateNavigation id " << event.GetId();;
#endif
	extern Astroconfig *astroconfig;
	double delta = 0;

	switch( event.GetId())
	{
		case DI_BACKWARD3:
			delta = -10 * astroconfig->directions.yearLength.getValue();
		break;
		case DI_BACKWARD2:
			delta = -astroconfig->directions.yearLength.getValue();
		break;
		case DI_BACKWARD1:
				delta = -30.0;
		break;
		case DI_FORWARD1:
				delta = 30.0;
		break;
		case DI_FORWARD2:
			delta = astroconfig->directions.yearLength.getValue();
		break;
		case DI_FORWARD3:
			delta = 10 * astroconfig->directions.yearLength.getValue();
		break;
		default:
			assert( false );
		break;
	}
	jd += delta;
	jdToDateControls();
	propagateChangeEvent();
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   setCurrentDate
**
******************************************************/
void DateInputCombiCtrl::setCurrentDate()
{
#ifdef DEBUG_SPINCTRL
	debuglog << "DateInputCombiCtrl::setCurrentDate";
#endif
	jd = MDate::getCurrentJD();
	jdToDateControls();
	propagateChangeEvent();
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   OnWrap
**
******************************************************/
void DateInputCombiCtrl::OnWrap( wxCommandEvent &event )
{
	dateControlsToJD();
#ifdef DEBUG_SPINCTRL
	debuglog << "DateInputCombiCtrl::OnWrap value " << event.GetInt() << " jd " << jd;
#endif

	switch( event.GetInt() )
	{
		case 1:
			jd += 1;
		break;
		case -1:
			jd -= 1;
		break;
		default:
			errorlog << "DateInputCombiCtrl::OnWrap invalid wrap value " << event.GetInt();
		break;
	}
	jdToDateControls();
	propagateChangeEvent();
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   addButton
**
******************************************************/
wxButton *DateInputCombiCtrl::addButton( const int& cmdid, const BitmapId &bmpid, const wxString &hint )
{
	wxBitmapButton *button = new wxBitmapButton( this, cmdid,
		ImageProvider::get()->getBitmap( bmpid, IconSize::Small ));
	button->SetToolTip( hint );
	return button;
}

/*****************************************************
**
**   DateInputCombiCtrl   ---   propagateChangeEvent
**
******************************************************/
void DateInputCombiCtrl::propagateChangeEvent()
{
#ifdef DEBUG_SPINCTRL
	debuglog << "DateInputCombiCtrl::propagateChangeEvent";
#endif
	wxCommandEvent event( COMMAND_DIALOGCTRL_CHANGED );
	GetParent()->HandleWindowEvent(event);
}



