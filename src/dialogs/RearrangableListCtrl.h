/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/RearrangableListCtrl.h
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

#ifndef _REARRANGEABLE_LISTCTRL_H_
#define _REARRANGEABLE_LISTCTRL_H_

#include "constants.h"
#include "enumserializer.h"
#include "logger.h"

#include <wx/panel.h>
#include <wx/validate.h>

#include <vector>

class wxCheckListBox;
class wxBitmapButton;

//#define DEBUG_REARRANGECTRL_H

/*****************************************************
**
**   RearrangableListCtrlBase
**
******************************************************/
class RearrangableListCtrlBase : public wxPanel
{
public:
	RearrangableListCtrlBase( wxWindow* , const int &id );

protected:

	virtual void checkItem( const uint&, const bool &isChecked ) = 0;

	virtual void swapItems( const uint&, const uint& ) = 0;

	void appendItem( const wxString&, const bool& );

	void OnItemChecked( wxCommandEvent& );

	void OnListbox( wxCommandEvent& );

	void OnUp( wxCommandEvent& );

	void OnDown( wxCommandEvent& );

	void clearList();

	void updateUi();

	void emitChangeEvent();

	wxCheckListBox *checklist = nullptr;

	wxBitmapButton *button_up = nullptr;

	wxBitmapButton *button_down = nullptr;

};

/*****************************************************
**
**   RearrangableListCtrl
**
******************************************************/
template<class E> class RearrangableListCtrl : public RearrangableListCtrlBase
{
public:
	RearrangableListCtrl( wxWindow *parent , const int &id, std::vector<std::pair<E,bool>> &items )
		: RearrangableListCtrlBase( parent, id ),
		items( items )
	{
		TransferDataToWindow();
	}

protected:

	virtual bool TransferDataToWindow() override
	{
#ifdef DEBUG_REARRANGECTRL_H
		debuglog << "RearrangableListCtrl::TransferDataToWindow";
#endif

		assert( checklist );
		clearList();

		for( const auto &a : items )
		{
			appendItem( enum_to_string( a.first ), a.second );
		}
		return true;
	}

	virtual void checkItem( const uint &i, const bool &isChecked ) override
	{
		assert( i < items.size());
		items[i].second = isChecked;

#ifdef DEBUG_REARRANGECTRL_H
		debuglog << "RearrangableListCtrl::checkItem";
		for( uint j = 0; j < items.size(); j++ )
		{
			debuglog << "item " << j << " checked: " << items[j].second;
		}
#endif
	}

	virtual void swapItems( const uint &i1, const uint &i2 ) override
	{
		assert( i1 < items.size());
		assert( i2 < items.size());

		const auto a = items[ i1 ];
		items[ i1 ] = items[ i2 ];
		items[ i2 ] = a;

#ifdef DEBUG_REARRANGECTRL_H
		debuglog << "RearrangableListCtrl::swapItems " << i1 << "/" << i2;
		for( uint j = 0; j < items.size(); j++ )
		{
			debuglog << "after swap " << j << ": " << items[j].first << "/" << items[j].second;
		}
#endif
	}

	std::vector<std::pair<E,bool>> &items;

};

#endif // _REARRANGEABLE_LISTCTRL_H_

