/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/ChartColorConflet.cpp
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

#include "ChartColorConflet.h"

#include "astroconfig.h"
#include "BrushDialog.h"
#include "CommonControls.h"
#include "gdiconfig.h"
#include "guibase.h"
#include "Lang.h"
#include "logger.h"
#include "mathbase.h"
#include "Painter.h"
#include "SymbolProvider.h"
#include "DocHelper.h"

#include <wx/button.h>
#include <wx/clrpicker.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum
{
	CD_PLANET_FGCOLOR = wxID_HIGHEST + 1,
	CD_SIGN_FGCOLOR,
	CD_SIGN_BRUSH,
	CD_HOUSE_FGCOLOR,
	CD_HOUSE_BRUSH
};

/*************************************************//**
*
* 
*
******************************************************/
class PlanetPreviewPanel : public StringPreviewPanel
{
public:
	PlanetPreviewPanel( wxWindow* parent ) : StringPreviewPanel( parent ) {}

	PlanetId planetId = PlanetId::Sun;

protected:

	virtual MString getStringContents() override
	{
		return MToken( planetId, Viewpoint::Western );
	}
};

/*************************************************//**
*
* 
*
******************************************************/
class SignPreviewPanel : public StringPreviewPanel
{
public:
	SignPreviewPanel( wxWindow* parent ) : StringPreviewPanel( parent ) {}

	int item = 0;

protected:

	virtual MString getStringContents() override
	{
		assert( item >= 0 && item < 4 );
		PlanetPropertyConfig cfg = astroconfig->getViewpointConfig( Viewpoint::Western ).planetProps;
		cfg.textFormat = TextFormat::Symbol;

		MString s;
		s << MToken( (Sign)item, cfg ) << " "
			<< MToken( (Sign)( item + 4 ), cfg ) << " "
			<< MToken( (Sign)( item + 8 ), cfg );

		return s;
	}
};

/*************************************************//**
*
* 
*
******************************************************/
class HousePreviewPanel : public StringPreviewPanel
{
public:
	HousePreviewPanel( wxWindow* parent ) : StringPreviewPanel( parent ) {}

	int item = 0;

protected:

	virtual MString getStringContents() override
	{
		assert( item >= 0 && item < 4 );
		const HouseNumberFormat houseNumberStyle = HouseNumberFormat::Roman;

		MString s;
		s << getHouseNumberFormatted( (House)(red12(item + 1)), houseNumberStyle )
			<< " " << getHouseNumberFormatted( (House)(red12(item + 5)), houseNumberStyle )
			<< " " << getHouseNumberFormatted( (House)(red12(item + 9)), houseNumberStyle );
		return s;
	}
};

