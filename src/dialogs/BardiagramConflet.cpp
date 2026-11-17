/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BardiagramConflet.cpp
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

#include "BardiagramConflet.h"

#include "Ashtakavarga.h"
#include "AshtakavargaDiagrams.h"
#include "BrushDialog.h"
#include "ConfigListItems.h"
#include "ContextProperties.h"
#include "DasaChart.h"
#include "GdiWidgetWrapper.h"
#include "GraphicalDasa.h"
#include "Horoscope.h"
#include "KalachakraDasa.h"
#include "Lang.h"
#include "logger.h"
#include "GdiWidget.h"
#include "Painter.h"
#include "PenDialog.h"
#include "DocHelper.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Gdiconfig *gdiconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum {
	CD_BAR_SKIN = wxID_HIGHEST + 1,
	CD_BAR_OBJECT,
	CD_BAR_PEN,
	CD_BAR_BRUSH,
	CD_CHECK_ZOOM,
	CD_SPIN_CORNER_RADIUS
};

/*************************************************//**
*
* 
*
******************************************************/
class DasaBarPreview : public GdiWidget
{
public:

	DasaBarPreview( ContextProps_ptr props, const BarDiagramConfig &cfg )
	 : GdiWidget( props, WidgetCategory::BardiagramGeneric ),
		barconfig( cfg ),
		ybar( 30 ),
		xborder( 30 )
	{
		doZoom = false;
	}

	virtual void OnDataChanged() override {}

	virtual void setLord( const DasaLord &l ) { lord = l; }

	/*****************************************************
	**
	**   DasaBarPreview   ---   doPaint
	**
	******************************************************/
	virtual void doPaint( Painter *painter, const MRect& /*refreshRect*/ ) override
	{
		debuglog << "DasaBarPreview::doPaint rect " << rect;
		barrect = MRect( rect.x + xborder, rect.y + .5 * rect.height - .5 * ybar, rect.width - 2 * xborder, ybar );
		Dasa_ptr dasa;
		DasaExpert_ptr expert;
		if ( std::holds_alternative<PlanetId>( lord ))
		{
			expert = DasaExpert_ptr( DasaExpertFactory().getDasaExpert( DasaId::Vimsottari ));
			dasa = Dasa_ptr( new Dasa( expert, nullptr, DasaLevel::Mahadasa, lord ));
		}
		else
		{
			expert = DasaExpert_ptr( DasaExpertFactory().getDasaExpert( DasaId::Kalachakra ));
			dasa = Dasa_ptr( new KalachakraDasa( expert, nullptr, DasaLevel::Mahadasa, lord ));
		}
		assert( expert );
		assert( dasa );

		GraphicalDasa_ptr gdasa( new GraphicalDasa( dasa ));
		gdasa->rect = barrect;

		painter->setDefaults();
		doLayout( painter );
		DasaBarPaintHelper helper( barconfig );
		helper.paintDasaBar( painter, gdasa, doZoom );
	}

	virtual MComplex doLayout( Painter* ) override
	{
		constexpr int tol = 20;
		barrect = MRect( rect.x + xborder, rect.y + .5 * rect.height - .5 * ybar, rect.width - 2 * xborder, ybar );
		return MComplex( rect.width, ybar + 2 * tol );
	}

	virtual WidgetResponse onResponsiveChartEvent( const MComplex &p, const int &keymod ) override
	{
		if ( ! barconfig.doItemZoom ) return false;

		const bool inside = barrect.containsPoint( p );
		debuglog << "DasaBarPreview::onResponsiveChartEvent rect " << rect
			<< " item rect " << barrect << " p " << p << " inside " << inside << " keymod " << keymod
			<< " doZoom " << doZoom;
		if ( inside != doZoom )
		{
			doZoom = ! doZoom;
			return true;
		}
		return false;
	}


private:

	const BarDiagramConfig &barconfig;

	MRect barrect;

	DasaLord lord;

	bool doZoom;

	const int ybar;	

	const int xborder;
};


