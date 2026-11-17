/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/UranianAspectConflet.cpp
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

#include "UranianAspectConflet.h"
#include "gdiconfig.h"
#include "enumserializer.h"
#include "Formulary.h"
#include "GdiHelper.h"
#include "logger.h"
#include "Painter.h"
#include "PenDialog.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dcclient.h>
#include <wx/graphics.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

constexpr TermId MAX_TERMID = TermId::Difference;
constexpr int MAX_ASPECT_TYPES = (int)MAX_TERMID + 1;

enum { CD_CONFIG_MAINPEN = wxID_HIGHEST + 1000,
	CD_CONFIG_CONNECTORPEN = wxID_HIGHEST + 1100,
	CD_CONFIG_PREVIEW = wxID_HIGHEST + 1200 };

/*************************************************//**
*
* 
*
******************************************************/
class UranianAspectPreview : public wxPanel
{
public:

	/*****************************************************
	**
	**   UranianAspectPreview   ---   Constructor
	**
	******************************************************/
	UranianAspectPreview( wxWindow* parent, const UranianAspectStyle &cfg, const TermId &termId )
		: wxPanel( parent, -1, wxDefaultPosition, wxSize( 100, 50 ), wxFULL_REPAINT_ON_RESIZE ),
		//: wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE ),
		cfg( cfg ),
		termId( termId )
	{
		Connect( wxEVT_PAINT, wxPaintEventHandler( UranianAspectPreview::OnPaint ));
	}


private:

	/*****************************************************
	**
	**   UranianAspectPreview   ---   OnPaint
	**
	******************************************************/
	void OnPaint( wxPaintEvent& )
	{
		wxCoord h, w;
		GetSize( &w, &h );

		wxPaintDC dc( this );
		PrepareDC( dc );
		wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
		assert( gc );
		Painter *painter = PainterFactory().createGcPainter( gc );
		assert( painter );

		// background
		painter->setPenColor( gdiconfig->colors.bgColor );
		painter->setBrush( gdiconfig->colors.bgColor );
		painter->drawRectangle( 0, 0, w, h );

		if ( cfg.showAspects && IsEnabled())
		{
			painter->setGraphicFont();

			MComplex proot, p1, p2;
			if ( termId == TermId::Single )
			{
				proot = MComplex( 0, .5 * h );
				p1 = MComplex( w, .5 * h );
			}
			else
			{
				proot = MComplex( .5 * w, h );
				p1 = MComplex( 0, .5 * h );
				p2 = MComplex( w, .5 * h );
			}
			debuglog << "UranianAspectPreview paint show Orbis " << cfg.showOrbis << " show Division " << cfg.showDivision;
			AspectPaintHelper( painter, &cfg ).paintUranianAspect(
				termId, 
				proot, p1, p2,
				4,
				.5 );

		}
		delete painter;
		delete gc;
	}

	const UranianAspectStyle &cfg;

	const TermId termId;
};

/*************************************************//**
*
* 
*
******************************************************/
class UranianAspectConfigPanel : public wxPanel
{
public:

	/*****************************************************
	**
	**   UranianAspectConfigPanel   ---   Constructor
	**
	******************************************************/
	UranianAspectConfigPanel( wxWindow *parent, UranianAspectStyle &cfg )
		: wxPanel( parent, wxID_ANY ),
		cfg( cfg )
	{
		Formulary *formulary = Formulary::get();

		wxFlexGridSizer *grid = new wxFlexGridSizer( 4, 3, 10 );
		grid->AddGrowableCol( 3 );
		//grid->SetFlexibleDirection( wxBOTH );

		// header
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Type" )));
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Main Pen" )));
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Connector Pen" )));
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Preview" )));

		for( const auto tid : Enum<TermId>( TermId::Single, MAX_TERMID ))
		{
			const int i = (int)tid;
			grid->AddGrowableRow( i );

			labels[i] = new wxStaticText( this, wxID_ANY, formulary->getTermName( tid ));
			grid->Add( labels[i], 0, wxALIGN_CENTER_VERTICAL, 0 );

			mainPenButtons[i] = new wxButton( this, CD_CONFIG_MAINPEN + i, _( "Pen ..." ) );
			grid->Add( mainPenButtons[i], 0, wxALIGN_CENTER_VERTICAL, 0 );

			if ( tid == TermId::Single ) // single term do not need connectors
			{
				grid->Add( 20, 20, 0, 0 );
			}
			else
			{
				connectorPenButtons[i] = new wxButton( this, CD_CONFIG_CONNECTORPEN + i, _( "Pen ..." ) );
				grid->Add( connectorPenButtons[i], 0, wxALIGN_CENTER_VERTICAL, 0 );
			}

			previews[i] = new UranianAspectPreview( this, cfg, tid );
			grid->Add( previews[i], 1, wxEXPAND, 0 );
		}


		SetSizer( grid );
		grid->Fit( this );

		updateUi();

		for( const auto tid : Enum<TermId>( TermId::Single, MAX_TERMID ))
		{
			Connect( CD_CONFIG_MAINPEN + (int)tid, wxEVT_COMMAND_BUTTON_CLICKED,
				wxCommandEventHandler( UranianAspectConfigPanel::OnButtonMainPen ));
			Connect( CD_CONFIG_CONNECTORPEN + (int)tid, wxEVT_COMMAND_BUTTON_CLICKED,
				wxCommandEventHandler( UranianAspectConfigPanel::OnButtonConnectorPen ));
		}
	}

