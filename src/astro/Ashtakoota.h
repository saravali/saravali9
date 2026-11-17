/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/astro/Ashtakoota.h
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

#ifndef _ASHTAKOOTA_H_
#define _ASHTAKOOTA_H_

#include "Expert.h"
#include "Nakshatra.h"

/*************************************************//**
*
* 
*
******************************************************/
class AshtakootaExpert : public NakshatraExpert
{
public:

	AshtakootaExpert( ContextProps_ptr props ) : NakshatraExpert( props ) {}

	static int calcYoniValue( const Yoni&, const Yoni& );
	static int calcRajjuValue( const Rajju&, const Rajju& );

	void update();
	void write( MDocument_ptr );
	void writeComprehensive( MDocument_ptr );

	double getPoints() { return ptotal; }

private:
	void init();

	int pvarna, pyoni, pmaitri, prasi, pgana, pnadi, prajju;
	double pvashya, ptara, ptotal;
	Sign rasi1, rasi2;
	Nakshatra27 nak1, nak2;
	PlanetaryFriendship mitra1, mitra2;
	Yoni yoni1, yoni2;
	Varna varna1, varna2;
	Vashya vashya1, vashya2;
	Gana gana1, gana2;
	int tara1, tara2;
	Nadi nadi1, nadi2;
	Rajju rajju1, rajju2;
};

#endif // _ASHTAKOOTA_H_

