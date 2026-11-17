/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/gdi/DocHelper.cpp
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

#include "DocHelper.h"

#include "logger.h"
#include "mathbase.h"
#include "Painter.h"

const MComplex DocHelper::default_extra_space = MComplex( .2, .2 );
const double DocHelper::sub_superscript_scalingfactor = .7;
const double DocHelper::sub_superscript_tokenshift = .2;

//#define DEBUG_DOCHELPER

/*****************************************************
**
**   DocHelper   ---   Constructor
**
******************************************************/
DocHelper::DocHelper( Painter *painter, const MComplex extra_space )
	: painter( painter ),
	extra_space( extra_space )
{
	assert( painter );
}

/*****************************************************
**
**   DocHelper   ---   preformatTextItem
**
******************************************************/
void DocHelper::preformatTextItem( DocItemText &item )
{
#ifdef DEBUG_DOCHELPER
	debuglog << "START DocHelper::preformatTextItem plain " << item.tf;
#endif
	item.resetFormat();
	item.spaceExtent = painter->getCharacterExtent();

	// 1st step: calculate token sizes
	for( const MToken &t : item.tf.tokens )
	{
		switch( t.tokenId )
		{
			case TokenId::Word:
			case TokenId::DegreeSymbol:
			case TokenId::MinuteSymbol:
			case TokenId::SecondSymbol:
				item.tokenSize.push_back( painter->getTextExtent( t.contents ));
			break;
			case TokenId::Symbol:
				painter->switchToSymbolFont( true );
				item.tokenSize.push_back( painter->getTextExtent( t.contents ));
				painter->switchToSymbolFont( false );
			break;
			case TokenId::Space:
			case TokenId::NoBreakSpace:
				item.tokenSize.push_back( item.spaceExtent );
			break;
			case TokenId::LineBreak:
				item.tokenSize.push_back( 0 );
			break;
			default: 
				fatallog << "Wrong token type " << (int)t.tokenId;
				assert( false );
			break;
		}
	}
	assert( item.tf.tokens.size() == item.tokenSize.size() );

	// 2nd step: determine words (i.e. unbreakable token clusters)
	TokenWord word;
	for( uint i = 0; i < item.tf.tokens.size(); i++ )
	{
		// end of a word: append TokenWord and create a new one
		if ( item.tf.tokens[i].tokenId == TokenId::Space
			|| item.tf.tokens[i].tokenId == TokenId::LineBreak )
		{
			if ( word.tokens.size() > 0 )
			{
				item.words.push_back( word );
				word.clear();
			}
			continue;
		}
		word.width += item.tokenSize[i].real();
		word.height = std::max( word.height, item.tokenSize[i].imag() );

#ifdef DEBUG_DOCHELPER
	debuglog << "DocHelper::preformatTextItem word w " << word.width << " h " << word.height;
#endif

		word.tokens.push_back( i );
	}
	// append word if anything left after leaving the loop
	if ( word.tokens.size() > 0 )
	{
		item.words.push_back( word );
	}
#ifdef DEBUG_DOCHELPER
	for( const auto &tz : item.tokenSize )
	{
		debuglog << "DocHelper::preformatTextItem token size: " << tz;
	}
	debuglog << "FINISHED DocHelper::preformatTextItem word count " << item.words.size();
#endif
}

