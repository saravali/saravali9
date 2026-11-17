/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/CommonControls.cpp
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

#include "CommonControls.h"

#include "astroconfig.h"
#include "Commands.h"
#include "DocHelper.h"
#include "enumserializer.h"
#include "FileConfig.h"
#include "gdiconfig.h"
#include "ImageProvider.h"
#include "Lang.h"
#include "logger.h"
#include "Painter.h"
#include "PrintoutConfig.h"
#include "TextValidator.h"

#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/filename.h>
#include <wx/graphics.h>
#include <wx/popupwin.h>
#include <wx/sizer.h>
#include <wx/settings.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;
extern Gdiconfig *gdiconfig;

/*****************************************************
**
**   YearLengthPanel   ---   Constructor
**
******************************************************/
YearLengthPanel::YearLengthPanel( wxWindow *parent, const int &id, YearLengthConfig &ylcfg )
	: wxPanel( parent, id ),
	cfg( ylcfg )
{
	wxStaticBoxSizer* sizer_yl = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Year Length")), wxVERTICAL);
	wxFlexGridSizer* grid_yl = new wxFlexGridSizer(2, 2, 0, 0);
	sizer_yl->Add(grid_yl, 1, wxEXPAND, 0);
	wxStaticText *label_yl = new wxStaticText(this, wxID_ANY, _("Length"));
	grid_yl->Add(label_yl, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 3);

	const wxString type_choices[] = { "365.25", tr( "Tropical (365.2422)" ), "365", "360", tr( "Custom" ) };
	choice_type = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 5, type_choices );
	choice_type->SetToolTip( _( "Year Length" ));
	grid_yl->Add( choice_type, 1, wxALIGN_RIGHT|wxALL|wxEXPAND, 3);

	label_custom_value = new wxStaticText(this, wxID_ANY, _("Custom Length"));
	grid_yl->Add(label_custom_value, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	text_custom_value = new wxTextCtrl(this, wxID_ANY, wxEmptyString);
	grid_yl->Add(text_custom_value, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 3);
	
	grid_yl->AddGrowableCol(1);
	SetSizer(sizer_yl);
	sizer_yl->Fit(this);

	choice_type->SetValidator( wxGenericValidator( &cfg.type ));
	text_custom_value->SetValidator( DoubleValidator( &cfg.customValue, 300, 400 ));
	TransferDataToWindow();
	updateUi();

	Connect( wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( YearLengthPanel::OnChoice ));
}

/*****************************************************
**
**   YearLengthPanel   ---   updateUi
**
******************************************************/
void YearLengthPanel::OnChoice( wxCommandEvent &event )
{
	event.Skip();
	updateUi();
}

/*****************************************************
**
**   YearLengthPanel   ---   updateUi
**
******************************************************/
void YearLengthPanel::updateUi()
{
	debuglog << "YearLengthPanel::updateUi";
	const bool activate = ( choice_type->GetSelection() == 4 );
	label_custom_value->Enable( activate );
	text_custom_value->Enable( activate );
}

/*****************************************************
**
**   DirectionModeChoice   ---   Constructor
**
******************************************************/
DirectionModeChoice::DirectionModeChoice( wxWindow *parent, const int &id )
	: wxChoice( parent, id ) 
{
	Lang lang;

	SetToolTip( tr( "Direction Mode" ));
	for( const HoroscopeRole &role : Enum<HoroscopeRole>( HoroscopeRole::Transit, HoroscopeRole::Profections ))
	{
		Append( lang.getHoroscopeRoleName( role ));
		debuglog << "FRITZ " << lang.getHoroscopeRoleName( role ) << " " << role;
	}
	SetSize( GetBestSize());
}

/*****************************************************
**
**   DirectionModeChoice   ---   getDirectionMode
**
******************************************************/
HoroscopeRole DirectionModeChoice::getDirectionMode() const
{
	return (HoroscopeRole)( GetSelection() + basic_offset );
}

/*****************************************************
**
**   DirectionModeChoice   ---   setDirectionMode
**
******************************************************/
void DirectionModeChoice::setDirectionMode( const HoroscopeRole &role )
{
	if ( role >= HoroscopeRole::Transit && role <= HoroscopeRole::Last )
	{
		SetSelection( (int)role - basic_offset );
	}
	else SetSelection( 0 );
}

/*****************************************************
**
**   TextViewModeChoice   ---   Constructor
**
******************************************************/
TextViewModeChoice::TextViewModeChoice( wxWindow *parent, const int &id, const int /* selected */ )
	: wxChoice( parent, id, wxDefaultPosition, wxDefaultSize )
{
	Lang lang;
	recreateModes( astroconfig->general.defaultViewpoint );

	for( const WidgetCategory &cat : Enum<WidgetCategory>(
		WidgetCategory::FirstComprehensiveTextWidgetItem,
		WidgetCategory::LastComprehensiveTextWidgetItem ))
	{
		//Append( enum_to_string( cat ));
		Append( lang.getWidgetCategoryName( cat ));
	}
	SetToolTip( _( "Mode" ));
	SetSize( DoGetBestSize());
}

