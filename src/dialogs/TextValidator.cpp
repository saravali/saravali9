/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/TextValidator.cpp
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

#include "TextValidator.h"

#include "DateFormatter.h"
#include "astroconfig.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Formatter.h"
#include "LanguageConfig.h"
#include "logger.h"

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/textctrl.h>
#include <iomanip>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

#define DEBUG_TEXTVALIDATOR

/*****************************************************
**
**   TextValidator   ---   Constructor
**
******************************************************/
TextValidator::TextValidator()
{
	wxValidator::SuppressBellOnError( ! gdiconfig->view.validatorFeedbackOnError );


	Connect( wxEVT_CHAR, wxKeyEventHandler( TextValidator::OnChar ));
	Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( TextValidator::OnSetFocus ));
	Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( TextValidator::OnKillFocus ));
	Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( TextValidator::OnTextUpdated ));
	//Connect( wxEVT_COMMAND_TEXT_ENTER, wxTextEventHandler( TextValidator::OnTextEnter ));
}

/*****************************************************
**
**   TextValidator   ---   OnChar
**
******************************************************/
void TextValidator::OnChar( wxKeyEvent& event )
{
	const int keyCode = event.GetUnicodeKey();
	event.Skip();

#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "TextValidator::OnChar keycode " << keyCode;
#endif

	// TODO kommt nicht an
	if ( keyCode == WXK_ESCAPE )
	{
#ifdef DEBUG_TEXTVALIDATOR
		debuglog << "TextValidator::OnChar ESCAPE pressed";
#endif
		getControl()->Close();
	}

	// TODO was soll das Command sein: zum nächsten control gehen
	//if ( keyCode == WXK_RETURN )
	if ( keyCode == WXK_RETURN || keyCode == WXK_TAB )
	{
#ifdef DEBUG_TEXTVALIDATOR
		debuglog << "TextValidator::OnChar RETURN pressed";
#endif
		doAccept();
		event.Skip( false );
	}

	if ( keyCode < WXK_SPACE || keyCode == WXK_DELETE ) return;

	const wxUniChar uc = static_cast<wxUniChar>( keyCode );

	if ( ! isAcceptable( uc ))
	{
		if ( ! wxValidator::IsSilent())
		{
			wxBell();
		}
		event.Skip( false );
	}
}

/*****************************************************
**
**   TextValidator   ---   isAcceptable
**
******************************************************/
bool TextValidator::isAcceptable( const wxUniChar &uc )
{
	// only allowedChars match if set has size>0, otherwise pass
	if ( allowedChars.size() > 0 )
	{
		for( auto c : allowedChars )
		{
#ifdef DEBUG_TEXTVALIDATOR
			debuglog << "TextValidator::isAcceptable check allowed char " << (int)c;
#endif
			if ( c == uc )
			{
#ifdef DEBUG_TEXTVALIDATOR
				debuglog << "TextValidator::isAcceptable MATCH key " << (int)uc;
#endif
				return true;
			}
		}
		// char not in allowed char set
		return false;
	}
	return true;
}

/*****************************************************
**
**   TextValidator   ---   OnTextEnter
**
******************************************************/
void TextValidator::OnTextEnter( wxCommandEvent& )
{
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "TextValidator::OnTextEnter";
#endif
}

/*****************************************************
**
**   TextValidator   ---   doAccept
**
******************************************************/
bool TextValidator::doAccept()
{
	wxTextCtrl *ctrl = getControl();
	assert( ctrl );

	const bool b = Validate( ctrl );
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "TextValidator::doAccept valid: "<< std::boolalpha << b;
#endif

	setErrorStatus( !b );	

	TransferFromWindow();
	if ( b ) format();
	propagateChangeEvent();
	return b;
}

/*****************************************************
**
**   TextValidator   ---   setErrorStatus
**
******************************************************/
void TextValidator::setErrorStatus( const bool b )
{
	wxTextCtrl *ctrl = getControl();
	assert( ctrl );


#ifdef __WXMAC__
	static wxColour textFgColor;
	if ( ! textFgColor.IsOk())
	{
		textFgColor = ctrl->GetForegroundColour();
	}
	ctrl->SetForegroundColour( b ? gdiconfig->colors.errorBgColor : *wxBLACK );
#else
	static wxColour textBgColor;
	if ( ! textBgColor.IsOk())
	{
		textBgColor = ctrl->GetBackgroundColour();
	}
	ctrl->SetBackgroundColour( b ? gdiconfig->colors.errorBgColor : textBgColor );
#endif
}

/*****************************************************
**
**   TextValidator   ---   OnSetFocus
**
******************************************************/
void TextValidator::OnSetFocus( wxFocusEvent &event )
{
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "TextValidator::OnSetFocus";
#endif

	setErrorStatus( false );
	event.Skip();
}