/*****************************************************
**
**   BardiagramConflet   ---   Constructor
**
******************************************************/
BardiagramConflet::BardiagramConflet( wxWindow *parent  )
	: ConfletIncarnation( parent, gdiconfig->bardiagram ),
	categories (
	{
		WidgetCategory::BardiagramGeneric,
		WidgetCategory::DasaChart,
		WidgetCategory::DasaTree
	})
{
	props = ContextProps_ptr( new ContextProperties );

	// begin wxGlade: BardiagramConflet::BardiagramConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizer_left = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_left, 0, wxEXPAND, 0);
	wxBoxSizer* sizer_diagram = new wxBoxSizer(wxVERTICAL);
	sizer_left->Add(sizer_diagram, 0, wxALL|wxEXPAND, 3);
	wxStaticBoxSizer* sizer_objectstyle = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Planet Style")), wxHORIZONTAL);
	sizer_diagram->Add(sizer_objectstyle, 0, wxEXPAND, 0);
	const wxString choice_object_choices[] = {
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
		_("dummy"),
	};
	choice_object = new wxChoice(sizer_objectstyle->GetStaticBox(), CD_BAR_OBJECT, wxDefaultPosition, wxDefaultSize, 10, choice_object_choices);
	choice_object->SetToolTip(_("Sign or Lord of the Dasa"));
	choice_object->SetSelection(0);
	sizer_objectstyle->Add(choice_object, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxBoxSizer* sizer_9 = new wxBoxSizer(wxVERTICAL);
	sizer_objectstyle->Add(sizer_9, 1, wxEXPAND, 0);
	button_pen = new wxButton(sizer_objectstyle->GetStaticBox(), CD_BAR_PEN, _("Pen ..."));
	sizer_9->Add(button_pen, 0, wxALIGN_RIGHT|wxALL, 3);
	button_brush = new wxButton(sizer_objectstyle->GetStaticBox(), CD_BAR_BRUSH, _("Brush ..."));
	sizer_9->Add(button_brush, 0, wxALIGN_RIGHT|wxALL, 3);
	wxStaticBoxSizer* sizer_dasa = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Graphical Dasa")), wxVERTICAL);
	sizer_left->Add(sizer_dasa, 0, wxEXPAND, 0);
	wxFlexGridSizer* grid_dasaoptions = new wxFlexGridSizer(2, 3, 0, 0);
	sizer_dasa->Add(grid_dasaoptions, 0, wxALL, 3);
	check_zoom = new wxCheckBox(sizer_dasa->GetStaticBox(), CD_CHECK_ZOOM, _("Zoom on Mouse Over"));
	grid_dasaoptions->Add(check_zoom, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	spin_zoom = new wxSpinCtrl(sizer_dasa->GetStaticBox(), wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 1, 15);
	grid_dasaoptions->Add(spin_zoom, 0, wxALL, 3);
	wxStaticText* label_mouseover_pixel = new wxStaticText(sizer_dasa->GetStaticBox(), wxID_ANY, _("px"));
	grid_dasaoptions->Add(label_mouseover_pixel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticText* label_cornerradius = new wxStaticText(sizer_dasa->GetStaticBox(), wxID_ANY, _("Corner Radius"));
	grid_dasaoptions->Add(label_cornerradius, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 3);
	spin_cornerradius = new wxSpinCtrl(sizer_dasa->GetStaticBox(), CD_SPIN_CORNER_RADIUS, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 0, 15);
	grid_dasaoptions->Add(spin_cornerradius, 0, wxALL, 3);
	wxStaticText* label_cornerradius_pixel = new wxStaticText(sizer_dasa->GetStaticBox(), wxID_ANY, _("px"));
	grid_dasaoptions->Add(label_cornerradius_pixel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
	wxStaticBoxSizer* sizer_right = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Preview")), wxVERTICAL);
	sizer_main->Add(sizer_right, 1, wxEXPAND, 0);
	barwrapper = new GdiWidgetWrapper(sizer_right->GetStaticBox(), props, WidgetCategory::BardiagramGeneric);
	barwrapper->SetMinSize(wxSize(300, 100));
	sizer_right->Add(barwrapper, 0, wxALL|wxEXPAND, 3);
	diagramwrapper = new GdiWidgetWrapper(sizer_right->GetStaticBox(), props, WidgetCategory::BardiagramGeneric);
	diagramwrapper->SetMinSize(wxSize(300, 300));
	sizer_right->Add(diagramwrapper, 0, wxALL|wxEXPAND, 3);
	
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	Lang lang;
	//constexpr Viewpoint viewpoint = Viewpoint::Western;
	constexpr Viewpoint viewpoint = Viewpoint::Vedic;
	constexpr TextFormat format = TextFormat::Long;
	choice_object->Clear();
	assert( cfg.styles.size() == MAX_BARDIAGRAM_COLORS );
	for ( Sign rasi : Enum<Sign>() )
	{
		choice_object->Append( lang.getSignName( rasi, viewpoint, format ));
	}
	// order must be western but western list can differ if uranian flag is set
	for ( const PlanetId p : Enum<PlanetId>( PlanetId::Sun, PlanetId::Saturn ))
	{
		choice_object->Append( lang.getPlanetName( p, viewpoint, format ));
	}
	choice_object->Append( lang.getPlanetName( PlanetId::AscNode, viewpoint, format ));
	choice_object->Append( lang.getPlanetName( PlanetId::DescNode, viewpoint, format ));
	choice_object->Append( lang.getPlanetName( PlanetId::Ascendant, viewpoint, format ));
	choice_object->SetSelection( 0 );

	horoscope = Horoscope_ptr( new Horoscope());
	props->setRadixHoroscope( horoscope );
	horoscope->update();
	expert = new AshtakavargaExpert( props, Varga::Rasi );
	expert->update();

	dasaWidget = new DasaBarPreview( props, cfg );
	dasaWidget->setRect( MRect( 0, 0, 300, 100 ));
	barwrapper->setWidget( dasaWidget );
	//barwrapper->SendSizeEvent();
	//barwrapper->Refresh();

	spin_cornerradius->SetValidator( wxGenericValidator( &cfg.cornerRadius ));
	spin_zoom->SetValidator( wxGenericValidator( &cfg.itemZoom ));

	check_zoom->SetValidator( wxGenericValidator( &cfg.doItemZoom ));

	Connect( wxID_ANY, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( BardiagramConflet::OnBarSpin ));
	Connect( wxID_ANY, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BardiagramConflet::OnBarCheckBox ));
	Connect( CD_BAR_OBJECT, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BardiagramConflet::OnBarObjectChoice ));

	Connect( CD_BAR_PEN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BardiagramConflet::OnButtonBarPen ));
	Connect( CD_BAR_BRUSH, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BardiagramConflet::OnButtonBarBrush ));
	Connect( wxEVT_SIZE, wxSizeEventHandler( BardiagramConflet::OnSize ));
	Connect( wxEVT_MOTION, wxMouseEventHandler( BardiagramConflet::OnMouseMotion ));
}