/*****************************************************
**
**   DocHelper   ---   calculateLineBreaks
**
******************************************************/
void DocHelper::calculateLineBreaks( DocItemText &item, const double &xmax )
{
#ifdef DEBUG_DOCHELPER
	debuglog << "START DocHelper::calculateLineBreaks word count " << item.words.size()
		<< " item rect " << item.rect << " xmax " << xmax
		<< " contents " << item.tf;
#endif

	assert( item.tokenSize.size() == item.tf.tokens.size());
	item.lines.clear();
	item.width = 0;
	item.height = 0;
	const double x_extra = extra_space.real() * item.spaceExtent.real();
	//const double y_extra = extra_space.imag() * item.spaceExtent.imag();
	const double y_extra = 0;
	
	double xlinebreak = xmax - 2 * x_extra;
#ifdef DEBUG_DOCHELPER
	debuglog << "DocHelper::calculateLineBreaks x_extra " << x_extra << " y_extra " << y_extra << " xlinebreak " << xlinebreak;
#endif

	DocTextLine line;
	for( uint i = 0; i < item.words.size(); i++ )
	{
		const TokenWord &word = item.words[i];
#ifdef DEBUG_DOCHELPER
		debuglog << "DocHelper::calculateLineBreaks LOOP " << i
			<< " item rect " << item.rect
			<< " line rect " << line.rect
			<< " word width " << word.width
			<< " word height " << word.height
			<< " line word count " << line.words.size()
			<< " item.spaceExtent.real() " << item.spaceExtent.real()
			<< " xlinebreak " << xlinebreak;
#endif

		// break if line is not empty and would exceed the given width
		if ( ! line.isEmpty() && line.rect.width + item.spaceExtent.real() + word.width >= xlinebreak )
		{
#ifdef DEBUG_DOCHELPER
			debuglog << "DocHelper::calculateLineBreaks do break at word " << i;
#endif
			item.width = std::max( item.width, line.rect.width );
			item.height += line.rect.height + 2 * y_extra;

			item.lines.push_back( line );
			line.clear();
		}
		else if ( ! line.isEmpty()) // append space if line is not empty and does not break
		{
			line.rect.width += item.spaceExtent.real();
		}
		line.words.push_back( i );
		line.rect.width += word.width; // + item.spaceExtent.real();
		line.rect.height = std::max( line.rect.height, word.height );
	}
	// finally add last line if anything left
	if ( line.words.size() > 0 )
	{
		item.width = std::max( item.width, line.rect.width );
		item.height += line.rect.height + 2 * y_extra;
		item.lines.push_back( line );
		line.clear();
	}
	item.width += 2 * x_extra;

#ifdef DEBUG_DOCHELPER
	debuglog << "FINISHED DocHelper::calculateLineBreaks line count " << item.lines.size()
		<< " word count " << item.words.size() << " width " << item.width << " height " << item.height;
#endif
}

/*****************************************************
**
**   DocHelper   ---   calculateTextAlign
**
******************************************************/
void DocHelper::calculateTextAlign( DocItemText &item, const bool doFit )
{
	const double x_extra = extra_space.real() * item.spaceExtent.real();
	//const double y_extra = extra_space.imag() * item.spaceExtent.imag();
	const double y_extra = extra_space.imag();
	//const double y_extra = 0;

#ifdef DEBUG_DOCHELPER
	debuglog << "START DocHelper::calculateTextAlign item rect " << item.rect
		<< " item width " << item.width << " height " << item.height << " line count " << item.lines.size();
	debuglog << "DocHelper::calculateTextAlign x_extra " << x_extra << " y_extra " << y_extra 
		<< " extra_space " << extra_space << " item.spaceExtent " << item.spaceExtent;
#endif


	// vertical align: calculate starting point for y-value
	double ytop = item.rect.y;
	if ( item.align & Align::Top )
	{
		ytop += y_extra;
	}
	else if ( item.align & Align::Bottom )
	{
		ytop += item.rect.height - item.height - y_extra;
	}
	else if ( item.align & Align::VCenter )
	{
		//ytop += .5 * ( item.rect.height - item.height ) + y_extra;
		ytop += .5 * ( item.rect.height - item.height );
#ifdef DEBUG_DOCHELPER
		debuglog << "DocHelper::calculateTextAlign VCENTER ytop " << ytop;
#endif
	}
#ifdef DEBUG_DOCHELPER
	debuglog << "DocHelper::calculateTextAlign ytop " << ytop;
#endif

	for( DocTextLine &line : item.lines )
	{
		line.rect.y = ytop;
		ytop += line.rect.height;

		line.rect.x = item.rect.x;
		if ( item.align & Align::Left )
		{
			line.rect.x += x_extra;
		}
		else if ( item.align & Align::HCenter )
		{
			line.rect.x += .5 * ( item.rect.width - line.rect.width );
		}
		else if ( item.align & Align::Right )
		{
			line.rect.x += item.rect.width - line.rect.width - x_extra;
		}
	}
	if ( doFit )
	{
		if ( item.align & Align::Left )
		{
		}
		else if ( item.align & Align::HCenter )
		{
			item.rect.x += .5 * ( item.rect.width - item.width );
		}
		else if ( item.align & Align::Right )
		{
			item.rect.x += item.rect.width;
		}
		item.rect.y = ytop;
		item.rect.width = item.width;
		item.rect.height = item.height;
	}
#ifdef DEBUG_DOCHELPER
	debuglog << "FINISHED DocHelper::calculateTextAlign item rect " << item.rect;
#endif
}

