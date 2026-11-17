/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/DocHelper.h
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

#ifndef _DOCHELPER_H_
#define _DOCHELPER_H_

#include "bootstrap.h"
#include "constants.h"

#include <wx/string.h>

struct MRect;
class MString;
class Painter;
class DocItemText;

/*************************************************//**
*
*  Formats and writes instances of DocTextItem.
*
*  Essential attributes of the DocText Item are
*  - rect: position and width/height of the rectangle
*  - width and height of the item
*  - align: location where the string will be printed within the rectangle
*  - contents: the MString itself (i.e. mixtures of plain string and symbols)
*
* Avaliable steps are
* 1. Preformat: calculate size of words (string contents already tokenized in MString)
* 2. calculate line breaks: depends on width
* 3. calculateTextAlign (position of the item in its rectangle)
* 4. draw with a given Painter.
*
* The additional "fit" feature corrects the size of the rectangle, so rect has the same width and height as the text.
* Left upper corner will be moved accordingn to the align of the DocTextItem.
*
* Available combination of steps are
* - formatTextItem: steps 1,2,3
* - formatAndFitTextItem: steps 1,2,3
* - formatAndDrawTextItem: all
*
* The font must be set correctly from outside, especially the
* settings should be consistent during format and paint.
*
* Extra spaces are available to extend the line with and height proportionally
* (always counted twice = left+right and top+bottom).
*
* Example: character extent (given by Painter) is 10x10 pt, Extra space is (0.5, 0.2)
* Result:
* width will be the proper text width PLUS 2 * 10 * .5 = 10pt (left and right)
* height will be the proper height (10) plus 2 * 10 * .2 = 10pt PER LINE
*
******************************************************/
class DocHelper
{
public:
	DocHelper( Painter*, const MComplex extra_space = default_extra_space );

	/**
	*  Formats a text item according to the given rectangle and align parameters.
	* 
	* The parameter xmax is the maximum width, i.e. the value causing a line break.
	* if xmax is not given or zero, the width of the items's rect will be used.
	*
	* Hint: xmax=0 and rect.width=0 will cause line break after each token
	*
	*/
	void formatTextItem( DocItemText&, const double xmax = 0 );

	/**
	*  Formats the text item and adjusts the rect (according to align parameter) to fit the text exactly.
	*  
	*/
	void formatAndFitTextItem( DocItemText&, const double xmax = 0 );

	/*
	*  Draw a formatted text item.
	*/
	void drawTextItem( const DocItemText& );

	/**
	*   Fast track: format text item and draw in one step.
	*/
	void formatAndDrawTextItem( DocItemText& );

	/**
	*   Fast track: format MString and draw in one step.
	*/
	void formatAndDrawTextItem( const MString&, const MRect&, const Align align = Align::West );

	/**
	*  Calculate the sizes of string tokens and identify word clusters.
	* 
	* The size of the text area is intentionally not included here.
	*
	* Preformat gets invalid if the underlying string or the painter's font change.
	*/
	void preformatTextItem( DocItemText& );

	/*
	*  Line break calculation after preformat is done.
	*
	*  The widht and heigt attributes of the text item are set properly. The rectangle is not touched.
	*/
	void calculateLineBreaks( DocItemText&, const double &xmax );

	/*
	*  Final formatting step: lines are adjusted according to the align parameter.
	*/
	void calculateTextAlign( DocItemText&, const bool doFit = false );

	/**
	*  Formatted text output.
	*/
	void drawTextFormatted( const wxString&, const MRect&, const Align& align );

protected:

	Painter *painter;

	// See above for the meaning
	const MComplex extra_space;

private:

	const static MComplex default_extra_space;

	const static double sub_superscript_scalingfactor;

	const static double sub_superscript_tokenshift;
};


#endif // _DOCHELPER_H_


