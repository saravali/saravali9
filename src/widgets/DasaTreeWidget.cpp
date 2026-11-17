/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/DasaTreeWidget.cpp
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

#include "astrobase.h"
#include "Commands.h"
#include "ConfigListItems.h"
#include "ContextProperties.h"
#include "Dasa.h"
#include "DasaTool.h"
#include "DasaTree.h"
#include "DateFormatter.h"
#include "gdiconfig.h"
#include "GdiWidgetWrapper.h"
#include "guibase.h"
#include "Horoscope.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "constants.h"
#include "TextHelper.h"
#include "TextWidget.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"

#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

extern Gdiconfig *gdiconfig;

enum { DASAWINDOW_CTRL = wxID_HIGHEST + 1, DW_SPLITTER, DW_TREE, DASAWINDOW_PANEL };

enum { PIC_DASA = 0, PIC_FOLDER, PIC_DASAROOT, PIC_DASATREE1, PIC_DASATREE2, PIC_DASATREE3,
	PIC_DASATREE4, PIC_DASATREE5, PIC_FOLDERE, PIC_DASAROOTE, PIC_DASATREE1E, PIC_DASATREE2E,
	PIC_DASATREE3E, PIC_DASATREE4E, PIC_DASATREE5E
};

/*************************************************//**
* 
*   bottom panel in Dasa tree view 
* 
******************************************************/
class DasaPanel : public wxPanel
{ 
public:
  
