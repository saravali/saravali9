/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/FontConflet.cpp
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

#include "FontConflet.h"

#include "CommonControls.h"
#include "guibase.h"
#include "logger.h"

#include <wx/choice.h>
#include <wx/fontenum.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_GRAPHIC_DEFAULT = wxID_HIGHEST + 100, CD_STANDARDFONT, CD_FIXEDFONT,
	CD_TEXTFONT_SIZE, CD_GRAPHICFONT, CD_GRAPHICFONT_SIZE, CD_SYMBOLFONT,
	CD_SYMBOLFONT_SIZE, CD_BASIC_FONTSIZE,
	CD_STANDARD_FONT,
	CD_FIXED_FONT,
	CD_GRAPHIC_FONT,
	CD_SYMBOL_FONT,
};

/*****************************************************
**
**   CLASS   ---   MFontEnumerator
**
******************************************************/
class MFontEnumerator: public wxFontEnumerator
{
public:
	void clear() { m_facenames.Clear(); }
	const wxArrayString& GetFacenames() const { return m_facenames; } 

protected:
	virtual bool OnFacename(const wxString& facename)
	{
		if ( ! facename.StartsWith( "." )) m_facenames.Add(facename);
		return true;
	}

private:
	wxArrayString m_facenames;
} fontEnumerator;


