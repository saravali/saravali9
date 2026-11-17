/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/widgets/GenericTextWidget.cpp
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

#include "GenericTextWidget.h"

#include "astroconfig.h"
#include "ContextProperties.h"
#include "enumserializer.h"
#include "MDocument.h"
#include "TextHelper.h"
#include "WidgetFactory.h"

/*****************************************************
**
**   GenericTextWidget   ---   OnDataChanged
**
******************************************************/
GenericTextWidget::GenericTextWidget( wxWindow *parent, ContextProps_ptr props, const WidgetCategory &cat )
	: TextWidget( parent, props, cat )
{
	dasa = DasaId::Vimsottari;
	varga = Varga::Rasi;
	OnDataChanged();
}

/*****************************************************
**
**   GenericTextWidget   ---   OnDataChanged
**
******************************************************/
void GenericTextWidget::OnDataChanged()
{
	debuglog << "GenericTextWidget::OnDataChanged category " << category;
	MDocument_ptr doc = getDoc();
	assert( doc );

	doc->clear();
	TextHelper helper( props, doc );

	int ret = 0;
	switch( category )
	{
		case WidgetCategory::TextPlanetListCombined:
		{
			extern Astroconfig *astroconfig;
			ret = helper.writeTextAnalysis( WidgetCategory::TextPlanetListUniversal, varga, dasa );
			if ( astroconfig->getViewpointConfig( getViewpoint() ).basedata.showBasedataInPlanetListWidget )
			{
				ret += helper.writeTextAnalysis( WidgetCategory::TextBaseData, varga, dasa );
			}
		}
		break;
		default:
			ret = helper.writeTextAnalysis( category, varga, dasa );
		break;
	}

	static bool shown = false;
	if ( ret && ! shown )
	{
		shown = true;
		showMissingEphemDataMessage( this );
	}
	TextWidget::OnDataChanged();
}

/*****************************************************
**
**   WidgetFactory   ---   createGenericTextWidget
**
******************************************************/
BasicWidget *WidgetFactory::createGenericTextWidget( wxWindow *parent, const WidgetCategory &cat )
{
	return new GenericTextWidget( parent, props, cat );
}

