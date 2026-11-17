/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/TextWidget.cpp
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

#include "TextWidget.h"

#include "ContextProperties.h"
#include "Commands.h"
#include "enumserializer.h"
#include "Exporter.h"
#include "FontTool.h"
#include "gdiconfig.h"
#include "MDocument.h"

#include <wx/clipbrd.h>
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/html/htmlwin.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/string.h>

extern Gdiconfig *gdiconfig;

#define DEBUG_TEXTWIDGET

/*************************************************//**
*
*  workaround event handler
*
******************************************************/
class TextWidgetEventHandler: public wxEvtHandler
{
public:
	TextWidgetEventHandler( TextWidget *parent )
	 : parent( parent )
	{
		Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( TextWidgetEventHandler::OnContextMenuEvent ));
	  Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( TextWidgetEventHandler::OnMouseWheelEvent ));
	}

protected:
	void OnContextMenuEvent( wxMouseEvent &event )
	{
#ifdef DEBUG_TEXTWIDGET
		debuglog << "TextWidgetEventHandler::OnContextMenuEvent";
#endif
		parent->OnContextMenuEvent( event );
		//event.Skip();
	}

	void OnMouseWheelEvent( wxMouseEvent &event )
	{
		//debuglog << "TextWidgetEventHandler::OnMouseWheelEvent";
		const bool ctrlpressed = wxGetKeyState( WXK_CONTROL );
		if ( ctrlpressed )
		{
			if ( event.GetWheelRotation() > 0 )
			{
				wxMenuEvent evt( wxEVT_COMMAND_MENU_SELECTED, (int)CommandId::ZoomIn );
				wxPostEvent( parent, evt );
			}
			else
			{
				wxMenuEvent evt( wxEVT_COMMAND_MENU_SELECTED, (int)CommandId::ZoomOut );
				wxPostEvent( parent, evt );
			}
		}
		else event.Skip();
	}

	TextWidget *parent;
};

/*****************************************************
**
**   TextWidget   ---   Constructor
**
******************************************************/
TextWidget::TextWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &cat, const ToolbarWidgetId toolbarId )
 	: BasicWidget( parent, props, cat, toolbarId )
{
#ifdef DEBUG_TEXTWIDGET
	debuglog << "TextWidget::TextWidget category " << cat;
#endif

	html = new wxHtmlWindow( this, -1 );
	mainsizer->Add( html, 1, wxEXPAND, 3 );
	SetSizer( mainsizer );

	doc = MDocument_ptr( new MDocument );

	html->PushEventHandler( new TextWidgetEventHandler( this ));

	Connect( (int)CommandId::Copy, (int)CommandId::ExportTextClipboard, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler( TextWidget::OnTextWidgetCommand ));
}

/*****************************************************
**
**   TextWidget   ---   OnDataChanged
**
******************************************************/
void TextWidget::OnDataChanged()
{
	//setFonts
	int sizes[7];
	FontTool ft;
	ft.fillTextSizesArray( sizes, gdiconfig->fonts.textFontSize + props->getLogZoom() );
	html->SetFonts( gdiconfig->fonts.standardFontFacename,
		gdiconfig->fonts.fixedFontFacename, sizes );

	Exporter *exporter = ExporterFactory().getExporter( WidgetExportType::Html );
#ifdef DEBUG_TEXTWIDGET
	debuglog << "viewpoint is " << (int)getViewpoint();
#endif

	exporter->setTableStyle( tableStyle );

	html->SetPage( exporter->exportDocument( doc ));
	guiHasChanged();
}

/*****************************************************
**
**   TextWidget   ---   clearDoc
**
******************************************************/
void TextWidget::clearDoc()
{
	MDocument_ptr doc = getDoc();
	assert( doc );
	doc->clear();
}

/*****************************************************
**
**   TextWidget   ---   getEmptyDoc
**
******************************************************/
MDocument_ptr TextWidget::getEmptyDoc()
{
	MDocument_ptr doc = getDoc();
	assert( doc );
	doc->clear();
	return doc;
}