/*****************************************************
**
**   ChartColorConflet   ---   Constructor
**
******************************************************/
ChartColorConflet::ChartColorConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->chartColors )
{
	// begin wxGlade: ChartColorConflet::ChartColorConflet
	wxFlexGridSizer* grid_main = new wxFlexGridSizer(3, 1, 0, 0);
	wxStaticBoxSizer* sizer_planets = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Planets")), wxVERTICAL);
	grid_main->Add(sizer_planets, 0, wxEXPAND, 0);
	const wxString choice_planets_choices[] = {
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
	};
	choice_planets = new wxChoice(sizer_planets->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 5, choice_planets_choices);
	choice_planets->SetSelection(0);
	sizer_planets->Add(choice_planets, 0, wxALL, 3);
	wxGridSizer* grid_signs_copy = new wxGridSizer(2, 2, 0, 0);
	sizer_planets->Add(grid_signs_copy, 1, wxEXPAND, 0);
	label_fg_planets = new wxStaticText(sizer_planets->GetStaticBox(), wxID_ANY, _("Text Color"));
	grid_signs_copy->Add(label_fg_planets, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	color_fgplanets = new wxColourPickerCtrl(sizer_planets->GetStaticBox(), CD_PLANET_FGCOLOR);
	grid_signs_copy->Add(color_fgplanets, 1, wxALL|wxEXPAND, 3);
	label_preview_planets = new wxStaticText(sizer_planets->GetStaticBox(), wxID_ANY, _("Preview"));
	grid_signs_copy->Add(label_preview_planets, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	preview_planetcolor = new PlanetPreviewPanel(sizer_planets->GetStaticBox());
	grid_signs_copy->Add(preview_planetcolor, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_signs = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Signs")), wxVERTICAL);
	grid_main->Add(sizer_signs, 1, wxEXPAND, 0);
	const wxString choice_signs_choices[] = {
		_("Fire"),
		_("Earth"),
		_("Air"),
		_("Water"),
	};
	choice_signs = new wxChoice(sizer_signs->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_signs_choices);
	choice_signs->SetSelection(0);
	sizer_signs->Add(choice_signs, 0, wxALL, 3);
	wxGridSizer* grid_signs = new wxGridSizer(3, 2, 0, 0);
	sizer_signs->Add(grid_signs, 1, wxEXPAND, 0);
	label_fg_signs = new wxStaticText(sizer_signs->GetStaticBox(), wxID_ANY, _("Text Color"));
	grid_signs->Add(label_fg_signs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	color_fgsigns = new wxColourPickerCtrl(sizer_signs->GetStaticBox(), CD_SIGN_FGCOLOR);
	grid_signs->Add(color_fgsigns, 1, wxALL|wxEXPAND, 3);
	label_bg_sign = new wxStaticText(sizer_signs->GetStaticBox(), wxID_ANY, _("Brush"));
	grid_signs->Add(label_bg_sign, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_signbrush = new wxButton(sizer_signs->GetStaticBox(), CD_SIGN_BRUSH, _("Change Brush ..."));
	grid_signs->Add(button_signbrush, 0, wxALL, 3);
	label_preview_sign = new wxStaticText(sizer_signs->GetStaticBox(), wxID_ANY, _("Preview"));
	grid_signs->Add(label_preview_sign, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	preview_sign = new SignPreviewPanel(sizer_signs->GetStaticBox());
	grid_signs->Add(preview_sign, 1, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_houses = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Houses")), wxVERTICAL);
	grid_main->Add(sizer_houses, 1, wxEXPAND, 0);
	const wxString choice_houses_choices[] = {
		_("Fire"),
		_("Earth"),
		_("Air"),
		_("Water"),
	};
	choice_houses = new wxChoice(sizer_houses->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, 4, choice_houses_choices);
	choice_houses->SetSelection(0);
	sizer_houses->Add(choice_houses, 0, wxALL, 3);
	wxGridSizer* grid_houses = new wxGridSizer(3, 2, 0, 0);
	sizer_houses->Add(grid_houses, 1, wxEXPAND, 0);
	label_fg_houses = new wxStaticText(sizer_houses->GetStaticBox(), wxID_ANY, _("Text Color"));
	grid_houses->Add(label_fg_houses, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	color_fghouses = new wxColourPickerCtrl(sizer_houses->GetStaticBox(), CD_HOUSE_FGCOLOR);
	grid_houses->Add(color_fghouses, 1, wxALL|wxEXPAND, 3);
	label_bg_houses = new wxStaticText(sizer_houses->GetStaticBox(), wxID_ANY, _("Brush"));
	grid_houses->Add(label_bg_houses, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	button_housebrush = new wxButton(sizer_houses->GetStaticBox(), CD_HOUSE_BRUSH, _("Change Brush ..."));
	grid_houses->Add(button_housebrush, 0, wxALL, 3);
	label_preview_houses = new wxStaticText(sizer_houses->GetStaticBox(), wxID_ANY, _("Preview"));
	grid_houses->Add(label_preview_houses, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	preview_house = new HousePreviewPanel(sizer_houses->GetStaticBox());
	grid_houses->Add(preview_house, 1, wxALL|wxEXPAND, 3);
	
	SetSizer(grid_main);
	grid_main->Fit(this);
	// end wxGlade

	choice_planets->Clear();
	Lang lang;
	constexpr Viewpoint viewpoint = Viewpoint::Western;

	for ( const PlanetId i : Enum<PlanetId>( PlanetId::Sun, PlanetId::LastEphem ))
	{
		if ( PlanetHelper::isNodeAlias( i )) continue;
		choice_planets->Append( lang.getPlanetName( i, viewpoint ));
	}
	choice_planets->SetSelection( 0 );

	// special behaviour of choices because conflet is not dirty on select
	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( ChartColorConflet::OnChoice ));

	// handlers for object, sign and house colors
	Connect( CD_PLANET_FGCOLOR, wxEVT_COMMAND_COLOURPICKER_CHANGED,
		wxColourPickerEventHandler( ChartColorConflet::OnObjectColorChanged ));

	Connect( CD_SIGN_FGCOLOR, wxEVT_COMMAND_COLOURPICKER_CHANGED,
		wxColourPickerEventHandler( ChartColorConflet::OnObjectColorChanged ));
	Connect( CD_SIGN_BRUSH, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChartColorConflet::OnButtonSignBrush ));

	Connect( CD_HOUSE_FGCOLOR, wxEVT_COMMAND_COLOURPICKER_CHANGED,
		wxColourPickerEventHandler( ChartColorConflet::OnObjectColorChanged ));
	Connect( CD_HOUSE_BRUSH, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChartColorConflet::OnButtonHouseBrush ));
}

/*****************************************************
**
**   ChartColorConflet   ---   OnChoice
**
******************************************************/
void ChartColorConflet::OnChoice( wxCommandEvent& )
{
	debuglog << "ChartColorConflet::OnCommand";
	updateUi();
}

/*****************************************************
**
**   ChartColorConflet   ---   updateUi
**
******************************************************/
void ChartColorConflet::updateUi()
{
	debuglog << "ChartColorConflet::updateUi";

	// planets
	const int selplanet = planetChoiceToIndex( choice_planets->GetSelection());
	assert( selplanet >= 0 && selplanet < MAX_EPHEM_FACTORS );
	color_fgplanets->SetColour( cfg.planetFgColors[selplanet] );
	
	preview_planetcolor->planetId = (PlanetId)selplanet;
	preview_planetcolor->textColor = cfg.planetFgColors[selplanet];
	preview_planetcolor->Refresh();

	// signs
	const int selsign = choice_signs->GetSelection();
	assert( selsign >= 0 && selsign < 4 );
	color_fgsigns->SetColour( cfg.signFgColors[selsign] );

	preview_sign->item = selsign;
	preview_sign->textColor = cfg.signFgColors[selsign];
	preview_sign->brush = cfg.signPalette.brushes[selsign];
	preview_sign->Refresh();

	// houses
	const int selhouse = choice_houses->GetSelection();
	assert( selhouse >= 0 && selhouse < 4 );
	color_fghouses->SetColour( cfg.houseFgColors[selhouse] );

	preview_house->item = selhouse;
	preview_house->textColor = cfg.houseFgColors[selhouse];
	preview_house->brush = cfg.housePalette.brushes[selhouse];
	preview_house->Refresh();
}

/*****************************************************
**
**   ChartColorConflet   ---   OnObjectColorChanged
***
******************************************************/
void ChartColorConflet::OnObjectColorChanged( wxColourPickerEvent& )
{
	const int selplanet = choice_planets->GetSelection();
	assert( selplanet >= 0 && selplanet < MAX_EPHEM_FACTORS - 2 );
	const int pindex = planetChoiceToIndex( selplanet );
	assert( cfg.planetFgColors.size() == MAX_EPHEM_FACTORS );

	const wxColour color1 = color_fgplanets->GetColour();
	cfg.planetFgColors[pindex] = color1;

	const int selsign = choice_signs->GetSelection();
	debuglog << "ChartColorConflet::OnObjectColorChanged cfg.signFgColors.size() " << cfg.signFgColors.size()
		<< " sel " << selsign;;
	assert( selsign >= 0 && selsign < 4 );
	assert( cfg.signFgColors.size() == 4 );
	cfg.signFgColors[selsign] = color_fgsigns->GetColour();

	const int selhouse = choice_houses->GetSelection();
	assert( selhouse >= 0 && selhouse < 4 );
	assert( cfg.houseFgColors.size() == 4 );

	cfg.houseFgColors[selhouse] = color_fghouses->GetColour();

	guiHasChanged();
}

/*****************************************************
**
**   ChartColorConflet   ---   OnButtonSignBrush
**
******************************************************/
void ChartColorConflet::OnButtonSignBrush( wxCommandEvent& )
{
	const int selsign = choice_signs->GetSelection();
	assert( selsign >= 0 && selsign < 4 );
	BrushDialog dialog( this, cfg.signPalette.brushes[selsign] );
	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg.signPalette.brushes[selsign] = dialog.getBrush();
		guiHasChanged();
	}
}

/*****************************************************
**
**   ChartColorConflet   ---   OnButtonHouseBrush
**
******************************************************/
void ChartColorConflet::OnButtonHouseBrush( wxCommandEvent& )
{
	const int selhouse = choice_houses->GetSelection();
	assert( selhouse >= 0 && selhouse < 4 );
	BrushDialog dialog( this, cfg.housePalette.brushes[selhouse] );
	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg.housePalette.brushes[selhouse] = dialog.getBrush();
		guiHasChanged();
	}
}

/*****************************************************
**
**   ChartColorConflet   ---   planetChoiceToIndex
***
******************************************************/
int ChartColorConflet::planetChoiceToIndex( const int &selection )
{
	// mean nodes are not included and so index must be increased
	//if ( selection >= (int)PlanetId::MeanAscNode ) return selection + 2;
	if ( selection >= (int)PlanetId::DescNode ) return selection + 4;
	return selection;
}


