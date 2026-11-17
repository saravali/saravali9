/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/WesternAspectConflet.cpp
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

#include "WesternAspectConflet.h"
#include "enumserializer.h"
#include "GdiHelper.h"
#include "guibase.h"
#include "logger.h"
#include "Painter.h"
#include "PenDialog.h"
#include "MDocument.h"
#include "SymbolProvider.h"
#include "DocHelper.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/graphics.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

#define MAX_ASPECT_PANEL_ITEMS (int)WesternAspectId::Last + 1

enum { CD_CONFIG_ENABLED = wxID_HIGHEST + 1000,
	CD_CONFIG_ORBIS = wxID_HIGHEST + 1100,
	CD_CONFIG_PEN = wxID_HIGHEST + 1200,
	CD_CONFIG_PREVIEW = wxID_HIGHEST + 1300 };

/*************************************************//**
*
*   Paints a single aspect in preview
*
******************************************************/
class AspectPreviewWidget : public wxPanel
{
public:
	AspectPreviewWidget( wxWindow *parent, const WesternAspectId &aid, WesternAspectConfig &cfg )
		: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize( 50, -1 )),
		aid( aid ),
		cfg( cfg )
	{
		Connect( wxEVT_PAINT, wxPaintEventHandler( AspectPreviewWidget::OnPaint ));
	}

protected:

	/*****************************************************
	**
	**   AspectPreviewWidget   ---   OnPaint
	**
	******************************************************/
	void OnPaint( wxPaintEvent& )
	{
		wxCoord h, w;
		wxPaintDC dc( this );
		PrepareDC( dc );

		GetSize( &w, &h );
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
			painter->setDefaultPen();
			painter->setDefaultTextColor();
			painter->setGraphicFont();

			SingleWesternAspectConfig &acfg = cfg.getConfig( aid );
			AspectPaintHelper ph( painter, &cfg );

			double orb = acfg.orbis;

			ph.paintWesternAspect( MComplex( 0, .5 * h ),
				MComplex( w, .5 * h ),
				{ aid, 1 }, orb );
		}

		delete painter;
		delete gc;
	}

	const WesternAspectId aid;

	WesternAspectConfig &cfg;

};

/*************************************************//**
*
*  Panel for aspect configuration per aspect
*
******************************************************/
class AspectConfigPanel : public wxPanel
{
public:
	AspectConfigPanel( wxWindow *parent, const int &id, WesternAspectConfig &cfg )
		: wxPanel( parent, id ),
		cfg( cfg )
	{
		// params: cols, vgap, hgap
		wxFlexGridSizer *grid = new wxFlexGridSizer( 4, 3, 10 );
		grid->AddGrowableCol( 3 );

		// header
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Type" )));
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Orbis" )));
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Pen" )));
		grid->Add( new wxStaticText( this, wxID_ANY, _( "Preview" )));

		for( WesternAspectId aid : Enum<WesternAspectId>())
		{
			const int i = (int)aid;
			grid->AddGrowableRow( i );

			checkboxes[i] = new wxCheckBox( this, CD_CONFIG_ENABLED + i, enum_to_string( aid ));
			grid->Add( checkboxes[i], 0, wxEXPAND, 0 );

			wxBoxSizer* sizer_spin = new wxBoxSizer( wxHORIZONTAL );

			spins[i] = new wxSpinCtrl( this, CD_CONFIG_ORBIS + i );
			sizer_spin->Add( spins[i], 0, wxEXPAND, 0 );

			text_deg[i] = new wxStaticText( this, wxID_ANY, _( "deg" ));
			sizer_spin->Add( text_deg[i], 0, wxALIGN_CENTER_VERTICAL, 0 );
			grid->Add( sizer_spin, 0, wxEXPAND, 0 );

			buttons[i] = new wxButton( this, CD_CONFIG_PEN + i, _( "Pen ..." ) );
			grid->Add( buttons[i], 0, wxEXPAND, 0 );

			previews[i] = new AspectPreviewWidget( this, aid, cfg );
			grid->Add( previews[i], 1, wxEXPAND, 0 );
		}
		SetSizer( grid );
		grid->Fit( this );

		updateUi();

		Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AspectConfigPanel::OnCheckBox ));
		Connect( wxID_ANY, wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( AspectConfigPanel::OnSpin ));
		Connect( wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AspectConfigPanel::OnButtonPen ));
	}

	virtual bool TransferDataToWindow() override
	{
		for( WesternAspectId aid : Enum<WesternAspectId>())
		{
			SingleWesternAspectConfig &acfg = cfg.getConfig( aid );
			const int i = (int)aid;

			checkboxes[i]->SetValue( acfg.active );
			spins[i]->SetValue( (int)acfg.orbis );
		}
		return true;
	}

	void updateUi()
	{
		for( int i = 0; i < MAX_ASPECT_PANEL_ITEMS; i++ )
		{
			const bool b = checkboxes[i]->IsChecked() && cfg.showAspects;

			spins[i]->Enable( b );
			text_deg[i]->Enable( b );

			buttons[i]->Enable( b );
			previews[i]->Enable( b );
			previews[i]->Refresh();
		}
	}
