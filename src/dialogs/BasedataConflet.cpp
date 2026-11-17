/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/BasedataConflet.cpp
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

#include "BasedataConflet.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "Horoscope.h"
#include "logger.h"
#include "MDocument.h"
#include "RearrangableListCtrl.h"
#include "Table.h"
#include "TableWriter.h"
#include "TextWidget.h"

#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/valgen.h>

extern Astroconfig *astroconfig;

// begin wxGlade: ::extracode
// end wxGlade

enum { CD_LIST = wxID_HIGHEST + 1 };

/*****************************************************
**
**   BasedataConflet   ---   Constructor
**
******************************************************/
BasedataConflet::BasedataConflet( wxWindow* parent, const Viewpoint &viewpoint )
	: ConfletIncarnation( parent, astroconfig->getViewpointConfig( viewpoint ).basedata ),
	viewpoint( viewpoint )
{
	props = ContextProps_ptr( new ContextProperties );
	props->setPreferredViewpoint( viewpoint );
	Horoscope_ptr horoscope = Horoscope_ptr( new Horoscope );
	horoscope->dl.location = astroconfig->standardLocation;
	props->setRadixHoroscope( horoscope );

	// begin wxGlade: BasedataConflet::BasedataConflet
	wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* sizer_left = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_left, 1, wxEXPAND, 0);
	sizer_list = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Tabs")), wxHORIZONTAL);
	sizer_left->Add(sizer_list, 1, wxEXPAND, 0);
	listctrl = new RearrangableListCtrlBasedata(sizer_list->GetStaticBox(), CD_LIST, cfg.basedataItems);
	sizer_list->Add(listctrl, 1, wxALL|wxEXPAND, 3);
	check_factorlist = new wxCheckBox(this, wxID_ANY, _("Show in Planet List Widget"));
	sizer_left->Add(check_factorlist, 0, wxALL, 3);
	wxBoxSizer* sizer_1 = new wxBoxSizer(wxVERTICAL);
	sizer_main->Add(sizer_1, 1, wxEXPAND, 0);
	wxStaticBoxSizer* sizer_preview = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Preview")), wxVERTICAL);
	sizer_1->Add(sizer_preview, 1, wxALL|wxEXPAND, 3);
	panel_preview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL);
	sizer_preview->Add(panel_preview, 1, wxALL|wxEXPAND, 3);
	wxFlexGridSizer* grid_previewpanel = new wxFlexGridSizer(1, 1, 3, 3);
	preview = new TextWidget(panel_preview, props, WidgetCategory::TextGeneric);
	preview->SetMinSize(wxSize(200, 220));
	grid_previewpanel->Add(preview, 1, wxALL|wxEXPAND, 3);
	
	grid_previewpanel->AddGrowableRow(0);
	grid_previewpanel->AddGrowableCol(0);
	panel_preview->SetSizer(grid_previewpanel);
	SetSizer(sizer_main);
	sizer_main->Fit(this);
	// end wxGlade

	check_factorlist->SetValidator( wxGenericValidator( &cfg.showBasedataInPlanetListWidget ));
}

/*****************************************************
**
**   BasedataConflet   ---   updateUi
**
******************************************************/
void BasedataConflet::updateUi()
{
	debuglog << "BasedataConflet::updateUi viewpoint " << viewpoint;

  Horoscope_ptr horoscope = props->getRadixHoroscope();
  assert( horoscope );
	horoscope->update();
  TableWriter extractor( props, horoscope );

	MDocument_ptr doc = preview->getEmptyDoc();
  doc->addHeader2( tr( "Basic Data" ));

  std::list<BasedataAttribute> bl;
	for( const auto &item: cfg.basedataItems )
	{
		if ( item.second ) bl.push_back( item.first );
	}
  doc->addItem( extractor.createBaseDataTable( bl ));

	preview->OnDataChanged();
}

/*****************************************************
**
**   BasedataConflet   ---   restoreDefaults
**
******************************************************/
void BasedataConflet::restoreDefaults()
{
	cfg = ViewpointConfig( viewpoint ).basedata;
	TransferDataToWindow();
}