/*****************************************************
**
**   BardiagramConflet   ---   Destructor
**
******************************************************/
BardiagramConflet::~BardiagramConflet()
{
	delete expert;
}

void BardiagramConflet::OnMouseMotion( wxMouseEvent& )
{
	debuglog << "BardiagramConflet::OnMouseMotion";
}

/*****************************************************
**
**   BardiagramConflet   ---   updateUi
**
******************************************************/
void BardiagramConflet::updateUi()
{
	TransferDataFromWindow();

	const wxSize size = GetSize();
	debuglog << "BardiagramConflet::updateUi size " << size.x << "/" << size.y << " cfg.doItemZoom " << std::boolalpha << cfg.doItemZoom;
	spin_zoom->Enable( cfg.doItemZoom  );

	const int sel = choice_object->GetSelection();
	if ( sel < 0 ) return;
	assert( sel < MAX_BARDIAGRAM_COLORS );
	const bool isGraha = ( sel >= 12 );

	{
		if ( isGraha )
		{
			diagramwrapper->setWidget( new AvPindaDiagram( diagramwrapper->getProps(),
				expert, PindaType::Rasi, &cfg ));
		}
		else
		{
			diagramwrapper->setWidget( new AvRasiDiagram( diagramwrapper->getProps(), expert,
				AvType::Rekha, &cfg ));
		}
	}
	diagramwrapper->OnDataChanged();
	diagramwrapper->SendSizeEvent();
	diagramwrapper->Refresh();

	debuglog << "BardiagramConflet::updateUi barwrapper rect " << dasaWidget->getRect();
	dasaWidget->setLord( BarDiagramConfig::getByGraphicalId( sel ) );
	barwrapper->OnDataChanged();
	barwrapper->SendSizeEvent();
	barwrapper->Refresh();
}

/*****************************************************
**
**   BardiagramConflet   ---   OnButtonBarPen
**
******************************************************/
void BardiagramConflet::OnButtonBarPen( wxCommandEvent& )
{
	const int sel = choice_object->GetSelection();
	if ( sel < 0 ) return;
	assert( sel < MAX_BARDIAGRAM_COLORS );
	assert( cfg.styles.size() == MAX_BARDIAGRAM_COLORS );

	PenDialog dialog( this, cfg.styles[sel].pen, cfg.styles[sel].brush );
	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg.styles[sel].pen = dialog.getPen();
		guiHasChanged();
	}
}

/*****************************************************
**
**   BardiagramConflet   ---   OnButtonBarBrush
**
******************************************************/
void BardiagramConflet::OnButtonBarBrush( wxCommandEvent& )
{
	const int sel = choice_object->GetSelection();
	if ( sel < 0 ) return;
	assert( sel < MAX_BARDIAGRAM_COLORS );
	assert( cfg.styles.size() == MAX_BARDIAGRAM_COLORS );

	BrushDialog dialog( this, cfg.styles[sel].brush );
	if ( dialog.ShowModal() == wxID_OK )
	{
		cfg.styles[sel].brush = dialog.getBrush();
		guiHasChanged();
	}
}

/*****************************************************
**
**   BardiagramConflet   ---   OnSize
**
******************************************************/
void BardiagramConflet::OnSize( wxSizeEvent &event )
{
	barwrapper->SendSizeEvent();
	barwrapper->Refresh();
	diagramwrapper->SendSizeEvent();
	diagramwrapper->Refresh();
	event.Skip();
}

