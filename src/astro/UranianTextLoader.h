/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/UranianTextLoader.h
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

#ifndef _URANIANTEXTLOADER_H_
#define _URANIANTEXTLOADER_H_

#include "bootstrap.h"
#include "constants.h"
#include "Planet.h"

#include <list>
#include <map>
#include <wx/string.h>

struct Match;
class MTerm;
struct PairTextItem;
struct TripleTextItem;

/*************************************************//**
*
* Loads text entries for Match events (if available)
*
******************************************************/
class UranianTextLoader : public Singleton<UranianTextLoader>
{
  friend class Singleton<UranianTextLoader>;

public:
	~UranianTextLoader();

	void cleanup();

	wxString getText4Match( const Match&, const bool witteMode );

	wxString getText4Pair( MTerm*, MTerm*, const bool witteMode );

	wxString getText4Term( const MTerm&, const bool witteMode );

	bool hasItemsForMode( const int &mode );

private:

	UranianTextLoader();

	wxString getTextfileName( const int &mode );

	PlanetId mapPlanetId( const PlanetId& ) const;

	TripleTextItem *getTripleTextItem( const PlanetId &p, const PlanetId &p1, const PlanetId &p2, const int &mode );

	PairTextItem *getPairTextItem( const PlanetId &p1, const PlanetId &p2, const int &mode );

	PlanetId getTextItem( wxString s );

	bool loadItems( const int &mode );

	const std::map<wxString, PlanetId>  wobjects;

	std::list<TripleTextItem*> triple_e, triple_w;

	std::list<PairTextItem*> pair_w;
};

#endif // _URANIANTEXTLOADER_H_