	/*****************************************************
	**
	**   UranianAspectConfigPanel   ---   updateUi
	**
	******************************************************/
	void updateUi()
	{
		for( const auto tid : Enum<TermId>( TermId::Single, MAX_TERMID ))
		{
			assert_valid_enum<TermId>( tid );
			const int i = (int)tid;
			assert( i >= 0 && i <= (int)MAX_TERMID );

			const bool b = cfg.showAspects;
			labels[i]->Enable( b );
			mainPenButtons[i]->Enable( b );
			if ( connectorPenButtons[i] ) connectorPenButtons[i]->Enable( b );
			previews[i]->Refresh();
		}
	}

protected:

	/*****************************************************
	**
	**   UranianAspectConfigPanel   ---   OnButtonMainPen
	**
	******************************************************/
	void OnButtonMainPen( wxCommandEvent &event )
	{
		const int id = event.GetId() - CD_CONFIG_MAINPEN;
		const TermId tid = (TermId)id;
		assert_valid_enum<TermId>( tid );
		debuglog << "OnMainPen id " << id << " tid " << tid;
		UranianAspectPenStyle &style = cfg.penStyles[ tid ];

		PenDialog dialog( this, style.mainPen, MBrush( gdiconfig->colors.bgColor ));
		if ( dialog.ShowModal() == wxID_OK )
		{
			style.mainPen  = dialog.getPen();
			updateUi();
			event.Skip();
		}
	}

	/*****************************************************
	**
	**   UranianAspectConfigPanel   ---   OnButtonConnectorPen
	**
	******************************************************/
	void OnButtonConnectorPen( wxCommandEvent &event )
	{
		const int id = event.GetId() - CD_CONFIG_CONNECTORPEN;
		const TermId tid = (TermId)id;
		assert_valid_enum<TermId>( tid );
		debuglog << "OnConnectorPen id " << id << " tid " << tid;
		UranianAspectPenStyle &style = cfg.penStyles[ tid ];

		PenDialog dialog( this, style.connectorPen, MBrush( gdiconfig->colors.bgColor ));
		if ( dialog.ShowModal() == wxID_OK )
		{
			style.connectorPen  = dialog.getPen();
			updateUi();
			event.Skip();
		}
	}

	std::array<wxStaticText*, MAX_ASPECT_TYPES> labels = { nullptr };

	std::array<wxButton*, MAX_ASPECT_TYPES> mainPenButtons = { nullptr };

	std::array<wxButton*, MAX_ASPECT_TYPES> connectorPenButtons = { nullptr };

	std::array<UranianAspectPreview*, MAX_ASPECT_TYPES> previews = { nullptr };

	UranianAspectStyle &cfg;
};

/*****************************************************
**
**   UranianAspectConflet   ---   Constructor
**
******************************************************/
UranianAspectConflet::UranianAspectConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->uranianAspectStyle )
{
	// begin wxGlade: UranianAspectConflet::UranianAspectConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxStaticBoxSizer* sizer_general = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("General")), wxVERTICAL);
	sizer_main->Add(sizer_general, 0, wxALL|wxEXPAND, 0);
	check_show_aspects = new wxCheckBox(sizer_general->GetStaticBox(), wxID_ANY, _("Show Aspects"));
	sizer_general->Add(check_show_aspects, 0, wxALL, 3);
	check_use_colors = new wxCheckBox(sizer_general->GetStaticBox(), wxID_ANY, _("Use Colors"));
	sizer_general->Add(check_use_colors, 0, wxALL, 3);
	check_draw_connector = new wxCheckBox(sizer_general->GetStaticBox(), wxID_ANY, _("Draw Connector"));
	sizer_general->Add(check_draw_connector, 0, wxALL, 3);
	check_show_division = new wxCheckBox(sizer_general->GetStaticBox(), wxID_ANY, _("Show Division"));
	sizer_general->Add(check_show_division, 0, wxALL, 2);
	check_show_orbis = new wxCheckBox(sizer_general->GetStaticBox(), wxID_ANY, _("Show Orbis"));
	sizer_general->Add(check_show_orbis, 0, wxALL, 2);
	wxBoxSizer* sizer_preview = new wxBoxSizer(wxHORIZONTAL);
	sizer_main->Add(sizer_preview, 1, wxALL|wxEXPAND, 3);
	panel = new UranianAspectConfigPanel(this, cfg);
	panel->SetMinSize(wxSize(-1, 500));
	sizer_preview->Add(panel, 1, wxALL, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	check_show_aspects->SetValidator( wxGenericValidator( &cfg.showAspects ));
	check_use_colors->SetValidator( wxGenericValidator( &cfg.useColors ));
	check_draw_connector->SetValidator( wxGenericValidator( &cfg.drawConnector ));
	check_show_orbis->SetValidator( wxGenericValidator( &cfg.showOrbis ));
	check_show_division->SetValidator( wxGenericValidator( &cfg.showDivision ));
}

/*****************************************************
**
**   UranianAspectConflet   ---   updateUi
**
******************************************************/
void UranianAspectConflet::updateUi()
{
	TransferDataFromWindow();

	const bool b = check_show_aspects->GetValue();
	debuglog << "UranianAspectConflet::updateUi showAspects " << b;

	check_use_colors->Enable( b );
	check_draw_connector->Enable( b );
	check_show_orbis->Enable( b );
	check_show_division->Enable( b );
	panel->Enable( b );
	panel->Refresh();
}

