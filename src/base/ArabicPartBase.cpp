/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/ArabicPartBase.cpp
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

#include "ArabicPartBase.h"
#include "Lang.h"

/*****************************************************
**
**   ArabicPartsConfig   ---   getPlanetName
**
******************************************************/
wxString ArabicPartsConfig::getPlanetName( const PlanetId &p, const TextFormat &format )
{
	assert( p >= PlanetId::FirstArabic && p <= PlanetId::LastArabic );

	const int index = (int)p - (int)PlanetId::FirstArabic;
  switch ( format )
  {
  case TextFormat::Short:
    return getPart( index ).name.substr( 0, 2 );
    break;
  case TextFormat::Medium:
    return getPart( index ).name.substr( 0, 4 );
    break;
	default:
		return getPart( index ).name;
    break;
  }
}

/*****************************************************
**
**   ArabicPartsConfig   ---   getPart
**
******************************************************/
ArabicPartDef ArabicPartsConfig::getPart( const int &i )
{
	const static ArabicPartDef parts2[1] =
	{
		{ tr( "Fortune"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Moon}, true }
	};

	const PlanetId fortuneId = PlanetId::FirstArabic;
	const PlanetId spritId = (PlanetId)((int)PlanetId::FirstArabic + 1 );
	const static ArabicPartDef parts[MAX_ARABIC_PARTS] =
	{
		// from Astrolog
		// start of generated code
		{ tr( "Fortune"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Spirit"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Victory"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, spritId }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Valor and Bravery"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LArabic, fortuneId}, true },
		{ tr( "Mind and Administrators"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Property and Goods"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LLord, House::h2 }, { Arabic::LHousecusp, House::h2}, true },
		{ tr( "Siblings"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Death of Siblings"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LHousecusp10, House::h3}, true },
		{ tr( "Death of Parents"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Grandparents"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LHousecusp, House::h2 }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Real Estate"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Children and Life"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Expected Birth"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanetHousecuspLord, PlanetId::Moon}, false },
		{ tr( "Disease and Defects (1)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Disease and Defects (2)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Captivity"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanetHousecuspLord, PlanetId::Saturn}, false },
		{ tr( "Servants"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Moon}, false },
		{ tr( "Partners"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LHousecusp, House::h7 }, false },
		{ tr( "Death"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LHousecusp, House::h8 }, false },
		{ tr( "Sickness and Murder"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Danger, Violence and Debt"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mercury}, true },
		{ tr( "Journeys"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LLord, House::h9 }, { Arabic::LHousecusp, House::h9}, false },
		{ tr( "Travel by Water"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LFixed, 105}, true },
		{ tr( "Faith, Trust and Belief"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Mercury}, true },
		{ tr( "Deep Reflection"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Understanding and Wisdom"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Fame and Recognition"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Rulers and Disassociation"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Father, Fate and Karma"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Sudden Advancement"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LArabic, fortuneId}, true },
		{ tr( "Celebrity of Rank"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Sun}, false },
		{ tr( "Surgery and Accident"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Merchants and Their Work"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Merchandise (Exchange)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, spritId }, { Arabic::LArabic, fortuneId}, true },
		{ tr( "Mother"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Glory and Constancy"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, fortuneId }, { Arabic::LArabic, spritId}, true },
		{ tr( "Honorable Acquaintances"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, fortuneId }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Success"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, fortuneId }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Worldliness"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, fortuneId }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Acquaintances"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Violence"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, spritId }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Liberty of Person"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Praise and Acceptance"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Enmity"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LLord, House::h12 }, { Arabic::LHousecusp, House::h12}, false },
		{ tr( "Bad Luck"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, spritId }, { Arabic::LArabic, fortuneId}, false },
		{ tr( "Debilitated Bodies"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, fortuneId }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Boldness and Violence"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LDispositor, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Trickery and Deceit"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LArabic, spritId}, true },
		{ tr( "Necessities"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LHousecusp, House::h3}, false },
		{ tr( "Realization of Needs"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LArabic, fortuneId }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Retribution"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Children (Male)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Children (Female)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Play and Variety"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Stability"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Speculation"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LHousecusp, House::h5}, true },
		{ tr( "Art"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Mercury}, true },
		{ tr( "Sexual Attraction"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LLord, House::h5 }, { Arabic::LHousecusp, House::h5}, false },
		{ tr( "Sex Drive and Stimulation"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LFixed, 10}, false },
		{ tr( "Passion"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Emotion and Affection"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Most Perilous Year"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LLord, House::h8}, false },
		{ tr( "Peril"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LHousecusp, House::h8}, false },
		{ tr( "Occultism"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Commerce"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Marriage Contracts"  ), { Arabic::LHousecusp, House::h9 }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LHousecusp, House::h3}, false },
		{ tr( "Travel by Land"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LLord, House::h9 }, { Arabic::LHousecusp, House::h9}, false },
		{ tr( "Travel by Air"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanetHousecusp, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Destiny"  ), { Arabic::LPlanet, PlanetId::Meridian }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Vocation and Status"  ), { Arabic::LPlanet, PlanetId::Meridian }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Honor, Nobility (Day)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LFixed, 19}, false },
		{ tr( "Honor, Nobility (Night)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LFixed, 33}, false },
		{ tr( "Organization"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LFixed, 10}, false },
		{ tr( "Divorce"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LHousecusp, House::h7 }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Ostracism and Loss"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Friends"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Tragedy and Brethren"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Race (Consciousness)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LFixed, 10 }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Bondage and Slavery"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LDispositor, House::h2 }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Imprisonment and Sorrow"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LArabic, fortuneId}, false },
		{ tr( "Perversion"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Self-Undoing"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LHousecusp, House::h12}, false },
		{ tr( "Treachery and Entrapment"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Bereavement"  ), { Arabic::LHousecusp, House::h12 }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LLord, House::h12}, false },
		{ tr( "Suicide (Yang)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LHousecusp, House::h12 }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Suicide (Yin)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LHousecusp, House::h8}, false },
		{ tr( "Depression"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Assassination (Yang)"  ), { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Assassination (Yin)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LLord, House::h12}, false },
		{ tr( "Cancer (Disease)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Catastrophe"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Foolhardiness"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Release and Luck"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Benevolence and Assurance"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Hope and Sensitivity"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Aptness and Aloofness"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Charm and Personality"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Faith and Good Manners"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Temperament"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Sun}, false },
		{ tr( "Security and Treasure"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Originality"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Eccentricity, Astrology"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Divination"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Intrusion"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Negotiation"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Discord and Controversy"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Coincidence"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Unpreparedness"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Popularity"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Misunderstanding"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Sentiment and Marriage"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Loneliness"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Success in Investment"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Frugality and Labor"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Wastefulness"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Vanity"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Corruptness"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Initiative"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Sun}, false },
		{ tr( "Memory"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Love, Beauty and Peace"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Disinterest and Boredom"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Sun}, false },
		{ tr( "Accomplishment"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Sun}, false },
		{ tr( "Influence"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Increase and Impression"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Caution"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Neptune}, false },
		{ tr( "Timidity"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Entertainment"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Uranus}, false },
		{ tr( "Bequest"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Genius"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Sun}, false },
		{ tr( "Revelation"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Neptune }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Delusion"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Neptune}, true },
		{ tr( "Misinterpretation"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Uranus}, true },
		{ tr( "Intellectuality"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Uranus }, { Arabic::LPlanet, PlanetId::Sun}, false },
		{ tr( "Earth"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Water"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Venus}, false },
		{ tr( "Air and Wind"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LLord, House::h4}, false },
		{ tr( "Fire"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Clouds"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Rains"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Cold"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Wheat"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Barley and Meats"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Rice and Millet"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Corn"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Lentils, Iron, Pungents"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Beans and Onions"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Chick Peas"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Sesame and Grapes"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Sugar and Legumes"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LPlanet, PlanetId::Mercury}, true },
		{ tr( "Honey"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Sun}, true },
		{ tr( "Oils"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Nuts and Flax"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Olives"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Apricots and Peaches"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Melons"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Mercury}, true },
		{ tr( "Salt"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Sweets"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Mercury}, true },
		{ tr( "Astrigents and Purgatives"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Saturn}, true },
		{ tr( "Silk and Cotton"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Venus}, true },
		{ tr( "Purgatives (Bitter)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mars}, true },
		{ tr( "Purgatives (Acid)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Jupiter}, true },
		{ tr( "Secrets"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LDispositor, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Meridian}, false },
		{ tr( "Information True/False"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Moon}, true },
		{ tr( "Injury to Business"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LDispositor, PlanetId::Ascendant }, { Arabic::LArabic, fortuneId}, true },
		{ tr( "Freedmen and Servants"  ), { Arabic::LPlanet, PlanetId::Mercury }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Employers"  ), { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Jupiter }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Marriage"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Venus }, { Arabic::LHousecusp, House::h7}, false },
		{ tr( "Time for Action/Success"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Time Occupied in Action"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Dismissal and Resignation"  ), { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Jupiter}, false },
		{ tr( "Life/Death of Absentee"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Lost Animal (Light)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Sun }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Lost Animal (Dark)"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Saturn }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Lawsuit"  ), { Arabic::LPlanet, PlanetId::Ascendant }, { Arabic::LPlanet, PlanetId::Mars }, { Arabic::LPlanet, PlanetId::Mercury}, false },
		{ tr( "Decapitation"  ), { Arabic::LHousecusp, House::h8 }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Mars}, false },
		{ tr( "Torture"  ), { Arabic::LPlanet, PlanetId::Meridian }, { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LPlanet, PlanetId::Saturn}, false },
		{ tr( "Lost Objects"  ), { Arabic::LPlanet, PlanetId::Moon }, { Arabic::LDispositor, PlanetId::Ascendant }, { Arabic::LHousecusp, House::h4}, false }
		// end of generated code
	};
	return parts[i];
}


