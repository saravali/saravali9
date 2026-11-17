/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/NotebookWidget.cpp
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

#include "NotebookWidget.h"

#include "BasicWidget.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "gdiconfig.h"
#include "Lang.h"
#include "MultipleView.h"
#include "TextHelper.h"
#include "TextWidget.h"
#include "WidgetFactory.h"

#include <wx/choicebk.h>
#include <wx/listbook.h>
#include <wx/notebook.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>
#include <wx/sizer.h>

extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   NotebookWidget   ---   Constructor
**
******************************************************/
NotebookWidget::NotebookWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &category,
	const std::vector<WidgetCategory> &elements, const ToolbarWidgetId &toolbarId )
		: BasicWidget( parent, props, category, toolbarId ),
		elements( elements )
{
	//assert( elements.size() > 0 );
	debuglog << "NotebookWidget::NotebookWidget DataContext " << props->getDataContext();
	for( const auto &e : elements )
	{
		debuglog << "NotebookWidget::NotebookWidget element " << e;
	}

	if ( elements.size() > 1 )
	{
		int orient = wxNB_TOP;
		switch ( gdiconfig->multipleView.notebookOrientation )
		{
			case 1:
				orient = wxNB_BOTTOM;
			break;
			case 2:
				orient = wxNB_LEFT;
			break;
			case 3:
				orient = wxNB_RIGHT;
			break;
			default:
			break;
		}
		const long style = orient;

		//notebook = new wxNotebook( this, -1 );
		switch ( gdiconfig->multipleView.notebookStyle )
		{
			case 1:
				notebook = new wxListbook( this, -1, wxDefaultPosition, wxDefaultSize, style );
			break;
			case 2:
				notebook = new wxChoicebook( this, -1, wxDefaultPosition, wxDefaultSize, style );
			break;
			case 3:
				notebook = new wxToolbook( this, -1, wxDefaultPosition, wxDefaultSize, style );
			break;
			case 4:
				notebook = new wxTreebook( this, -1, wxDefaultPosition, wxDefaultSize, style );
			break;
			case 0:
				notebook = new wxNotebook( this, -1, wxDefaultPosition, wxDefaultSize, style );
			default:
			break;
		}
	}

	WidgetFactory factory( props );
	Lang lang;

	if ( elements.size() > 0 )
	{
		for( const auto &element : elements )
		{
			BasicWidget *page = nullptr;
			if ( notebook )
			{
				page = factory.createWidget( notebook, element );
				notebook->AddPage( page, lang.getWidgetCategoryName( element ));
			}
			else
			{
				assert( pages.size() == 0 );
				page = factory.createWidget( this, element );
			}
			pages.push_back( page );
			dirty.push_back( true );
		}
	}
	else
	{
		warnlog << "NotebookWidget has no elements";
		pages.push_back( factory.createWidget( notebook, WidgetCategory::EmptyWidget ));
		dirty.push_back( false );
	}

	assert( pages.size() > 0 );
	assert( pages.size() == elements.size() || elements.size() == 0 );
	assert( pages.size() == dirty.size() );

	if ( notebook )
	{
		Connect( wxID_ANY, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( NotebookWidget::OnNotebookEvent ));
		notebook->SetSelection( gdiconfig->getActiveNotebookTab( configkey ));
		mainsizer->Add( notebook, 1, wxEXPAND, 3 );
	}
	else
	{
		mainsizer->Add( pages.front(), 1, wxEXPAND, 3 );
	}
	SetSizer( mainsizer );

	Connect( wxEVT_IDLE, wxIdleEventHandler( NotebookWidget::OnIdle ));
}

/*****************************************************
**
**   NotebookWidget   ---   Destructor
**
******************************************************/
NotebookWidget::~NotebookWidget()
{
	if ( notebook )
	{
	  gdiconfig->setActiveNotebookTab( configkey, notebook->GetSelection());
	}
}

/*****************************************************
**
**   NotebookWidget   ---   postCreate
**
******************************************************/
void NotebookWidget::postCreate()
{
	debuglog << "NotebookWidget::postCreate";
	for( auto page : pages )
	{
		page->postCreate();
	}
}

/*****************************************************
**
**   NotebookWidget   ---   OnIdle
**
******************************************************/
void NotebookWidget::OnIdle( wxIdleEvent& )
{
	//debuglog << "NotebookWidget::OnIdle";
	const int sel = ( notebook ? notebook->GetSelection() : 0 );
	if ( sel >= 0 && sel < (int)pages.size())
	{
		assert( sel < (int)dirty.size());
		if ( dirty[sel] )
		{
			debuglog << "NotebookWidget::OnIdle calls updatePage on #" << sel;
			updatePage( sel );
		}
	}
	else
	{
		errorlog << "NotebookWidget has invalid selection " << sel << ", page size is " << pages.size();
	}
}

/*****************************************************
**
**   NotebookWidget   ---   OnDataChanged
**
******************************************************/
void NotebookWidget::OnDataChanged()
{
	debuglog << "NotebookWidget::OnDataChanged";
	setDirty();
}

/*****************************************************
**
**   NotebookWidget   ---   OnNotebookEvent
**
******************************************************/
void NotebookWidget::OnNotebookEvent( wxBookCtrlEvent &event )
{
	const int sel = event.GetSelection();
	debuglog << "NotebookWidget::OnNotebookEvent page " << sel;
	updatePage( sel );
}

/*****************************************************
**
**   NotebookWidget   ---   updatePage
**
******************************************************/
void NotebookWidget::updatePage( const int &sel )
{
	// the method can eventually by called after close event
	if ( ! props->isValid() ) return;

	debuglog << "NotebookWidget::updatePage sel " << sel;
	assert( sel >= 0 && sel < (int)pages.size());
	assert( sel >= 0 && sel < (int)dirty.size());
	if ( dirty[sel] )
	{
		BasicWidget *page = pages[sel];
		assert( page );
		debuglog << "NotebookWidget::updatePage call OnDataChanged on page #" << sel;
		page->OnDataChanged();
		dirty[sel] = false;
	}
	pages[sel]->Refresh();
}

/*****************************************************
**
**   NotebookWidget   ---   updatePage
**
******************************************************/
void NotebookWidget::setDirty( const bool b )
{
	dirty.assign( dirty.size(), b );
}

/*****************************************************
**
**   WidgetFactory   ---   createNotebookWidget
**
******************************************************/
BasicWidget *WidgetFactory::createNotebookWidget( wxWindow *parent, const WidgetCategory &category,
	const std::vector<WidgetCategory> &elements, const ToolbarWidgetId &toolbarId )
{
	return new NotebookWidget( parent, props, category, elements, toolbarId );
}