/*****************************************************
**
**   DocHelper   ---   formatTextItem
**
******************************************************/
void DocHelper::formatTextItem( DocItemText &item, const double xmax )
{
	preformatTextItem( item );
	calculateLineBreaks( item, xmax >= 0 ? xmax : item.rect.width );
	calculateTextAlign( item );
}

/*****************************************************
**
**   DocHelper   ---   formatAndFitTextItem
**
******************************************************/
void DocHelper::formatAndFitTextItem( DocItemText &item, const double xmax )
{
	formatTextItem( item, xmax );
	if ( item.align & Align::HCenter )
	{
		item.rect.x += .5 * ( item.rect.width - item.width );
	}
	else if ( item.align & Align::Right )
	{
		item.rect.x += item.rect.width - item.width;
	}
	item.rect.width = item.width;

	if ( item.align & Align::Bottom )
	{
		item.rect.y += item.rect.height - item.height;
	}
	else if ( item.align & Align::VCenter )
	{
		item.rect.y += .5 * ( item.rect.height - item.height );
	}
	item.rect.height = item.height;
}

/*****************************************************
**
**   DocHelper   ---   formatAndDrawTextItem
**
******************************************************/
void DocHelper::formatAndDrawTextItem( const MString &ms, const MRect &rect, const Align align )
{
	DocItemText text( ms, rect, align );
	formatAndDrawTextItem( text );
}

/*****************************************************
**
**   DocHelper   ---   formatAndDrawTextItem
**
******************************************************/
void DocHelper::formatAndDrawTextItem( DocItemText &item )
{
#ifdef DEBUG_DOCHELPER
	debuglog << "DocHelper::formatAndDrawTextItem rect " << item.rect << " plain " << item.tf;
#endif
	formatTextItem( item, item.rect.width );
	drawTextItem( item );
}