/*****************************************************
**
**   TextWidget   ---   OnTextWidgetCommand
**
******************************************************/
void TextWidget::OnTextWidgetCommand( wxCommandEvent &event )
{
	const CommandId command = (CommandId)event.GetId();
#ifdef DEBUG_TEXTWIDGET
	debuglog << "TextWidget::OnTextWidgetCommand  viewpoint " << getViewpoint() << " command " << command;
#endif

  switch ( command )
	{
		case CommandId::ExportPlainText:
			doTextToFileExport( WidgetExportType::Text );
		break;
		case CommandId::ExportCsv:
			doTextToFileExport( WidgetExportType::Csv );
		break;
		case CommandId::ExportHtml:
			doTextToFileExport( WidgetExportType::Html );
		break;
		case CommandId::ExportPlainHtml:
			doTextToFileExport( WidgetExportType::PlainHtml );
		break;

		case CommandId::ExportTextClipboard:
			doTextToClipboardExport();
		break;
		default:
			warnlog << "TextWidget::OnTextWidgetCommand invalid command " << command;
			//event.Skip();
		break;
	}
	//BasicWidget::OnCommand( event );
}

/*****************************************************
**
**   TextWidget   ---   doTextToClipboardExport
**
******************************************************/
void TextWidget::doTextToClipboardExport()
{
#ifdef DEBUG_TEXTWIDGET
		debuglog << "TextWidget::doTextToClipboardExport";
#endif
	if ( wxTheClipboard->Open() )
	{
		Exporter *exporter = ExporterFactory().getExporter( WidgetExportType::Html );
		const wxString s =  exporter->exportDocument( getDoc() );
#ifdef DEBUG_TEXTWIDGET
		debuglog << "TextWidget::doTextToClipboardExport: " << s;
#endif
		wxTheClipboard->SetData( new wxHTMLDataObject( s ));
		//wxTheClipboard->SetData( new wxTextDataObject( s ));
		wxTheClipboard->Close();
		delete exporter;

		doNotShowAgainMessage( this, DoNotShowAgainTopic::TextExportSuccessful, tr( "Text exported to clipboard" ), true );
	}
	else
	{
		errorlog << "Cannot open clipboard";
	}
}

/*****************************************************
**
**   TextWidget   ---   doTextToFileExport
**
******************************************************/
void TextWidget::doTextToFileExport( const WidgetExportType &type )
{
#ifdef DEBUG_TEXTWIDGET
	debuglog << "TextWidget::doTextExport type " << (int)type;
#endif
	wxString filename, filetypes, title;

	switch( type  )
	{
		case WidgetExportType::Text:
			filename = "out.txt";
			filetypes = "Text (*.txt)|*.txt|All files (*)|*.*";
			title = tr( "Export as Plain Text" );
		break;
		case WidgetExportType::Csv:
			filename = "out.csv";
			filetypes = "Text (*.csv)|*.csv|All files (*)|*.*";
			title = tr( "Export as Csv Text" );
		break;
		case WidgetExportType::Html:
			filename = "out.html";
			filetypes = "Html (*.html)|*.html|All files (*)|*.*";
			title = tr( "Export as HTML Text" );
		break;
		case WidgetExportType::PlainHtml:
			filename = "out.html";
			filetypes = "Html (*.html)|*.html|All files (*)|*.*" ;
			title = tr( "Export as plain HTML Text" );
		break;
		default:
			assert( false );
		break;
	}
	int style = wxFD_SAVE;
	if ( gdiconfig->view.exportAskOnOverwrite ) style |= wxFD_OVERWRITE_PROMPT;

	wxFileDialog exportFileDialog( nullptr, title, gdiconfig->prefs.defExportPath,
		filename, filetypes, style, wxDefaultPosition );
	if ( exportFileDialog.ShowModal() == wxID_OK )
	{
		filename = exportFileDialog.GetPath();
		gdiconfig->prefs.defExportPath = exportFileDialog.GetDirectory();

		Exporter *exporter = ExporterFactory().getExporter( type );

		wxString s =  exporter->exportDocument( getDoc() );
#ifdef DEBUG_TEXTWIDGET
		debuglog << "TextWidget::doTextExport text " << s;
#endif
		wxFile file;
		file.Create( filename, true );
		file.Write( s );
		file.Close();
		delete exporter;

		doNotShowAgainMessage( this, DoNotShowAgainTopic::TextExportSuccessful,
			std::format( "{} {}", tr("Text exported to"), filename ), true );
	}
}