/*****************************************************
**
**   TextValidator   ---   OnKillFocus
**
******************************************************/
void TextValidator::OnKillFocus( wxFocusEvent &event )
{
#ifdef DEBUG_TEXTVALIDATOR
	doAccept();
#else
	const bool b = doAccept();
	debuglog << "TextValidator::OnKillFocus valid: "<< std::boolalpha << b;
#endif

	event.Skip();
}

/*****************************************************
**
**   TextValidator   ---   OnTextUpdated
**
** This method is called whenever text input is a valid character,
** i.e. the OnChar method is passed successfully.
** This does not mean that the input is valid.
**
******************************************************/
void TextValidator::OnTextUpdated( wxCommandEvent &event )
{
	const bool b = Validate( getControl() );

#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "TextValidator::OnTextUpdated text " << getText() << " valid: " << std::boolalpha << b;
#endif

	event.Skip();
}

/*****************************************************
**
**   TextValidator   ---   getControl
**
******************************************************/
wxTextCtrl *TextValidator::getControl()
{
	wxWindow *w = GetWindow();
	assert( w );
	wxTextCtrl *ctrl = dynamic_cast< wxTextCtrl*>( w );
	assert( ctrl );
	return ctrl;
}

/*****************************************************
**
**   TextValidator   ---   getText
**
******************************************************/
wxString TextValidator::getText()
{
	return getControl()->GetValue();
}

/*****************************************************
**
**   TextValidator   ---   allowDigits
**
******************************************************/
void TextValidator::allowDigits()
{
	for( char c = '0'; c <= '9'; c++ )
	{
		allowedChars.insert( c );
	}
}

/*****************************************************
**
**    StringValidator   ---   TransferFromWindow
**
******************************************************/
bool StringValidator::TransferFromWindow()
{
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "StringValidator::TransferFromWindow";
#endif
	*value = getControl()->GetValue();
	return true;
}

/*****************************************************
**
**    StringValidator   ---   TransferToWindow
**
******************************************************/
bool StringValidator::TransferToWindow()
{
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "StringValidator::TransferToWindow";
#endif
	getControl()->ChangeValue( *value );
	return true;
}

/*****************************************************
**
**    FilenameValidator   ---   Validate
**
******************************************************/
bool FilenameValidator::Validate( wxWindow* )
{
	const bool b = wxFileName::Exists( *value );
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "FilenameValidator::Validate " << std::boolalpha << b;
#endif
	return b;
}

/*****************************************************
**
**    DirectoryValidator   ---   Validate
**
******************************************************/
bool DirectoryValidator::Validate( wxWindow* )
{
	const bool b = wxDir::Exists( *value );
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DirectoryValidator::Validate " << std::boolalpha << b;
#endif
	return b;
}

/*****************************************************
**
**   DoubleValidator   ---   Constructor
**
******************************************************/
DoubleValidator::DoubleValidator( double *a, const double &minvalue, const double &maxvalue )
	: TextValidator(),
	value( a ),
	minvalue( minvalue ),
	maxvalue( maxvalue )
{
	allowDigits();
	allowedChars.insert( '.' );
	allowedChars.insert( ',' );
	if ( minvalue < 0 )
	{
		allowedChars.insert( '-' );
	}
}

/*****************************************************
**
**   DoubleValidator   ---   Validate
**
******************************************************/
bool DoubleValidator::Validate( wxWindow* )
{
	double dummy;
	wxString err;
	const bool b = Formatter::get()->parseDoubleString( getText(), dummy, minvalue, maxvalue, err );

#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DoubleValidator::Validate " << getText() << " valid: " << std::boolalpha << b;
#endif
	return b;
}

/*****************************************************
**
**   DoubleValidator   ---   TransferFromWindow
**
******************************************************/
bool DoubleValidator::TransferFromWindow()
{
	Formatter *formatter = Formatter::get();
	double a = 0;
	wxString err;
	const bool isvalid = formatter->parseDoubleString( getText(), a, minvalue, maxvalue, err );

#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DoubleValidator::TransferFromWindow  text " << getText()
		<< " isvalid " << std::boolalpha << isvalid
		<< " double value " << a;
#endif

	if ( isvalid )
	{
		*value = a;
	}
	else
	{
		warnlog << err;
	}
	return isvalid;
}

/*****************************************************
**
**   DoubleValidator   ---   TransferToWindow
**
******************************************************/
bool DoubleValidator::TransferToWindow()
{
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DoubleValidator::TransferToWindow value " << *value;
#endif

	format();
	return true;
}

/*****************************************************
**
**   DoubleValidator   ---   format
**
******************************************************/
void DoubleValidator::format()
{
	getControl()->ChangeValue( Formatter::get()->formatDoubleString( *value, DegPrecision::FlexibleCutSecond ));
}