private:

	void OnCheckBox( wxCommandEvent &event )
	{
		const int id = event.GetId() - CD_CONFIG_ENABLED;
		const WesternAspectId aid = (WesternAspectId)id;
		debuglog << "OnCheckBox id " << id << " aid" << aid;
		assert_valid_enum<WesternAspectId>( aid );
		SingleWesternAspectConfig &acfg = cfg.getConfig( aid );
		acfg.active = event.IsChecked();

		event.Skip();
		updateUi();
	}

	void OnSpin( wxCommandEvent &event )
	{
		const int id = event.GetId() - CD_CONFIG_ORBIS;
		const WesternAspectId aid = (WesternAspectId)id;
		debuglog << "OnSpin id " << id << " aid " << aid << " value " << event.GetInt();
		SingleWesternAspectConfig &acfg = cfg.typeConfigs[ aid ];

		acfg.orbis = event.GetInt();

		event.Skip();
		updateUi();
	}

	void OnButtonPen( wxCommandEvent &event )
	{
		const int id = event.GetId() - CD_CONFIG_PEN;
		const WesternAspectId aid = (WesternAspectId)id;
		assert_valid_enum<WesternAspectId>( aid );
		debuglog << "OnPen id " << id << " aid " << aid;
		SingleWesternAspectConfig &acfg = cfg.typeConfigs[ aid ];

		PenDialog dialog( this, acfg.pen, MBrush( gdiconfig->colors.bgColor ));
		if ( dialog.ShowModal() == wxID_OK )
		{
			acfg.pen  = dialog.getPen();
			updateUi();
			event.Skip();
		}
	}


	std::array<wxCheckBox*, MAX_ASPECT_PANEL_ITEMS> checkboxes = { nullptr };

	std::array<wxSpinCtrl*, MAX_ASPECT_PANEL_ITEMS> spins = { nullptr };

	// text "deg" after spin ctrl
	std::array<wxStaticText*, MAX_ASPECT_PANEL_ITEMS> text_deg = { nullptr };

	std::array<wxButton*, MAX_ASPECT_PANEL_ITEMS> buttons = { nullptr };

	std::array<AspectPreviewWidget*, MAX_ASPECT_PANEL_ITEMS> previews = { nullptr };

	WesternAspectConfig &cfg;

};

/*****************************************************
**
**   WesternAspectConflet   ---   Constructor
**
******************************************************/
WesternAspectConflet::WesternAspectConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->westernAspects )
{
	// begin wxGlade: WesternAspectConflet::WesternAspectConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(1, 2, 0, 0);
	wxFlexGridSizer* sizer_left = new wxFlexGridSizer(1, 1, 0, 0);
	sizer_main->Add(sizer_left, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_globalparams = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Global Parameters")), wxVERTICAL);
	sizer_left->Add(sizer_globalparams, 1, wxEXPAND, 0);
	check_show_aspects = new wxCheckBox(sizer_globalparams->GetStaticBox(), wxID_ANY, _("Show Aspects"));
	check_show_aspects->SetValue(1);
	sizer_globalparams->Add(check_show_aspects, 0, wxALL, 3);
	check_use_colors = new wxCheckBox(sizer_globalparams->GetStaticBox(), wxID_ANY, _("Use Colors"));
	sizer_globalparams->Add(check_use_colors, 0, wxALL, 3);
	check_show_type = new wxCheckBox(sizer_globalparams->GetStaticBox(), wxID_ANY, _("Show Aspect Type"));
	sizer_globalparams->Add(check_show_type, 0, wxALL, 3);
	check_show_orbis = new wxCheckBox(sizer_globalparams->GetStaticBox(), wxID_ANY, _("Show Orbis"));
	sizer_globalparams->Add(check_show_orbis, 0, wxALL, 3);
	wxBoxSizer* sizer_right = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_right, 1, wxALL|wxEXPAND, 3);
	panel = new AspectConfigPanel(this, wxID_ANY, cfg);
	sizer_right->Add(panel, 1, wxALL|wxEXPAND, 3);
	
	sizer_main->AddGrowableRow(0);
	sizer_main->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	check_show_aspects->SetValidator( wxGenericValidator( &cfg.showAspects ));
	check_use_colors->SetValidator( wxGenericValidator( &cfg.useColors ));
	check_show_type->SetValidator( wxGenericValidator( &cfg.showSymbols ));
	check_show_orbis->SetValidator( wxGenericValidator( &cfg.showOrbis ));
}

/*****************************************************
**
**   WesternAspectConflet   ---   updateUi
**
******************************************************/
void WesternAspectConflet::updateUi()
{
	const bool b = check_show_aspects->GetValue();
	debuglog << "WesternAspectConflet::updateUi show Aspects" << b;

	check_use_colors->Enable( b );
	check_show_type->Enable( b );
	check_show_orbis->Enable( b );
	panel->Enable( b );
	panel->updateUi();

}

