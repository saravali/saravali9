/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/TextValidator.h
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

#ifndef _TEXTVALIDATOR_H_
#define _TEXTVALIDATOR_H_

#include "Validator.h"

#include <set>

class wxTextCtrl;

/*************************************************//**
*
* 
*
******************************************************/
class TextValidator : public BaseValidator
{
public:
	TextValidator();

protected:

	wxTextCtrl *getControl();

	wxString getText();

	/**
	* This method checks input and prevents invalid characters (i.e. chars not included in allowed chars).
	* On success OnTextUpdated will be called from event handler.
	*/
	void OnChar( wxKeyEvent& );

	void OnSetFocus( wxFocusEvent& );

	void OnKillFocus( wxFocusEvent& );

	virtual void format() {}

	void OnTextEnter( wxCommandEvent& );

	bool doAccept();

	virtual bool isAcceptable( const wxUniChar& );

	void setErrorStatus( const bool = true );

	/**
	* This method is called whenever text input is a valid character,
	* i.e. the OnChar method is passed successfully.
	* This does not mean that the input is valid.
	*/
	void OnTextUpdated( wxCommandEvent& );

	void allowDigits();

	std::set<wxUniChar> allowedChars;

};

/*************************************************//**
*
* 
*
******************************************************/
class StringValidator : public TextValidator
{
public:
	StringValidator( wxString *s ) : TextValidator(), value( s ) {}

	StringValidator( const StringValidator &a ) : StringValidator( a.value ) {}

	virtual wxObject *Clone() const override { return new StringValidator( *this ); }

	virtual bool TransferFromWindow() override;

	virtual bool TransferToWindow() override;

	virtual bool Validate( wxWindow* ) override { return true; }

protected:

	wxString *value;

};

/*************************************************//**
*
* 
*
******************************************************/
class FilenameValidator : public StringValidator
{
public:
	FilenameValidator( wxString *s ) : StringValidator( s ) {}

	FilenameValidator( const FilenameValidator &a ) : FilenameValidator( a.value ) {}

	virtual wxObject *Clone() const override { return new FilenameValidator( *this ); }

	virtual bool Validate( wxWindow* ) override;

protected:
};

/*************************************************//**
*
* 
*
******************************************************/
class DirectoryValidator : public StringValidator
{
public:
	DirectoryValidator( wxString *s ) : StringValidator( s ) {}

	DirectoryValidator( const DirectoryValidator &a ) : DirectoryValidator( a.value ) {}

	virtual wxObject *Clone() const override { return new DirectoryValidator( *this ); }

	virtual bool Validate( wxWindow* ) override;

protected:
};

/*************************************************//**
*
*  Validator for a text input field containing a floating point (double) item.
*  
*  Event COMMAND_DIALOGCTRL_CHANGED is send on update.
*
******************************************************/
class DoubleValidator : public TextValidator
{
public:
	DoubleValidator( double*, const double &minvalue, const double &maxvalue );

	DoubleValidator( const DoubleValidator &a ) : DoubleValidator( a.value, a.minvalue, a.maxvalue ) {}

	virtual wxObject *Clone() const override { return new DoubleValidator( *this ); }

	virtual bool TransferFromWindow() override;

	virtual bool TransferToWindow() override;

protected:

	virtual void format() override;

	virtual bool Validate( wxWindow* ) override;

	double *value;

	const double minvalue = 0.0;

	const double maxvalue = 0.0;
};

/*************************************************//**
*
*  Validator for a text input field containing a floating point item in degree (DD:MM:SS) format.
*  
*  Event COMMAND_DIALOGCTRL_CHANGED is send on update.
*
******************************************************/
class DegreeValidator : public TextValidator
{
public:
	DegreeValidator( double*, const double &minvalue, const double &maxvalue );

	DegreeValidator( const DegreeValidator &a ) : DegreeValidator( a.value, a.minvalue, a.maxvalue ) {}

	virtual wxObject *Clone() const override { return new DegreeValidator( *this ); }

	virtual bool Validate( wxWindow* ) override;

	virtual bool TransferFromWindow() override;

	virtual bool TransferToWindow() override;

protected:

	virtual void format() override;

	double *value;

	const double minvalue = 0.0;

	const double maxvalue = 0.0;
};

/*************************************************//**
*
*  Validator for a text input field containing a Date represented by JD in (YYYY:MM:DD) format.
*  
*  Event COMMAND_DIALOGCTRL_CHANGED is send on update.
*
******************************************************/
class DateValidator : public TextValidator
{
public:
	DateValidator( double* );

	DateValidator( const DateValidator &a ) : DateValidator( a.value ) {}

	virtual wxObject *Clone() const override { return new DateValidator( *this ); }

	virtual bool Validate( wxWindow* ) override;

	virtual bool TransferFromWindow() override;

	virtual bool TransferToWindow() override;

protected:

	virtual void format() override;

	double *value;
};


#endif // _TEXTVALIDATOR_H_

