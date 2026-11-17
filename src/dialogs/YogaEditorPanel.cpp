/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/dialogs/YogaEditorPanel.cpp
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

#include "YogaEditorPanel.h"

#include "Commands.h"
#include "constants.h"
#include "DragChart.h"
#include "guibase.h"
#include "ImageProvider.h"
#include "logger.h"

#include <wx/bitmap.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>

#include <set>

// begin wxGlade: ::extracode
// end wxGlade

/*****************************************************
**
**   YogaEditorPanel   ---   Constructor
**
******************************************************/
YogaEditorPanel::YogaEditorPanel( wxWindow* parent, std::vector<YogaConfig> &yogas )
	: wxPanel( parent, -1, wxPoint( 0, 0 ), wxDefaultSize, wxTAB_TRAVERSAL | wxWANTS_CHARS | wxSUNKEN_BORDER ),
		yogas( yogas )
{
    // begin wxGlade: YogaEditorPanel::YogaEditorPanel
    wxBoxSizer* sizer_main = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBoxSizer* sizer_yogalist = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Yogas")), wxHORIZONTAL);
    sizer_main->Add(sizer_yogalist, 0, wxEXPAND, 0);
    list_yogas = new wxListCtrl(this, YE_YOGA_LIST, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxFULL_REPAINT_ON_RESIZE|wxLC_AUTOARRANGE|wxLC_NO_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL);
    list_yogas->SetMinSize(wxSize(250, 300));
    sizer_yogalist->Add(list_yogas, 1, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_yoga_buttons = new wxBoxSizer(wxVERTICAL);
    sizer_yogalist->Add(sizer_yoga_buttons, 0, wxALIGN_CENTER_VERTICAL, 0);
    button_up = new wxBitmapButton(this, YE_UP, wxNullBitmap);
    button_up->SetToolTip(_("Move current Yoga up"));
    button_up->SetSize(button_up->GetBestSize());
    sizer_yoga_buttons->Add(button_up, 0, wxALL, 3);
    button_down = new wxBitmapButton(this, YE_DOWN, wxNullBitmap);
    button_down->SetToolTip(_("Move current Yoga down"));
    button_down->SetSize(button_down->GetBestSize());
    sizer_yoga_buttons->Add(button_down, 0, wxALL, 3);
    button_new_yoga = new wxBitmapButton(this, YE_NEW_YOGA, wxNullBitmap);
    button_new_yoga->SetToolTip(_("Insert new Yoga at current position"));
    button_new_yoga->SetSize(button_new_yoga->GetBestSize());
    sizer_yoga_buttons->Add(button_new_yoga, 0, wxALL, 3);
    button_delete_yoga = new wxBitmapButton(this, YE_DELETE_YOGA, wxNullBitmap);
    button_delete_yoga->SetToolTip(_("Delete current Yoga"));
    button_delete_yoga->SetSize(button_delete_yoga->GetBestSize());
    sizer_yoga_buttons->Add(button_delete_yoga, 0, wxALL, 3);
    button_copy_yoga = new wxBitmapButton(this, YE_COPY_YOGA, wxNullBitmap);
    button_copy_yoga->SetToolTip(_("Copy current Yoga"));
    button_copy_yoga->SetSize(button_copy_yoga->GetBestSize());
    sizer_yoga_buttons->Add(button_copy_yoga, 0, wxALL, 3);
    wxStaticBoxSizer* sizer_right = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Properties of Selected Yoga")), wxHORIZONTAL);
    sizer_main->Add(sizer_right, 1, wxALL|wxEXPAND, 3);
    wxFlexGridSizer* grid_right = new wxFlexGridSizer(5, 1, 0, 0);
    sizer_right->Add(grid_right, 1, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_description = new wxBoxSizer(wxHORIZONTAL);
    grid_right->Add(sizer_description, 0, wxALL|wxEXPAND, 3);
    label_description = new wxStaticText(this, YE_DESCRIPTION, _("Description"));
    label_description->SetMinSize(wxSize(100, -1));
    sizer_description->Add(label_description, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    text_description = new wxTextCtrl(this, YE_TEXT_DESCRIPTION, wxEmptyString);
    text_description->SetMinSize(wxSize(300, -1));
    sizer_description->Add(text_description, 1, wxALL|wxEXPAND, 3);
    wxGridSizer* grid_sizer_source_group = new wxGridSizer(2, 3, 0, 0);
    grid_right->Add(grid_sizer_source_group, 0, wxALL, 3);
    label_group = new wxStaticText(this, wxID_ANY, _("Group"));
    label_group->SetMinSize(wxSize(100, -1));
    grid_sizer_source_group->Add(label_group, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_group_choices[] = {
        _("dummy"),
    };
    choice_group = new wxChoice(this, YE_CHOICE_GROUP, wxDefaultPosition, wxDefaultSize, 1, choice_group_choices);
    choice_group->SetMinSize(wxSize(150, -1));
    grid_sizer_source_group->Add(choice_group, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    button_new_group = new wxBitmapButton(this, YE_NEW_GROUP, wxNullBitmap);
    button_new_group->SetMinSize(wxSize(32, 32));
    button_new_group->SetToolTip(_("Create a new group"));
    grid_sizer_source_group->Add(button_new_group, 0, wxALL, 3);
    label_source = new wxStaticText(this, wxID_ANY, _("Source"));
    label_source->SetMinSize(wxSize(100, -1));
    grid_sizer_source_group->Add(label_source, 1, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    const wxString choice_source_choices[] = {
        _("dummy"),
        wxEmptyString,
    };
    choice_source = new wxChoice(this, YE_CHOICE_SOURCE, wxDefaultPosition, wxDefaultSize, 2, choice_source_choices);
    choice_source->SetMinSize(wxSize(150, -1));
    grid_sizer_source_group->Add(choice_source, 0, wxALIGN_CENTER_VERTICAL|wxALL, 3);
    button_new_source = new wxBitmapButton(this, YE_NEW_SOURCE, wxNullBitmap);
    button_new_source->SetMinSize(wxSize(32, 32));
    button_new_source->SetToolTip(_("Create a new source"));
    grid_sizer_source_group->Add(button_new_source, 0, wxALL, 3);
    wxStaticBoxSizer* sizer_effect = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Effect")), wxVERTICAL);
    grid_right->Add(sizer_effect, 0, wxALL|wxEXPAND, 3);
    text_effect = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    text_effect->SetMinSize(wxSize(300, 80));
    sizer_effect->Add(text_effect, 1, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_rule = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Rule")), wxVERTICAL);
    grid_right->Add(sizer_rule, 0, wxALL|wxEXPAND, 3);
    text_rule = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    text_rule->SetMinSize(wxSize(300, 80));
    sizer_rule->Add(text_rule, 1, wxALL|wxEXPAND, 3);
    wxStaticBoxSizer* sizer_dragchart = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Drag and Drop Chart")), wxHORIZONTAL);
    grid_right->Add(sizer_dragchart, 2, wxALL|wxEXPAND, 3);
    drag_chart = new DragChart(this);
    drag_chart->SetMinSize(wxSize(340, 100));
    sizer_dragchart->Add(drag_chart, 2, wxALL|wxEXPAND, 3);
    wxBoxSizer* sizer_evaluate = new wxBoxSizer(wxVERTICAL);
    sizer_dragchart->Add(sizer_evaluate, 0, wxALL, 3);
    button_parse_rule = new wxButton(this, YE_PARSE_RULE, _("Parse Rule"));
    button_parse_rule->SetToolTip(_("Parse the current rule for syntactical correctness"));
    sizer_evaluate->Add(button_parse_rule, 0, wxALL, 3);
    button_evaluate = new wxButton(this, YE_EVALUATE, _("Evaluate"));
    button_evaluate->SetToolTip(_("Evaluate the rule in the chart"));
    sizer_evaluate->Add(button_evaluate, 0, wxALL, 3);
    button_clear_chart = new wxButton(this, YE_CLEAR_CHART, _("Clear Chart"));
    button_clear_chart->SetToolTip(_("Remove all objects from the chart"));
    sizer_evaluate->Add(button_clear_chart, 0, wxALL, 3);
    
    grid_right->AddGrowableRow(2);
    grid_right->AddGrowableRow(3);
    grid_right->AddGrowableRow(4);
    grid_right->AddGrowableCol(0);
    SetSizer(sizer_main);
    sizer_main->Fit(this);
    // end wxGlade

	// remove dummy
	choice_group->Clear();
	choice_source->Clear();

	// setup list box
	constexpr int w = 200;
	//const int imagesize = 20;
	ImageProvider *ip = ImageProvider::get();

	wxListItem itemCol;
	itemCol.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;
	itemCol.m_text = "dummy";
	list_yogas->InsertColumn( 0, itemCol );
	list_yogas->SetColumnWidth( 0, w );

	// setup bitmap buttons
	button_new_group->SetBitmapLabel( ip->getBitmap( BitmapId::New, IconSize::Small ));
	button_new_source->SetBitmapLabel( ip->getBitmap( BitmapId::New, IconSize::Small ));

	button_up->SetBitmapLabel( ip->getBitmap( BitmapId::ArrowUp, IconSize::Small ));
	button_down->SetBitmapLabel( ip->getBitmap( BitmapId::ArrowDown, IconSize::Small ));
	button_new_yoga->SetBitmapLabel( ip->getBitmap( BitmapId::New, IconSize::Small ));
	button_delete_yoga->SetBitmapLabel( ip->getBitmap( BitmapId::Close, IconSize::Small ));
	button_copy_yoga->SetBitmapLabel( ip->getBitmap( BitmapId::Copy, IconSize::Small ));

	propagateUserInput = true;
	dirty = false;

	Connect( YE_DOWN, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorPanel::OnYogaListItemDown ));
	Connect( YE_UP, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorPanel::OnYogaListItemUp ));
	Connect( YE_NEW_YOGA, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YogaEditorPanel::OnYogaListNewItem ));
	Connect( YE_DELETE_YOGA, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( YogaEditorPanel::OnYogaListDeleteItem ));
	Connect( YE_COPY_YOGA, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler( YogaEditorPanel::OnYogaListCopyItem ));

	// events in Yoga list box
	Connect( YE_YOGA_LIST, wxEVT_COMMAND_LIST_ITEM_SELECTED,
		wxListEventHandler( YogaEditorPanel::OnYogaListLeftSelected ));

	// changes events of input items
	Connect( wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( YogaEditorPanel::OnFieldChanged ));

	Connect( YE_CHOICE_GROUP, wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler( YogaEditorPanel::OnFieldChanged ));
	Connect( YE_CHOICE_SOURCE, wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler( YogaEditorPanel::OnFieldChanged ));

	Connect( YE_CHECK_VARGAS, wxEVT_COMMAND_CHECKBOX_CLICKED,
		wxCommandEventHandler( YogaEditorPanel::OnFieldChanged ));

	updateFieldStatus();
}

/*****************************************************
**
**   YogaEditorPanel   ---   addSource
**
******************************************************/
void YogaEditorPanel::addSource( wxString s )
{
	choice_source->Append( s );
	choice_source->SetStringSelection( s );
	storeFields();
}

/*****************************************************
**
**   YogaEditorPanel   ---   addGroup
**
******************************************************/
void YogaEditorPanel::addGroup( wxString s )
{
	choice_group->Append( s );
	choice_group->SetStringSelection( s );
	storeFields();
}

/*****************************************************
**
**   YogaEditorPanel   ---   loadYogas
**
******************************************************/
void YogaEditorPanel::loadYogas()
{
	std::set<wxString> sources, groups;

	propagateUserInput = false;

	list_yogas->DeleteAllItems();

	uint i = 0;
	for ( const auto &yoga : yogas )
	{
		list_yogas->InsertItem( i, yoga.description );
		sources.insert( yoga.source );
		groups.insert( yoga.group );
		++i;
	}
	list_yogas->SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	list_yogas->Arrange();

	choice_group->Clear();
	for ( const auto &g : groups )
	{
		choice_group->Append( g );
	}

	choice_source->Clear();
	for ( const auto &s : sources )
	{
		choice_source->Append( s );
	}

	if ( yogas.size() > 0 )
	{
		choice_group->SetStringSelection( yogas.front().group );
		choice_source->SetStringSelection( yogas.front().source );
	}
	else
	{
		choice_group->SetSelection( 0 );
		choice_source->SetSelection( 0 );
	}

	propagateUserInput = true;
}

/*****************************************************
**
**   YogaEditorPanel   ---   getCurrentItem
**
******************************************************/
YogaConfig *YogaEditorPanel::getCurrentItem()
{
	long item = -1;
	item = list_yogas->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if ( item < 0 ) return nullptr;

	assert( item <= (long)yogas.size());

	auto iter = yogas.begin();
	std::advance( iter, item );
	return &*iter;
}

/*****************************************************
**
**   YogaEditorPanel   ---   getYogaConfig
**
******************************************************/
YogaConfig &YogaEditorPanel::getYogaConfig( const long &item )
{
	assert( item >= 0 && item <= (long)yogas.size());
	auto iter = yogas.begin();
	std::advance( iter, item );
	return *iter;
}

/*****************************************************
**
**   YogaEditorPanel   ---   OnYogaListLeftSelected
**
******************************************************/
void YogaEditorPanel::OnYogaListLeftSelected( wxListEvent& )
{
	YogaConfig *yoga = getCurrentItem();
	if ( yoga )
	{
		propagateUserInput = false;
		text_description->SetValue( yoga->description );

		choice_group->SetStringSelection( yoga->group );
		choice_source->SetStringSelection( yoga->source );

		text_effect->SetValue( yoga->effect );
		text_rule->SetValue( yoga->rule );
		updateFieldStatus();
		propagateUserInput = true;
	}
}

/*****************************************************
**
**   YogaEditorPanel   ---   OnFieldChanged
**
******************************************************/
void YogaEditorPanel::OnFieldChanged( wxCommandEvent& )
{
	debuglog << "YogaEditorPanel::OnFieldChanged";

	if ( ! propagateUserInput ) return;
	storeFields();
}

/*****************************************************
**
**   YogaEditorPanel   ---   storeFields
**
******************************************************/
void YogaEditorPanel::storeFields()
{
	long item = -1;
	item = list_yogas->GetNextItem( item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if ( item < 0 ) return;

	YogaConfig &yoga = getYogaConfig( item );
	yoga.description = text_description->GetValue();
	yoga.effect = text_effect->GetValue();
	yoga.group = choice_group->GetStringSelection();
	yoga.source = choice_source->GetStringSelection();
	yoga.rule = text_rule->GetValue();

	updateLabels();
	/*
	if ( event.GetId() == YE_TEXT_DESCRIPTION )
	{
		list_yogas->SetItemText( item, yoga.description );
	}
	*/
	updateFieldStatus();
	propagateDirtyEvent();
}

/*****************************************************
**
**   YogaEditorPanel   ---   updateFieldStatus
**
******************************************************/
void YogaEditorPanel::updateFieldStatus()
{
	int size = yogas.size();
	const long item = size > 0 ? list_yogas->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED ) : -1;
	debuglog << "YogaEditorPanel::updateFieldStatus size " << size << " item " << item;

	// only if selected item is not first
	button_up->Enable( size > 0 && item > 0 );

	// only if selected item is not last
	button_down->Enable( size > 0 && item < size -1 );

	// only if there is a selected item
	button_delete_yoga->Enable( size > 0 && item >= 0 );
	button_copy_yoga->Enable( size > 0 && item >= 0 );

	choice_group->Enable( size > 0 );
	choice_source->Enable( size > 0 );
	text_description->Enable( size > 0 );
	text_effect->Enable( size > 0 );
	text_rule->Enable( size > 0 );

	// Rule evaluation buttons, only if rule text filed is not empty
	button_parse_rule->Enable( text_rule->IsEnabled() && ! text_rule->IsEmpty() );
	button_evaluate->Enable( text_rule->IsEnabled() && ! text_rule->IsEmpty() );
}

/*****************************************************
**
**   YogaEditorPanel   ---   getCurrentItemId
**
******************************************************/
long YogaEditorPanel::getCurrentItemId()
{
	return list_yogas->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
}

/*****************************************************
**
**   YogaEditorPanel   ---   getItemAt
**
******************************************************/
std::vector<YogaConfig>::iterator YogaEditorPanel::getItemAt( const long &index )
{
	assert( index >= 0 && index < (long)yogas.size());

  std::vector<YogaConfig>::iterator iter = yogas.begin();
  std::advance( iter, index );
	return iter;
}

/*****************************************************
**
**   YogaEditorPanel   ---   updateLabels
**
******************************************************/
void YogaEditorPanel::updateLabels()
{
	debuglog << "YogaEditorPanel::updateLabels yogas " << yogas.size() << " list size " << list_yogas->GetItemCount();
	assert( (long)yogas.size() == list_yogas->GetItemCount());

	int i = 0;
	for( const YogaConfig &c : yogas )
	{
		//debuglog << "Set item " << i << " value " << c.description;
		list_yogas->SetItem( i, 0, c.description );
		++i;
	}
}

/*****************************************************
**
**   YogaEditorPanel   ---   OnYogaListItemUp
**
******************************************************/
void YogaEditorPanel::OnYogaListItemUp( wxCommandEvent& )
{
	const long item = getCurrentItemId();
	if ( item < 1  ) return;

  std::vector<YogaConfig>::iterator iter = getItemAt( item );
  std::vector<YogaConfig>::iterator iter2 = getItemAt( item - 1 );
	std::swap( *iter, *iter2 );
	list_yogas->SetItemState( item - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	updateLabels();
	updateFieldStatus();
	propagateDirtyEvent();
}

/*****************************************************
**
**   YogaEditorPanel   ---   OnYogaListItemDown
**
******************************************************/
void YogaEditorPanel::OnYogaListItemDown( wxCommandEvent& )
{
	const long item = getCurrentItemId();
	if ( item >= (long)yogas.size() -1  ) return;

  std::vector<YogaConfig>::iterator iter = getItemAt( item );
  std::vector<YogaConfig>::iterator iter2 = getItemAt( item + 1 );
	std::swap( *iter, *iter2 );
	list_yogas->SetItemState( item + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	updateLabels();
	updateFieldStatus();
	propagateDirtyEvent();
}

/*****************************************************
**
**   YogaEditorPanel   ---   newItem
**
******************************************************/
void YogaEditorPanel::newItem()
{
	const long index = getCurrentItemId();
	//const long insertIndex = index + 1;
	const long insertIndex = std::min( index + 1, (long)yogas.size());
	debuglog << "YogaEditorPanel::newItem index " << index << " size " << yogas.size() <<  " insert at " << insertIndex;

	YogaConfig yc;
	yc.description = tr( "New Yoga" );

	if ( index == -1 )
	{
		yogas.push_back( yc );
		list_yogas->InsertItem( 0, yc.description );
	}
	else if ( index == (int)yogas.size() - 1 )
	{
		yogas.push_back( yc );
		list_yogas->InsertItem( list_yogas->GetItemCount(), yc.description );
	}
	else
	{
		yogas.insert( getItemAt( insertIndex ), yc );
		list_yogas->InsertItem( insertIndex, yc.description );
	}
	updateLabels();
	updateFieldStatus();
	list_yogas->SetItemState( insertIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	setDirty();
	propagateDirtyEvent();
}

/*****************************************************
**
**   YogaEditorPanel   ---   OnYogaListDeleteItem
**
******************************************************/
void YogaEditorPanel::OnYogaListDeleteItem( wxCommandEvent& )
{
	const long index = getCurrentItemId();
	debuglog << "YogaEditorPanel::OnYogaListDeleteItem current item ID " << index;

	if ( index < 0  ) return;
	yogas.erase( getItemAt( index ));
	list_yogas->DeleteItem( index );
	updateLabels();
	updateFieldStatus();
	if ( index < (long)yogas.size()) list_yogas->SetItemState( index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	propagateDirtyEvent();
}

/*****************************************************
**
**   YogaEditorPanel   ---   OnYogaListCopyItem
**
******************************************************/
void YogaEditorPanel::OnYogaListCopyItem( wxCommandEvent& )
{
	const long index = getCurrentItemId();
	const long insertIndex = std::min( index + 1, (long)yogas.size() );

	YogaConfig yc = YogaConfig( getYogaConfig( index ));

	yc.description = tr( "Copy of" ) + " " + yc.description;

	if ( index == (int)yogas.size() - 1 )
	{
		yogas.push_back( yc );
		list_yogas->InsertItem( list_yogas->GetItemCount(), yc.description );
	}
	else
	{
		yogas.insert( getItemAt( insertIndex ), yc );
		list_yogas->InsertItem( insertIndex, yc.description );
	}
	list_yogas->SetItemState( insertIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	updateLabels();
	updateFieldStatus();
	setDirty();
}

/*****************************************************
**
**   YogaEditorPanel   ---   propagateDirtyEvent
**
******************************************************/
void YogaEditorPanel::propagateDirtyEvent()
{
	if ( ! propagateUserInput ) return;
	setDirty();
}

/*****************************************************
**
**   YogaEditorPanel   ---   clearGui
**
******************************************************/
void YogaEditorPanel::clearGui()
{
	list_yogas->DeleteAllItems();

	text_description->SetValue( wxEmptyString );
	choice_group->Clear();
	choice_source->Clear();
	text_effect->SetValue( wxEmptyString );
	text_rule->SetValue( wxEmptyString );

	clearChart();
}

/*****************************************************
**
**   YogaEditorPanel   ---   clearChart
**
******************************************************/
void YogaEditorPanel::clearChart()
{
	drag_chart->clearChart();
	drag_chart->Refresh();
}

/*****************************************************
**
**   YogaEditorPanel   ---   getRuleText
**
******************************************************/
wxString YogaEditorPanel::getRuleText() const
{
	return text_rule->GetValue();
}

/*****************************************************
**
**   YogaEditorPanel   ---   getPlanetData
**
******************************************************/
std::map<PlanetId,Sign> YogaEditorPanel::getPlanetData() const
{
	assert( drag_chart );
	return drag_chart->getPlanetData();
}

/*****************************************************
**
**   YogaEditorPanel   ---   echoOnDragChart
**
******************************************************/
void YogaEditorPanel::echoOnDragChart( wxString s )
{
	drag_chart->echo( s );
}