/*****************************************************
**
**   FontConflet   ---   Constructor
**
******************************************************/
FontConflet::FontConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->fonts )
{
	// begin wxGlade: FontConflet::FontConflet
	wxFlexGridSizer* sizer_main = new wxFlexGridSizer(1, 1, 0, 0);
	wxBoxSizer* sizer_left = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_left, 1, wxALL, 3);
	wxStaticBoxSizer* sizer_textviews = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Text Views")), wxHORIZONTAL);
	sizer_left->Add(sizer_textviews, 0, 0, 0);
	wxFlexGridSizer* grid_textviews = new wxFlexGridSizer(3, 3, 0, 0);
	sizer_textviews->Add(grid_textviews, 1, 0, 0);
	label_standardfont = new wxStaticText(sizer_textviews->GetStaticBox(), wxID_ANY, _("Standard Font"));
	grid_textviews->Add(label_standardfont, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_standardfont_facename_choices[] = {
		_("dummy"),
	};
	choice_standardfont_facename = new wxChoice(sizer_textviews->GetStaticBox(), CD_STANDARDFONT, wxDefaultPosition, wxDefaultSize, 1, choice_standardfont_facename_choices);
	grid_textviews->Add(choice_standardfont_facename, 0, wxALL|wxEXPAND, 3);
	text_standardfont = new wxTextCtrl(sizer_textviews->GetStaticBox(), -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	grid_textviews->Add(text_standardfont, 0, wxALL|wxEXPAND, 3);
	label_fixedfont = new wxStaticText(sizer_textviews->GetStaticBox(), CD_FIXEDFONT, _("Fixed Font"));
	grid_textviews->Add(label_fixedfont, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_fixedfont_facename_choices[] = {
		_("dummy"),
	};
	choice_fixedfont_facename = new wxChoice(sizer_textviews->GetStaticBox(), CD_FIXEDFONT, wxDefaultPosition, wxDefaultSize, 1, choice_fixedfont_facename_choices);
	grid_textviews->Add(choice_fixedfont_facename, 0, wxALL|wxEXPAND, 3);
	text_fixedfont = new wxTextCtrl(sizer_textviews->GetStaticBox(), -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	grid_textviews->Add(text_fixedfont, 0, wxALL|wxEXPAND, 3);
	label_fontsize = new wxStaticText(sizer_textviews->GetStaticBox(), wxID_ANY, _("Basic Font Size"));
	grid_textviews->Add(label_fontsize, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_textfont_size = new wxSpinCtrl(sizer_textviews->GetStaticBox(), CD_BASIC_FONTSIZE, wxT("6"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 6, 20);
	grid_textviews->Add(spin_textfont_size, 0, wxALL|wxEXPAND, 3);
	label_font_pt = new wxStaticText(sizer_textviews->GetStaticBox(), wxID_ANY, _("pt"));
	grid_textviews->Add(label_font_pt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticBoxSizer* sizer_graphicviews = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Graphical Views")), wxHORIZONTAL);
	sizer_left->Add(sizer_graphicviews, 0, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_symbolfont_copy = new wxFlexGridSizer(2, 3, 0, 0);
	sizer_graphicviews->Add(grid_symbolfont_copy, 1, 0, 0);
	label_graphicfont_facename = new wxStaticText(sizer_graphicviews->GetStaticBox(), wxID_ANY, _("Facename"));
	grid_symbolfont_copy->Add(label_graphicfont_facename, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_graphicfont_facename_choices[] = {
		_("dummy"),
	};
	choice_graphicfont_facename = new wxChoice(sizer_graphicviews->GetStaticBox(), CD_GRAPHICFONT, wxDefaultPosition, wxDefaultSize, 1, choice_graphicfont_facename_choices);
	grid_symbolfont_copy->Add(choice_graphicfont_facename, 0, wxALL|wxEXPAND, 3);
	text_graphicfont = new wxTextCtrl(sizer_graphicviews->GetStaticBox(), -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	grid_symbolfont_copy->Add(text_graphicfont, 0, wxALL|wxEXPAND, 3);
	label_graphicfont_size = new wxStaticText(sizer_graphicviews->GetStaticBox(), wxID_ANY, _("Font Size"));
	grid_symbolfont_copy->Add(label_graphicfont_size, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_graphicfont_size = new wxSpinCtrl(sizer_graphicviews->GetStaticBox(), CD_GRAPHICFONT_SIZE, wxT("6"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 6, 20);
	grid_symbolfont_copy->Add(spin_graphicfont_size, 0, wxALL|wxEXPAND, 3);
	label_graphicfont_pt = new wxStaticText(sizer_graphicviews->GetStaticBox(), wxID_ANY, _("pt"));
	grid_symbolfont_copy->Add(label_graphicfont_pt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticBoxSizer* sizer_symbolfont = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Symbol Font")), wxHORIZONTAL);
	sizer_left->Add(sizer_symbolfont, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_symbolfont = new wxFlexGridSizer(2, 3, 0, 0);
	sizer_symbolfont->Add(grid_symbolfont, 1, wxEXPAND, 0);
	label_symbolfont_facename = new wxStaticText(sizer_symbolfont->GetStaticBox(), wxID_ANY, _("Facename"));
	grid_symbolfont->Add(label_symbolfont_facename, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	const wxString choice_symbolfont_facename_choices[] = {
		_("dummy"),
	};
	choice_symbolfont_facename = new wxChoice(sizer_symbolfont->GetStaticBox(), CD_SYMBOLFONT, wxDefaultPosition, wxDefaultSize, 1, choice_symbolfont_facename_choices);
	grid_symbolfont->Add(choice_symbolfont_facename, 0, wxALL|wxEXPAND, 3);
	text_symbolfont = new wxTextCtrl(sizer_symbolfont->GetStaticBox(), -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	grid_symbolfont->Add(text_symbolfont, 0, wxALL|wxEXPAND, 3);
	label_symbolfont_size = new wxStaticText(sizer_symbolfont->GetStaticBox(), wxID_ANY, _("Symbol Size in Charts"));
	grid_symbolfont->Add(label_symbolfont_size, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_symbolfont_size = new wxSpinCtrl(sizer_symbolfont->GetStaticBox(), CD_SYMBOLFONT_SIZE, wxT("6"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 6, 20);
	grid_symbolfont->Add(spin_symbolfont_size, 0, wxALL|wxEXPAND, 3);
	label_symbolfont_pt = new wxStaticText(sizer_symbolfont->GetStaticBox(), wxID_ANY, _("pt"));
	grid_symbolfont->Add(label_symbolfont_pt, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	
	sizer_main->AddGrowableRow(0);
	sizer_main->AddGrowableCol(0);
	sizer_main->AddGrowableCol(1);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	fontEnumerator.EnumerateFacenames( wxFONTENCODING_SYSTEM, false );
	{
		choice_standardfont_facename->Clear();
		choice_graphicfont_facename->Clear();
		choice_symbolfont_facename->Clear();
		const wxArrayString& facenames = fontEnumerator.GetFacenames();
		for( uint i = 0; i < facenames.Count(); i++ )
		{
			//debuglog << "Facename: " << facenames[ i ] ;
			choice_standardfont_facename->Append( facenames[i] );
			choice_graphicfont_facename->Append( facenames[i] );
			choice_symbolfont_facename->Append( facenames[i] );
		}
	}

	fontEnumerator.clear();
	fontEnumerator.EnumerateFacenames( wxFONTENCODING_SYSTEM, true );
	{
		choice_fixedfont_facename->Clear();
		const wxArrayString& fixedfacenames = fontEnumerator.GetFacenames();
		for( uint i = 0; i < fixedfacenames.Count(); i++ )
		{
			// debuglog << "Facename: " << fixedfacenames[ i ] ;
			choice_fixedfont_facename->Append( fixedfacenames[i] );
		}
	}

	spin_textfont_size->SetValidator( wxGenericValidator( &cfg.textFontSize ));
	spin_graphicfont_size->SetValidator( wxGenericValidator( &cfg.graphicFontSize ));
	spin_symbolfont_size->SetValidator( wxGenericValidator( &cfg.symbolFontSize ));

	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( FontConflet::OnChoice ));
}

/*****************************************************
**
**   FontConflet   ---   OnChoice
**
******************************************************/
void FontConflet::OnChoice( wxCommandEvent& )
{
	debuglog << "FontConflet::OnChoice";
	cfg.standardFontFacename = choice_standardfont_facename->GetStringSelection();
	cfg.fixedFontFacename = choice_fixedfont_facename->GetStringSelection();
	cfg.graphicFontFacename = choice_graphicfont_facename->GetStringSelection();
	cfg.symbolFontFacename = choice_symbolfont_facename->GetStringSelection();

	guiHasChanged();
}


/*****************************************************
**
**   FontConflet   ---   TransferDataToWindow
**
******************************************************/
bool FontConflet::TransferDataToWindow()
{
	debuglog << "FontConflet::TransferDataToWindow";
	const bool b =  wxWindow::TransferDataToWindow();

	int sel = choice_standardfont_facename->FindString ( cfg.standardFontFacename );
	debuglog << "FontConflet::config2model cfg.standardFontFacename " << cfg.standardFontFacename
		<< " sel " << sel;
	choice_standardfont_facename->SetSelection( sel >= 0 ? sel : 0 );

	sel = choice_fixedfont_facename->FindString ( cfg.fixedFontFacename );
	choice_fixedfont_facename->SetSelection( sel >= 0 ? sel : 0 );

	sel = choice_graphicfont_facename->FindString ( cfg.graphicFontFacename );
	choice_graphicfont_facename->SetSelection( sel >= 0 ? sel : 0 );

	sel = choice_symbolfont_facename->FindString ( cfg.symbolFontFacename );
	choice_symbolfont_facename->SetSelection( sel >= 0 ? sel : 0 );
	return b;
}

/*****************************************************
**
**   FontConflet   ---   config2model
**
******************************************************/
void FontConflet::config2model()
{
	cfg = globalConfig;

	if ( cfg.standardFontFacename.empty())
	{
		const wxFont romanFont( 10, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
		cfg.standardFontFacename = romanFont.GetFaceName();
	}

	if ( cfg.fixedFontFacename.empty())
	{
		const wxFont modernFont( 10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
		cfg.fixedFontFacename = modernFont.GetFaceName();
	}

	if ( cfg.graphicFontFacename.empty())
	{
		wxFont graphicFont( 10, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
		cfg.graphicFontFacename = graphicFont.GetFaceName();
	}

	if ( cfg.symbolFontFacename.empty())
	{
		cfg.symbolFontFacename = SYMBOL_FONT_NAME;
	}
}

/*****************************************************
**
**   FontConflet   ---   updateUi
**
******************************************************/
void FontConflet::updateUi()
{
	debuglog << "FontConflet::updateUi";
	{
		const wxFont standardFont( cfg.textFontSize, wxFONTFAMILY_ROMAN,
			wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, cfg.standardFontFacename );
		text_standardfont->SetFont( standardFont );
		text_standardfont->ChangeValue( tr( "Meridian" ));
	}

	{
		const wxFont fixedFont( cfg.textFontSize, wxFONTFAMILY_MODERN,
			wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, cfg.fixedFontFacename );
		text_fixedfont->SetFont( fixedFont );
		text_fixedfont->ChangeValue( tr( "Ascendant" ));
	}

	{
		const wxFont graphicFont( cfg.graphicFontSize, wxFONTFAMILY_ROMAN,
			wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, cfg.graphicFontFacename );
		text_graphicfont->SetFont( graphicFont );
		text_graphicfont->ChangeValue( tr( "Jupiter" ));
	}

	{
		const wxFont symbolFont( cfg.symbolFontSize, wxFONTFAMILY_ROMAN,
			wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, cfg.symbolFontFacename );
		text_symbolfont->SetFont( symbolFont );
		text_symbolfont->ChangeValue( "ABC abc" );
	}
}


