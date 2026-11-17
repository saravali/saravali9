/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/BarDiagram.h
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

#ifndef _BARDIAGRAM_H_
#define _BARDIAGRAM_H_

#include "GdiWidget.h"

#include <list>
#include <wx/string.h>

struct BarDiagramConfig;

/*************************************************//**
*
* 
*
******************************************************/
class BarDiagramItem : public DocItem
{
public:

	BarDiagramItem( const std::variant<PlanetId, Sign> &o, const double &v, const int &configId )
		: DocItem( DocItemType::Bar ),
		object( o ),
		value( v ),
		configId( configId )
	{}

	const std::variant<PlanetId, Sign> object = PlanetId::None;

	double value = 0;

	const int configId = 0;
};

/*************************************************//**
*
*  Base class for bar diagram widgets.
*
******************************************************/
class BarDiagram :  public GdiWidget
{

public:
  BarDiagram(
		ContextProps_ptr,
		wxString title,
		const BarDiagramConfig *const = nullptr
	);

  ~BarDiagram();

	void clear();

	void addItem( const std::variant<PlanetId, Sign>&, const double value = 0.0 );

	virtual void doPaint( Painter *painter, const MRect &refreshRect ) override;

protected:

	void drawBar( Painter *painter, const BarDiagramItem& );

	wxString title;

	std::list<BarDiagramItem> items;

	double maxvalue, minvalue;

	double text_height;

	const int deltasteps = 4;
  const double xmargin = 5;
	const double xdmargin = 10;
	const double ymargin = 10;
	const double ydmargin = 10;

	const BarDiagramConfig *const barconfig;

	inline static constexpr int x3d = 5;

	inline static constexpr int y3d = 10;
};

#endif