/*****************************************************
**
**   TextViewModeChoice   ---   setSelectedCategory
**
******************************************************/
void TextViewModeChoice::setSelectedCategory( const WidgetCategory &cat )
{
	for( uint i = 0; i < cats.size(); i++ )
	{
		if ( cats[i] == cat )
		{
			SetSelection( i );
			break;
		}
	}
}

/*****************************************************
**
**   TextViewModeChoice   ---   recreateModes
**
******************************************************/
void TextViewModeChoice::recreateModes( const Viewpoint& /*viewpoint*/ )
{
	cats.clear();
	for( const WidgetCategory &cat : Enum<WidgetCategory>( WidgetCategory::TextBaseData,
		WidgetCategory::TextWesternAspectList ))
	{
		cats.push_back( cat );
	}
	/*
	switch( astroconfig->general.viewpointChangePolicy )
  {
    case ViewpointChangePolicy::Flexible:
      return WidgetCategory::UniversalChart;
    break;
    case ViewpointChangePolicy::Fixed:
      return astroconfig->general.defaultViewpoint == Viewpoint::Vedic ?
        WidgetCategory::VargaChart : WidgetCategory::WesternChart;
    break;
    case ViewpointChangePolicy::Medium:
      return props->getPreferredViewpoint() == Viewpoint::Vedic ?
        WidgetCategory::VargaChart : WidgetCategory::WesternChart;


	modes =
	{
		TextReportMode::BaseData,
		TextReportMode::ArabicParts,
		TextReportMode::Ashtakavarga,
		TextReportMode::ObjectBalance,
		TextReportMode::DasaShort,
		TextReportMode::DasaCompact,
		TextReportMode::DasaComprehensive,
		TextReportMode::Houses,
		TextReportMode::Jaimini,
		TextReportMode::Kp,
		TextReportMode::Nakshatra,
	};
	switch( astroconfig->general.viewpointChangePolicy )
  {
    case ViewpointChangePolicy::Flexible:
      modes.push_back( TextReportMode::PlanetListUniversal );
    break;
    case ViewpointChangePolicy::Fixed:
    case ViewpointChangePolicy::Medium:
		default:
			modes.insert( modes.end(),
			{
				TextReportMode::PlanetListAstronomical,
				TextReportMode::PlanetListUranian,
				TextReportMode::PlanetListVedic,
				TextReportMode::PlanetListWestern
			});
    break;
	}
	modes.insert( modes.end(),
	{
		TextReportMode::Shadbala,
		TextReportMode::Varga,
		TextReportMode::VedicAspectarium,
		TextReportMode::WesternAspectarium,
		TextReportMode::WesternAspectList
	});
	*/
}

/*****************************************************
**
**   TextViewModeChoice   ---   getSelectedCategory
**
******************************************************/
WidgetCategory TextViewModeChoice::getSelectedCategory()
{
	return getCategoryForSelection( GetSelection() );
}

/*****************************************************
**
**   TextViewModeChoice   ---   getCategoryForSelection
**
******************************************************/
WidgetCategory TextViewModeChoice::getCategoryForSelection( const int &sel )
{
	if ( sel < 0 || sel >= (int)cats.size())
	{
		errorlog << "Wrong report mode " << sel;
		return WidgetCategory::None;
	}
	return cats[sel];
}

/*****************************************************
**
**   TimezoneChoice   ---   Constructor
**
******************************************************/
TimezoneChoice::TimezoneChoice( wxWindow *parent, const int &id ) : wxChoice( parent, id )
{
	SetToolTip( tr( "Time Zone" ));
	Append( tr( "Universal Time (UT)" ));
	Append( tr( "Local Time" ));
	SetSelection( 0 );
	SetSize( GetBestSize());
}

/*****************************************************
**
**   BackgroundImageChoice   ---   Constructor
**
******************************************************/
BackgroundImageChoice::BackgroundImageChoice( wxWindow *parent, const int &id )
	: wxChoice( parent, id, wxDefaultPosition, wxDefaultSize )
{
	FileConfig *fc = FileConfig::get();
	const std::list<wxString> extensions = { "jpg", "jpeg", "png", "xpm" };

	for( const auto &f : fc->getMatchingFiles( fc->getResourceDir( ConfigResource::Pic ), extensions, false ))
	{
		wxFileName fn( f );
		Append( fn.GetFullName() );
	}
	SetToolTip( tr( "Background image" ));
	SetSize( DoGetBestSize());
}

