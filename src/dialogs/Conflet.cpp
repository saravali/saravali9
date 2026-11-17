/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/Conflet.cpp
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

#include "Conflet.h"
#include "guibase.h"
#include "logger.h"

#include <wx/clrpicker.h>
#include <wx/spinctrl.h>

/*****************************************************
**
**   Conflet   ---   Constructor
**
******************************************************/
Conflet::Conflet( wxWindow *parent )
	: wxPanel( parent ),
	dirty( false )
{
	debuglog << "Constructor Conflet";

	// generic control handled by wxGenericValidator
	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_RADIOBOX, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_SLIDER, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( Conflet::OnSpin ));

	Connect( wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_COMMAND_COLOURPICKER_CHANGED, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_CHECKLISTBOX, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_LISTBOX, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Conflet::OnCommand ));
	Connect( wxID_ANY, COMMAND_REARRANGECTRL_CHANGED, wxCommandEventHandler( Conflet::OnCommand ));

	Connect( wxID_ANY, COMMAND_DIALOGCTRL_CHANGED, wxCommandEventHandler( Conflet::OnDialogElementChanged ));
}

/*****************************************************
**
**   Conflet   ---   OnDialogElementChanged
**
******************************************************/
void Conflet::OnDialogElementChanged( wxCommandEvent &event )
{
	debuglog << "Conflet::OnDialogElementChanged";
	guiHasChanged();
	event.Skip();
}

/*****************************************************
**
**   Conflet   ---   OnTextUpdated
**
******************************************************/
void Conflet::OnTextUpdated( wxCommandEvent &event )
{
	debuglog << "Conflet::OnTextUpdated";
	guiHasChanged();
	event.Skip();
}

/*****************************************************
**
**   Conflet   ---   OnCommand
**
******************************************************/
void Conflet::OnCommand( wxCommandEvent &event )
{
	debuglog << "Conflet::OnCommand";
	guiHasChanged();
	event.Skip();
}

/*****************************************************
**
**   Conflet   ---   OnSpin
**
******************************************************/
void Conflet::OnSpin( wxSpinEvent &event )
{
	debuglog << "Conflet::OnSpin";
	guiHasChanged();
	event.Skip();
}

/*****************************************************
**
**   Conflet   ---   guiHasChanged
**
******************************************************/
void Conflet::guiHasChanged()
{
	debuglog << "Conflet::guiHasChanged";
	setDirty();
	updateUi();
	propagateChangeEvent();
}

/*****************************************************
**
**   Conflet   ---   propagateChangeEvent
**
******************************************************/
void Conflet::propagateChangeEvent()
{
	propagateEvent( CONFLET_CHANGED );
	/*
	wxCommandEvent e( CONFLET_CHANGED, GetId() );
	wxPostEvent( GetParent(), e );
	*/
}

/*****************************************************
**
**   Conflet   ---   propagateEvent
**
******************************************************/
void Conflet::propagateEvent( const int &commandId )
{
	wxCommandEvent e( commandId, GetId() );
	//e.ResumePropagation( 10 );
	wxPostEvent( GetParent(), e );
}


