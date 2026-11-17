/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/Conflet.h
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

#ifndef CONFLET_H
#define CONFLET_H

#include "bootstrap.h"

#include <wx/panel.h>

class wxSpinEvent;

/*************************************************//**
*
* 
*
******************************************************/
class Conflet : public wxPanel
{
public:
	Conflet( wxWindow *parent );

	virtual void updateUi() {}

	virtual void config2model() = 0;

	virtual void model2config() = 0;

	virtual void restoreDefaults() = 0;

	virtual void onActivate() { updateUi(); }

	virtual void onPassivate() {}

	bool isDirty() { return dirty; }

	void setDirty( const bool b = true ) { dirty = b; }

protected:

	// TODO temp
	void OnTextUpdated( wxCommandEvent& );

	void OnDialogElementChanged( wxCommandEvent& );

	void OnCommand( wxCommandEvent& );

	void OnSpin( wxSpinEvent& );
	
	void guiHasChanged();

	void propagateEvent( const int &commandId );

	void propagateChangeEvent();

	bool dirty;
};

/*************************************************//**
*
* 
*
******************************************************/
template<class T> class ConfletIncarnation : public Conflet
{
public:
	ConfletIncarnation( wxWindow *parent, T &globalConfig )
		: Conflet( parent ),
		globalConfig( globalConfig )
	{
		config2model();
	}

	virtual void config2model() override { cfg = globalConfig; }

	virtual void model2config() override { globalConfig = cfg; }

	virtual void restoreDefaults() override { cfg = T(); }

protected:

	T cfg;

	T &globalConfig;
};


#endif // CONFLET_H