  DasaPanel(wxWindow* parent, const int id )
	 : wxPanel(parent, id )
	{
		button_entry_chart = new wxBitmapButton(this, (int)CommandId::NewEntryChart, wxNullBitmap);
		button_entry_chart->SetBitmapLabel( ImageProvider::get()->getBitmapBundle( BitmapId::New ));
		button_entry_chart->SetMinSize(wxSize( 32, 32 ));
		button_entry_chart->SetToolTip(_("Create Entry Chart"));

		text_header = new wxStaticText(this, -1, _("dummy"));
		{
			wxFont font = text_header->GetFont();
			font.SetWeight( wxFONTWEIGHT_BOLD );
			text_header->SetFont( font );
		}

		text_lord = new wxStaticText(this, -1, _("dummy"));
		text_startdate = new wxStaticText(this, -1, _("dummy"));
		text_enddate = new wxStaticText(this, -1, _("dummy"));
		text_duration = new wxStaticText(this, -1, _("dummy"));

		wxBoxSizer* sizer_main = new wxBoxSizer( wxHORIZONTAL );
		wxBoxSizer* sizer_text = new wxBoxSizer( wxVERTICAL );

		sizer_text->Add(text_header, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);

		wxFlexGridSizer* sizer = new wxFlexGridSizer(4, 2, 3, 3);
		sizer->Add( new  wxStaticText( this, wxID_ANY, _("Lord")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
		sizer->Add(text_lord, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);

		sizer->Add( new  wxStaticText( this, wxID_ANY, _("From")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
		sizer->Add(text_startdate, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);

		sizer->Add( new  wxStaticText( this, wxID_ANY, _("To")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
		sizer->Add(text_enddate, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);

		sizer->Add( new  wxStaticText( this, wxID_ANY, _("Duration")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 3);
		sizer->Add(text_duration, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 3);
		sizer->AddGrowableCol(1);

		sizer_text->Add(sizer, 0, wxALL|wxEXPAND, 3);
		sizer_main->Add(sizer_text, 1, wxALL, 3);
		sizer_main->Add(button_entry_chart, 0, wxALL|wxALIGN_RIGHT, 3);

		SetSizer(sizer_main);
		sizer_main->Fit(this);
		enableButton( true );
	}

  void clear()
	{
		text_lord->SetLabel( wxEmptyString );
		text_startdate->SetLabel( wxEmptyString );
		text_enddate->SetLabel( wxEmptyString);
		text_duration->SetLabel( wxEmptyString );
		enableButton( false );
	}

	void setHeader( wxString s ) { text_header->SetLabel( s ); }
	void setLord( wxString s ) { text_lord->SetLabel( s ); }
	void setStartDate( wxString s ) { text_startdate->SetLabel( s ); }
	void setEndDate( wxString s ) { text_enddate->SetLabel( s ); }
	void setDuration( wxString s ) { text_duration->SetLabel( s ); }
	void enableButton( const bool b ) { button_entry_chart->Enable( b ); }
  
private:

  wxBitmapButton* button_entry_chart;
  wxStaticText* text_header;
  wxStaticText* text_lord;
  wxStaticText* text_startdate;
  wxStaticText* text_enddate;
  wxStaticText* text_duration;

};

/*************************************************//**
*
* 
*
******************************************************/
class DasaTreeWidget : public BasicWidget
{
public:

	DasaTreeWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &cat )
		: BasicWidget( parent, props, cat ),
		splitmode( cat == WidgetCategory::DasaTreeComposite )
	{
		treewidget = new DasaTree( props );
		wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );

		if ( splitmode )
		{
			splitter = new wxSplitterWindow( this );
			treewrapper = new GdiWidgetWrapper( splitter, treewidget );
			twidget = new TextWidget( splitter, props, WidgetCategory::TextDasaTree );

			splitter->SetMinimumPaneSize( 150 );
			splitter->SetSashGravity( 0.5 );
			splitter->SplitVertically( treewrapper, twidget );
			sizer->Add( splitter, 1, wxEXPAND );
		}
		else
		{
			treewrapper = new GdiWidgetWrapper( this, treewidget );
			panel = new DasaPanel( this, -1 );

			wxBoxSizer *treesizer = new wxBoxSizer( wxVERTICAL );
			treesizer->Add( treewrapper, 1, wxEXPAND );
			treesizer->Add( panel, 0, wxEXPAND );
			sizer->Add( treesizer, 1, wxEXPAND );
		}
		SetSizer( sizer );

		updateSecondControl( nullptr );

		Connect( wxID_ANY, WIDGET_NOTIFY_PARENT, wxCommandEventHandler( DasaTreeWidget::OnWidgetNotify ));
		Connect( (int)CommandId::NewEntryChart, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( DasaTreeWidget::OnEntryChart ));
		Connect( (int)CommandId::NewEntryChart, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DasaTreeWidget::OnEntryChart ));
	}

	/*****************************************************
	**
	**   DasaTreeWidget   ---   Destructor
	**
	******************************************************/
	~DasaTreeWidget()
	{
		if ( splitter )
		{
			gdiconfig->prefs.splitterSashPos[ configkey ] = splitter->GetSashPosition();
		}
	}

	/*****************************************************
	**
	**   DasaTreeWidget   ---   postCreate
	**
	******************************************************/
	virtual void postCreate() override
	{
		debuglog << "DasaTreeWidget::postCreate";

		if ( splitter )
		{
			if ( const auto p = gdiconfig->prefs.splitterSashPos.find( configkey );
				p != gdiconfig->prefs.splitterSashPos.end())
			{
				if ( p->second )
				{
					debuglog << "DasaTreeWidget::postCreate set sash to " << p->second;
					splitter->SetSashPosition( p->second );
				}
			}
			else splitter->SetSashPosition( 250 );
		}
	}

	/*****************************************************
	**
	**   DasaTreeWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
		debuglog << "DasaTreeWidget::OnDataChanged";
		treewrapper->OnDataChanged();
		if ( twidget ) twidget->OnDataChanged();
	}

protected:

	/*****************************************************
	**
	**   DasaTreeWidget   ---   OnWidgetNotify
	**
	******************************************************/
	void OnWidgetNotify( wxCommandEvent& )
	{
		debuglog << "DasaTreeWidget::OnWidgetNotify";
		updateSecondControl( treewidget->getCurrentItem());
	}

	/*****************************************************
	**
	**   DasaTreeWidget   ---   updateSecondControl
	**
	******************************************************/
	void updateSecondControl( GraphicalDasa_ptr item )
	{
		if ( panel ) updateDetailPanel( item );
		if ( twidget ) updateText( item );
	}

	/*****************************************************
	**
	**   DasaTreeWidget   ---   updateDetailPanel
	**
	******************************************************/
	void updateDetailPanel( GraphicalDasa_ptr item )
	{
		assert( panel );
		panel->clear();

		DateFormatter *dh = DateFormatter::get();
		constexpr bool isLocaltime = true;

    Horoscope_ptr h0 = props->getRadixHoroscope();
		assert( h0 );

		if ( item )
		{
			panel->setHeader( treewidget->getFullTitle( item ));

			if ( item->getLevel() >= DasaLevel::Mahadasa )
			{
				DasaTool *tool = DasaTool::get();
				panel->setLord( tool->getDasaLordChain( item->dasa ));
				const double startjd = std::max( item->dasa->getStartJD(), h0->getJD());

				// Start and end of Dasa
				panel->setStartDate( dh->formatDatetime( startjd, isLocaltime ));
				panel->setEndDate( dh->formatDatetime( item->dasa->getEndJD(), isLocaltime ));

				panel->setDuration( getDurationYmd( item->dasa->getEndJD() - startjd ));
				panel->enableButton( true );
			}
		}
		else
		{
			panel->setHeader( _( "Dasa Details" ));
			panel->enableButton( false );
		}
	}

	/*****************************************************
	**
	**   DasaTreeWidget   ---   updateText
	**
	******************************************************/
	void updateText( GraphicalDasa_ptr item )
	{
		debuglog << "DasaTreeWidget::updateText";
		assert( twidget );
		DasaTool *tool = DasaTool::get();

		MDocument_ptr doc = twidget->getDoc();
		doc->clear();

		if ( item )
		{
			tool->writeDasaDocumentation( doc, props->getRadixHoroscope(), item->dasa, sourcefilter );
		}
		twidget->OnDataChanged();
	}

	/*****************************************************
	**
	**   DasaTreeWidget   ---   OnEntryChart
	**
	******************************************************/
	void OnEntryChart( wxCommandEvent& )
	{
		debuglog << "DasaTreeWidget::OnEntryChart";
		assert( treewidget );
		const double currentjd = treewidget->getSelectedJD();
    Horoscope_ptr h0 = props->getRadixHoroscope();
		assert( h0 );

		if ( currentjd == 0 )
		{
			doMessageBox( this, "Error: can't determine date for entry chart." );
		}
		else
		{
			Horoscope_ptr h1( new Horoscope );
			h1->dl.location = h0->dl.location;
			h1->setDate( currentjd );
			h1->dl.name = std::format( "{}: {}", h0->dl.name, tr( "Entry Chart" ));

			wxCommandEvent e( APPWINDOW_CREATEVIEW, GetId());
			e.SetEventObject( new HoroscopeWrapper( h1 ));
			wxPostEvent( GetParent(), e );
		}
	}

	const bool splitmode = false;

	DasaTree *treewidget = nullptr;

	GdiWidgetWrapper *treewrapper = nullptr;

	wxSplitterWindow *splitter = nullptr;

	DasaPanel *panel = nullptr;

	TextWidget *twidget = nullptr;

	wxString sourcefilter;
};

/*****************************************************
**
**   WidgetFactory   ---   createDasaTreeWidget
**
******************************************************/
BasicWidget *WidgetFactory::createDasaTreeWidget( wxWindow *parent, const WidgetCategory &cat )
{
	return new DasaTreeWidget( parent, props, cat );
}