/*****************************************************
**
**   ToolbarLabelCtrl   ---   Constructor
**
******************************************************/
ToolbarLabelCtrl::ToolbarLabelCtrl( wxWindow *parent, wxWindowID id, wxPoint pos, wxSize size, wxString /* tooltip */ )
	: wxControl( parent, id, pos, size )
{
	wxPanel *panel = new wxPanel(this, -1, pos, size, wxEXPAND | wxSUNKEN_BORDER | wxTAB_TRAVERSAL );
	wxBoxSizer* sizerp = new wxBoxSizer( wxVERTICAL );
	wxBoxSizer* sizert = new wxBoxSizer( wxVERTICAL );
	sizerp->Add( panel, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL );

	const wxString defaultText = "";
	theText = new wxStaticText( panel, -1, defaultText, wxDefaultPosition, size );
	sizert->Add( theText, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 3 );
	panel->SetSizer(sizert);
	sizert->Fit(panel);
	sizerp->Fit(this);
	sizerp->SetSizeHints(this);
	sizert->SetSizeHints(panel);
	SetSizer(sizerp);
	Layout();
}

/*****************************************************
**
**   ToolbarLabelCtrl   ---   SetLabel
**
******************************************************/
void ToolbarLabelCtrl::SetLabel( const wxString &s )
{
	theText->SetLabel( s );
}

/*************************************************//**
*
*  Popup window for conext help
*
******************************************************/
class HelpPopupWidget : public wxPopupTransientWindow
{
public:
  HelpPopupWidget( wxWindow *parent, wxString title, wxString message )
   : wxPopupTransientWindow( parent )
  {
    wxPanel *panel = new wxPanel( this, wxID_ANY );

    wxStaticText *text1 = new wxStaticText( panel, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    wxFont font = text1->GetFont();
    font.SetWeight( wxFONTWEIGHT_BOLD );
    text1->SetFont( font );

    wxStaticText *text2 = new wxStaticText( panel, wxID_ANY, message );

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( text1, 0, wxALL | wxEXPAND | wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL, 3 );
    sizer->Add( new wxStaticLine( panel ), 0, wxALL | wxEXPAND, 0 );
    sizer->Add( text2, 0, wxALL, 3 );
    panel->SetSizer( sizer );
    sizer->Fit( panel );
    SetClientSize( panel->GetSize() );
  }
};

/*****************************************************
**
**   HelpButton   ---   Constructor
**
******************************************************/
HelpButton::HelpButton( wxWindow *parent, const wxString title, const wxString contents )
	: wxBitmapButton( parent, -1, ImageProvider::get()->getBitmap( BitmapId::Tip, IconSize::Small )),
		title( title ),
		contents( contents )
{
	SetToolTip( _( "Context Help" ));
	Connect( wxID_ANY, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( HelpButton::OnButton ));
}

/*****************************************************
**
**   HelpButton   ---   OnButton
**
******************************************************/
void HelpButton::OnButton( wxCommandEvent& )
{
  HelpPopupWidget *popup = new HelpPopupWidget( this, title, contents );

  wxMouseState m = wxGetMouseState();

  const wxPoint pos = m.GetPosition();
  popup->Position( pos, wxSize( 0, 0 ) );
  popup->Popup();
}

/*************************************************//**
*
*    StringPreviewPanel   ---   Constructor
*
******************************************************/
StringPreviewPanel::StringPreviewPanel( wxWindow* parent ) : wxPanel( parent, -1, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE )
{
	textColor = gdiconfig->colors.textFgColor;
	brush = gdiconfig->colors.bgColor;

	Connect( wxEVT_PAINT, wxPaintEventHandler( StringPreviewPanel::OnPaint ));
}

/*************************************************//**
*
*    StringPreviewPanel   ---   OnPaint
*
******************************************************/
void StringPreviewPanel::OnPaint( wxPaintEvent& )
{
	wxCoord h, w;
	GetSize( &w, &h );

	wxPaintDC dc( this );
	PrepareDC( dc );

	wxGraphicsContext *gc = wxGraphicsContext::Create( dc );
	assert( gc );
	Painter *painter = PainterFactory().createGcPainter( gc );
	assert( painter );
	painter->setBrush( brush );
	painter->setTransparentPen();

	MRect rect ( 0, 0, w, h );
	painter->drawRectangle( rect );

	painter->setDefaults();
	painter->setTextColor( textColor );
	painter->setGraphicFont();

	DocHelper( painter ).formatAndDrawTextItem( getStringContents(), rect, Align::Center );
	delete painter;
	delete gc;
}


