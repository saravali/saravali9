/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/DasaChartWidget.cpp
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

#include "BasicWidget.h"
#include "Commands.h"
#include "ContextProperties.h"
#include "DasaTool.h"
#include "DasaChart.h"
#include "DateFormatter.h"
#include "gdiconfig.h"
#include "GdiWidgetWrapper.h"
#include "Horoscope.h"
#include "mathbase.h"
#include "ToolPanel.h"
#include "WidgetFactory.h"

#include <wx/choicdlg.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/textdlg.h>

#include<format>

extern Gdiconfig *gdiconfig;

#define DEBUG_DASACHART

/*************************************************//**
*
*    CLASS ---   DasaChartWidget
*
******************************************************/
class DasaChartWidget : public BasicWidget
{
public:
	/*****************************************************
	**
	**   DasaChartWidget   ---   Constructor
	**
	******************************************************/
	DasaChartWidget( wxWindow *parent, ContextProps_ptr props )
		: BasicWidget( parent, props, WidgetCategory::DasaChartWidget, ToolbarWidgetId::DasaChartWidget )
	{
		gdiwidget = new DasaChart( props );
		wrapper = new GdiWidgetWrapper( this, gdiwidget );
		mainsizer->Add( wrapper, 1, wxEXPAND, 3 );
		SetSizer( mainsizer );

		assert( toolbar );
		if ( wxChoice *choice = getToolbarControl<wxChoice>( ToolbarCmd::ChoiceAntardasa ); choice )
		{
			choice_antardasas = choice;;
			choice->SetSelection( gdiconfig->prefs.dasaChartAntarDasaLevel );
		}

		OnDataChanged();
		gdiwidget->calculateViewport();

		Connect( (int)CommandId::SelectDasa, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DasaChartWidget::OnDasas ));
		Connect( (int)CommandId::Now, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DasaChartWidget::OnNow ));
		Connect( (int)CommandId::GotoYear, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DasaChartWidget::OnYear ));

		Connect( wxID_ANY, MOUSE_WHEEL_UP, wxCommandEventHandler( DasaChartWidget::OnDasaLevelUp ));
		Connect( wxID_ANY, MOUSE_WHEEL_DOWN, wxCommandEventHandler( DasaChartWidget::OnDasaLevelDown ));
		Connect( (int)ToolbarCmd::ChoiceAntardasa, wxEVT_COMMAND_CHOICE_SELECTED,
			wxCommandEventHandler( DasaChartWidget::OnChoiceAntardasa ));
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   postCreate
	**
	******************************************************/
	virtual void postCreate() override
	{
		const MRect sheetRect = gdiwidget->getSheetRect();
		gdiwidget->calculateViewport();
#ifdef DEBUG_DASACHART
		const double currentjdx = gdiwidget->jd2x( MDate::getCurrentJD());
		debuglog << "DasaChartWidget::postCreate sheetRect " << sheetRect
			<< " currentjdx " << currentjdx;
#endif

		wrapper->SetVirtualSize( sheetRect.width, sheetRect.height );
		gotoJD( MDate::getCurrentJD());

		lastZoom = props->getLogZoom();
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   OnNow
	**
	******************************************************/
	void OnNow( wxCommandEvent& )
	{
		gotoJD( MDate::getCurrentJD());
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   OnDataChanged
	**
	******************************************************/
	virtual void OnDataChanged() override
	{
#ifdef DEBUG_DASACHART
		debuglog << "DasaChartWidget::OnDataChanged() zoom " << props->getLogZoom();
#endif
		
#ifdef DEBUG_DASACHART
		if ( lastZoom != props->getLogZoom() )
		{
			const wxPoint scrollPos = wrapper->GetViewStart();
			const wxPoint mousePos = wxGetMousePosition();
			const wxPoint viewStart = GetViewStart();
			const wxSize clientSize = GetClientSize();
			const MComplex mp2 = getMousePosition();
			const int scrollx = GetScrollPos( wxHORIZONTAL );
			debuglog << "DasaChartWidget::OnDataChanged Zoom changed view start " << scrollPos.x << "/" << scrollPos.y
				<< " mouse pos " << mousePos.x << "/" << mousePos.y
				<< " view start " << viewStart.x << "/" << viewStart.y 
				<< " client size " << clientSize.x << "/" << clientSize.y 
				<< " scrollx " << scrollx
				<< " mouse date " << DateFormatter::get()->formatUTDate( gdiwidget->x2jd( mousePos.x ))
				<< " mouse pos2 " << mp2;
		}
#endif

		wrapper->invalidateLayout();
		gdiwidget->OnDataChanged();
		BasicWidget::OnDataChanged();
		lastZoom = props->getLogZoom();
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   OnYear
	**
	******************************************************/
	void OnYear( wxCommandEvent& )
	{
		const wxString defvalue = std::format( "{}", getCurrentYear() );
		wxTextEntryDialog dialog( this, _("Please enter a year"), GUI_APP_NAME, defvalue );
		if ( dialog.ShowModal() == wxID_OK )
		{
			wxString s = dialog.GetValue();
			long y = 0;
			if ( ! s.ToLong( &y ))
			{
				wxString s1 = "The value \"" + s + "\" is not valid.";
				doMessageBox( this, s1, wxOK | wxICON_ERROR | wxCENTRE );
				return;
			}
			gotoJD( MDate( (int)y, 1, 1, 0 ).jd );
		}
		wrapper->Refresh();
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   gotoJD
	**
	******************************************************/
	void gotoJD( const double &jd )
	{
#ifdef DEBUG_DASACHART
		//debuglog << "gotoJD ->Scroll " << gdiwidget->jd2x( jd ) << " -- " <<  gdiwidget->jd2x( jd ) - xoffset_on_set_jd;
#endif
		wrapper->Scroll( gdiwidget->jd2x( jd ) - xoffset_on_set_jd, 0 );
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   OnDasas
	**
	******************************************************/
	void OnDasas( wxCommandEvent& )
	{
		wxArrayString l;
		DasaTool *tool = DasaTool::get();

		for ( const DasaId id : Enum<DasaId>() )
		{
			l.Add( tool->getDasaName( id ));
		}
		wxMultiChoiceDialog dialog( this, _( "Choose Dasas" ), GUI_APP_NAME, l );

		wxArrayInt a;
		for( const DasaId d : gdiwidget->getDasas() )
		{
			a.Add( (int)d );
		}
		dialog.SetSelections( a );
		if ( dialog.ShowModal() == wxID_OK )
		{
			std::list<DasaId> l;
			for ( unsigned dasa = 0; dasa < dialog.GetSelections().Count(); dasa++ )
			{
				l.push_back( (DasaId)dialog.GetSelections()[dasa] );
			}
			if ( l.size() == 0 )
			{
				l.push_back( DasaId::Vimsottari );
			}
			gdiwidget->setDasas( l );
			OnDataChanged();
		}
		wrapper->Refresh();
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   increaseDasaLevel
	**
	******************************************************/
	void increaseDasaLevel( const int &delta )
	{
#ifdef DEBUG_DASACHART
		debuglog << "DasaChartWidget::increaseDasaLevel " << delta;
#endif
		if ( choice_antardasas )
		{
			int sel = choice_antardasas->GetSelection() + delta;
			sel = std::max( 0, sel );
			sel = std::min( 4, sel );
			choice_antardasas->SetSelection( sel );
			propagateAntardasa();
		}
	}

	/*****************************************************
	**
	**   DasaChartWidget   ---   propagateAntardasa
	**
	******************************************************/
	void propagateAntardasa()
	{
		if ( choice_antardasas )
		{
			gdiconfig->prefs.dasaChartAntarDasaLevel = choice_antardasas->GetSelection();
			gdiwidget->setAntardasaLevel( gdiconfig->prefs.dasaChartAntarDasaLevel );
			OnDataChanged();
			wrapper->Refresh();
		}
	}

	void OnDasaLevelUp( wxCommandEvent& ) { increaseDasaLevel( 1 ); }
	void OnDasaLevelDown( wxCommandEvent& ) { increaseDasaLevel( -1 ); }
	void OnChoiceAntardasa( wxCommandEvent& ) { propagateAntardasa(); }

	DasaChart *gdiwidget = nullptr;

	wxChoice *choice_antardasas = nullptr;

	GdiWidgetWrapper *wrapper = nullptr;

	int lastZoom = 0;

	constexpr static double xoffset_on_set_jd = 300;
};

/**************************************************************
***
**   WidgetFactory   ---   createDasaChartWidget
***
***************************************************************/
BasicWidget *WidgetFactory::createDasaChartWidget( wxWindow *parent )
{
	return new DasaChartWidget( parent, props );
}