/*****************************************************
**
**   DegreeValidator   ---   Constructor
**
******************************************************/
DegreeValidator::DegreeValidator( double *a, const double &minvalue, const double &maxvalue )
	: TextValidator(),
	value( a ),
	minvalue( minvalue ),
	maxvalue( maxvalue )
{
	allowDigits();
	allowedChars.insert( ':' );
	if ( minvalue < 0 )
	{
		allowedChars.insert( '-' );
	}
	allowedChars.insert( LanguageConfig::get()->getDoubleSeparationCharacter()[0]);
}

/*****************************************************
**
**   DegreeValidator   ---   Validate
**
******************************************************/
bool DegreeValidator::Validate( wxWindow* )
{
	double dummy;
	wxString err;
	const bool b = Formatter::get()->parseDegreeString( getText(), dummy, maxvalue, err );
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DegreeValidator::Validate valid: " << std::boolalpha << b << " err " << err;
#endif
	return b;
}

/*****************************************************
**
**   DegreeValidator   ---   TransferFromWindow
**
******************************************************/
bool DegreeValidator::TransferFromWindow()
{
	Formatter *formatter = Formatter::get();
	double a = 0;
	wxString err;
	const bool isvalid = formatter->parseDegreeString( getText(), a, maxvalue, err );

#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DegreeValidator::TransferFromWindow  text " << getText()
		<< " isvalid " << std::boolalpha << isvalid
		<< " double value " << a << " err " << err;
#endif

	if ( isvalid )
	{
		*value = a;
	}
	else
	{
		warnlog << err;
	}
	return isvalid;
}

/*****************************************************
**
**   DegreeValidator   ---   TransferToWindow
**
******************************************************/
bool DegreeValidator::TransferToWindow()
{
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DegreeValidator::TransferToWindow value " << *value;
#endif

	format();
	setErrorStatus( false );
	return true;
}

/*****************************************************
**
**   DegreeValidator   ---   format
**
******************************************************/
void DegreeValidator::format()
{
	Formatter *formatter = Formatter::get();
	getControl()->ChangeValue( formatter->formatDegrees( *value ));
}


/*****************************************************
**
**   DateValidator   ---   Constructor
**
******************************************************/
DateValidator::DateValidator( double *value )
	: TextValidator(),
	value( value )
{
	allowDigits();
	//allowedChars.insert( '.' );
	allowedChars.insert( '-' );
	//allowedChars.insert( '/' );
}

/*****************************************************
**
**   DateValidator   ---   Validate
**
******************************************************/
bool DateValidator::Validate( wxWindow* )
{
	double dummy = 0;
	wxString err;
	const bool b = DateFormatter::get()->parseDateString( getText(), dummy, astroconfig->general.dateFormat, err );
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DateValidator::Validate " << getText() << " valid: " << std::boolalpha << b;
#endif
	return b;
}

/*****************************************************
**
**   DateValidator   ---   format
**
******************************************************/
void DateValidator::format()
{
	DateFormatter *formatter = DateFormatter::get();
#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DateValidator::format value " << std::setprecision( 12 ) << std::setw( 12 ) << *value
		<< " forceISODateFormat " << astroconfig->general.dateFormat;
#endif
	getControl()->ChangeValue( formatter->formatUTDate( *value, astroconfig->general.dateFormat ));
}

/*****************************************************
**
**   DateValidator   ---   TransferFromWindow
**
******************************************************/
bool DateValidator::TransferFromWindow()
{
	DateFormatter *formatter = DateFormatter::get();
	double a = 0;
	wxString err;
	const bool isvalid = formatter->parseDateString( getText(), a, astroconfig->general.dateFormat, err );

	if ( ! isvalid )
	{
		if ( err.IsEmpty())
		{
			warnlog << std::format( "general error while parsing date \"{}\"", getText() );
		}
		else
		{
			warnlog << err;
		}
	}

#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DateValidator::TransferFromWindow  text " << getText()
		<< " isvalid " << std::boolalpha << isvalid
		<< " double value " << a
		<< " err " << err
		<< " date " << formatter->formatDate( *value, false );
#endif

	if ( isvalid )
	{
		*value = a;
		//propagateChangeEvent();
	}
	return true;
}

/*****************************************************
**
**   DateValidator   ---   TransferToWindow
**
******************************************************/
bool DateValidator::TransferToWindow()
{

#ifdef DEBUG_TEXTVALIDATOR
	debuglog << "DateValidator::TransferToWindow value " << *value
		<< " date " << DateFormatter::get()->formatDate( *value, true );
#endif

	format();
	setErrorStatus( false );
	return true;
}