/*****************************************************
**
**   DocHelper   ---   drawTextItem
**
******************************************************/
void DocHelper::drawTextItem( const DocItemText &item )
{
	double x0 = 0;
	double y0 = 0;

	/*
	* reference point for rawTextOut is either left upper or lower point of the rectangle
	*/
	const bool shiftCorner = painter->getTextAnchorPoint() != TextAnchorPoint::TopLeft;
	double tokenZoom = 1;
	double ypos = 0;
	double htoken = 0;

#if 0
	const wxColor oldcolor = painter->getPenColor();
	painter->setTransparentBrush();
	painter->setPenColor( *wxRED );
	painter->drawRectangle( item.rect );
	painter->drawLine( item.rect.x, item.rect.y, item.rect.x + item.rect.width, item.rect.y + item.rect.height );
	painter->drawLine( item.rect.x, item.rect.y + item.rect.height, item.rect.x + item.rect.width, item.rect.y );
	painter->setPenColor( oldcolor );
#endif

#ifdef DEBUG_DOCHELPER
	debuglog << "START DocHelper::drawTextItem rect " << item.rect
		<< " lines " << item.lines.size()
		<< " plain " << item.tf;
#endif
	for( const DocTextLine &line : item.lines )
	{
#ifdef DEBUG_DOCHELPER
		debuglog << "DocHelper::drawTextItem LINE rect " << line.rect;
#endif

		x0 = line.rect.x;
		y0 = line.rect.y;
		for( uint i = 0; i < line.words.size(); i++ )
		{
			const uint wordId = line.words[i];
			assert( wordId < item.words.size());
			for( uint j = 0; j < item.words[wordId].tokens.size(); j++ )
			{
				assert( j < item.tf.tokens.size());
				const uint id = item.words[wordId].tokens[j];
				assert( id < item.tf.tokens.size());
				const MToken& token = item.tf.tokens[id];
#ifdef DEBUG_DOCHELPER
				debuglog << "DocHelper::drawTextItem token id " << id << " coord " << x0 << "/" << y0
					<< " width " << item.tokenSize[id].real() << " height " << item.tokenSize[id].imag()
					<< " contents #" << token.contents << "# token ID " << token.tokenId;
#endif

				/*
				* find out token zoom and correct y position
				*/
				switch( token.fontFormat )
				{
					case TokenFontPosition::Subscriptum:
						tokenZoom = sub_superscript_scalingfactor;
						htoken = line.rect.height * tokenZoom;
						ypos = y0 + line.rect.height - ( 1 - sub_superscript_tokenshift ) * htoken + ( shiftCorner ? htoken : 0 );
					break;
					case TokenFontPosition::Superscriptum:
						tokenZoom = sub_superscript_scalingfactor;
						htoken = line.rect.height * tokenZoom;
						ypos = y0 - sub_superscript_tokenshift * htoken + ( shiftCorner ? htoken : 0 );
					break;
					case TokenFontPosition::Normal:
					default:
						tokenZoom = 1;
						htoken = line.rect.height * tokenZoom;
						ypos = y0 + ( shiftCorner ? htoken : 0 );
					break;
				}

#ifdef DEBUG_DOCHELPER
				debuglog << "DocHelper::drawTextItem " << token << " coordinates " << x0 << " - " << y0
					<< " tokenId " << token.tokenId << " font facename " << painter->getFont().GetFaceName();
#endif
				switch( token.tokenId )
				{
					case TokenId::Symbol:
						painter->switchToSymbolFont( true, tokenZoom );
						painter->rawTextOut( x0, ypos, token.contents );
						painter->switchToSymbolFont( false, tokenZoom );
					break;
					case TokenId::Word:
					case TokenId::DegreeSymbol:
					case TokenId::MinuteSymbol:
					case TokenId::SecondSymbol:
						//painter->switchToSymbolFont( false, tokenZoom );
						painter->rawTextOut( x0, ypos, token.contents );
					break;
					case TokenId::Space: // TODO should not happen
					case TokenId::NoBreakSpace:
						painter->setPen( *wxGREEN );
						painter->drawRectangle( MRect( x0, y0, item.tokenSize[id].real(), item.tokenSize[id].imag() ));
					break;
					default:
						fatallog << "wrong token ID " << (int) token.tokenId;
						assert( false );
					break;
				}
				x0 += item.tokenSize[id].real();
			}
			// end of word, add space
			x0 += item.spaceExtent.real();
		}
		y0 += line.rect.height;
	}
#ifdef DEBUG_DOCHELPER
	debuglog << "FINISHED DocHelper::drawTextItem";
#endif
}

/*****************************************************
**
**   DocHelper   ---   drawTextFormatted
**
******************************************************/
void DocHelper::drawTextFormatted( const wxString &t, const MRect &r, const Align& align )
{
	if ( t.empty() ) return;

#ifdef DEBUG_DOCHELPER
	debuglog << "DocHelper::drawTextFormatted x " << r.x << " y " << r.y
		<< " width " << r.width << " height " << r.height << " String " << t;
#endif

	MComplex p = painter->getTextExtent( t );
	double xmitte, ymitte, x, y;

	ymitte = 2 * r.y + r.height;
	ymitte /= 2;
	if ( align & Align::VCenter ) y = ymitte - p.imag() /2;
	else if ( align & Align::Top ) y = r.y;
	else y = r.y + r.height - p.imag();

	xmitte = 2 * r.x + r.width;
	xmitte /= 2;
	if ( align & Align::HCenter ) x = xmitte - p.real() /2;
	else if ( align & Align::Left ) x = r.x;
	else x = r.x + r.width - p.real();

#if 0
	painter->setPen( MColor( "RED" ));	
	painter->drawRectangle( x-2, y-2, 4, 4 );
#endif

	if ( painter->getTextAnchorPoint() == TextAnchorPoint::TopLeft ) // DcPainter
	{
		//y += p.imag();
	}
	else //BottomLeft : CairoPainter
	{
		y += p.imag();
	}
	painter->rawTextOut( x, y, t );
}


