/***********************************************************************
 This is the source code of Saravali, a program for Vedic and western astrology.

 File       src/base/YdlLexer.ll
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

%option 8bit
%option yylineno
%option noyywrap
%option caseless
%option noinput
%option nounput

%{
#include "constants.h"
#include "mathbase.h"
#include "logger.h"
#include "Planet.h"
#include "YdlExpression.h"
#include "YdlParser.hh"

extern ExpressionErrorStatus parserErrorStatus;

#ifdef _MSC_VER
#include <io.h>


// Workaround
#define isatty _isatty
#define fileno _fileno
#endif

%}

DIGIT         [0-9]+
FLOAT         [0-9]+[.][0-9]+
IDENT         [A-Za-z]+[A-Za-z0-9_]*
VARGADIVISION D-[0-9]+
WS            [ 	]+
WSE            \t\n]*
%x C_COMMENT

%%

"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }

"//".*\n {}

"getLordOfHouse" return GETLORDOFHOUSE;
"getLordOfSign" return GETLORDOFSIGN;
"getRasi" return GETRASI;
"getBhava" return GETBHAVA;
"getNakshatra" return GETNAKSHATRA;

"isPlanetInRasi" return ISPLANETINRASI;
"isPlanetInKendra" return ISPLANETINKENDRA;
"isPlanetInApoklima" return ISPLANETINAPOKLIMA;
"isPlanetInPanaphara" return ISPLANETINPANAPHARA;
"isPlanetInDualRasi" return ISPLANETINDUALRASI;
"isPlanetInFixedRasi" return ISPLANETINFIXEDRASI;
"isPlanetInMovableRasi" return ISPLANETINMOVABLERASI;
"isPlanetInVargottama" return ISINVARGOTTAMA;
"isPlanetInExaltationSign" return ISPLANETINEXALTATIONSIGN;
"isPlanetInDebilationSign" return ISPLANETINDEBILATIONSIGN;
"isPlanetInMulatrikonaSign" return ISPLANETINMULATRIKONASIGN;

"isPlanetInHouse" return ISPLANETINHOUSE;
"isPlanetInSign" return ISPLANETINSIGN;
"isRelativeHouseOccupiedBy" return ISRELATIVEHOUSEOCCUPIEDBY;
"equalsOneOf" return EQUALSONEOF;

"getLongitude" return GETLONGITUDE;
"isRetrograde" return ISRETROGRADE;
"isInVargottama" return ISINVARGOTTAMA;
"isInMulatrikona" return ISINMULATRIKONA;
"getRasiInVarga" return GETRASIINVARGA;
"getHouseInVarga" return GETHOUSEINVARGA;

"isBenefic" return ISBENEFIC;
"isMalefic" return ISMALEFIC;
"isMutualKendra" return ISMUTUALKENDRA;
"isMutualTrikona" return ISMUTUALTRIKONA;
"isGrahaDrishti" return ISGRAHADRISHTI;
"isRasiDrishti" return ISRASIDRISHTI;

"countPlanetsInSign" return COUNTPLANETSINSIGN;
"countPlanetsInHouse" return COUNTPLANETSINHOUSE;
"countNumberOfOccupiedSigns" return NUMBER_OF_OCCUPIED_SIGNS;

"Sun" { yylval.ival = (int)PlanetId::Sun; return PLANET; }
"Moon" { yylval.ival = (int)PlanetId::Moon; return PLANET; }
"Mars" { yylval.ival = (int)PlanetId::Mars; return PLANET; }
"Mercury" { yylval.ival = (int)PlanetId::Mercury; return PLANET; }
"Jupiter" { yylval.ival = (int)PlanetId::Jupiter; return PLANET; }
"Venus" { yylval.ival = (int)PlanetId::Venus; return PLANET; }
"Saturn" { yylval.ival = (int)PlanetId::Saturn; return PLANET; }
"Uranus" { yylval.ival = (int)PlanetId::Uranus; return PLANET; }
"Neptune" { yylval.ival = (int)PlanetId::Neptune; return PLANET; }
"Pluto" { yylval.ival = (int)PlanetId::Pluto; return PLANET; }
"Rahu" { yylval.ival = (int)PlanetId::AscNode; return PLANET; }
"Ketu" { yylval.ival = (int)PlanetId::DescNode; return PLANET; }

"Moo" { yylval.ival = (int)PlanetId::Moon; return PLANET; }
"Mar" { yylval.ival = (int)PlanetId::Mars; return PLANET; }
"Mer" { yylval.ival = (int)PlanetId::Mercury; return PLANET; }
"Jup" { yylval.ival = (int)PlanetId::Jupiter; return PLANET; }
"Ven" { yylval.ival = (int)PlanetId::Venus; return PLANET; }
"Sat" { yylval.ival = (int)PlanetId::Saturn; return PLANET; }
"Ura" { yylval.ival = (int)PlanetId::Uranus; return PLANET; }
"Nep" { yylval.ival = (int)PlanetId::Neptune; return PLANET; }
"Plu" { yylval.ival = (int)PlanetId::Pluto; return PLANET; }
"Rah" { yylval.ival = (int)PlanetId::AscNode; return PLANET; }
"Ket" { yylval.ival = (int)PlanetId::DescNode; return PLANET; }

"Dhuma" { yylval.ival = (int)(int)PlanetId::Dhuma; return PLANET; }
"Vyatipata" { yylval.ival = (int)PlanetId::Vyatipata; return PLANET; }
"Parivesha" { yylval.ival = (int)PlanetId::Parivesha; return PLANET; }
"Chapa" { yylval.ival = (int)PlanetId::Chapa; return PLANET; }
"Upaketu" { yylval.ival = (int)PlanetId::Upaketu; return PLANET; }
"Kala" { yylval.ival = (int)PlanetId::Kala; return PLANET; }
"Mrityu" { yylval.ival = (int)PlanetId::Mrityu; return PLANET; }
"Ardhaprahara" { yylval.ival = (int)PlanetId::Ardhaprahara; return PLANET; }
"Yamaghantaka" { yylval.ival = (int)PlanetId::Yamaghantaka; return PLANET; }
"Gulika" { yylval.ival = (int)PlanetId::Gulika; return PLANET; }
"Mandi" { yylval.ival = (int)PlanetId::Mandi; return PLANET; }

"Aries" { yylval.ival = (int)Sign::Aries; return SIGN; }
"Taurus" { yylval.ival = (int)Sign::Taurus; return SIGN; }
"Gemini" { yylval.ival = (int)Sign::Gemini; return SIGN; }
"Cancer" { yylval.ival = (int)Sign::Cancer; return SIGN; }
"Leo" { yylval.ival = (int)Sign::Leo; return SIGN; }
"Virgo" { yylval.ival = (int)Sign::Virgo; return SIGN; }
"Libra" { yylval.ival = (int)Sign::Libra; return SIGN; }
"Scorpio" { yylval.ival = (int)Sign::Scorpio; return SIGN; }
"Sagittarius" { yylval.ival = (int)Sign::Sagittarius; return SIGN; }
"Capricorn" { yylval.ival = (int)Sign::Capricorn; return SIGN; }
"Aquarius" { yylval.ival = (int)Sign::Aquarius; return SIGN; }
"Pisces" { yylval.ival = (int)Sign::Pisces; return SIGN; }

"Ari" { yylval.ival = (int)Sign::Aries; return SIGN; }
"Tau" { yylval.ival = (int)Sign::Taurus; return SIGN; }
"Gem" { yylval.ival = (int)Sign::Gemini; return SIGN; }
"Can" { yylval.ival = (int)Sign::Cancer; return SIGN; }
"Vir" { yylval.ival = (int)Sign::Virgo; return SIGN; }
"Lib" { yylval.ival = (int)Sign::Libra; return SIGN; }
"Sco" { yylval.ival = (int)Sign::Scorpio; return SIGN; }
"Sag" { yylval.ival = (int)Sign::Sagittarius; return SIGN; }
"Cap" { yylval.ival = (int)Sign::Capricorn; return SIGN; }
"Aqu" { yylval.ival = (int)Sign::Aquarius; return SIGN; }
"Pis" { yylval.ival = (int)Sign::Pisces; return SIGN; }

"House1" { yylval.ival = (int)House::h1; return HOUSE; }
"House2" { yylval.ival = (int)House::h2; return HOUSE; }
"House3" { yylval.ival = (int)House::h3; return HOUSE; }
"House4" { yylval.ival = (int)House::h4; return HOUSE; }
"House5" { yylval.ival = (int)House::h5; return HOUSE; }
"House6" { yylval.ival = (int)House::h6; return HOUSE; }
"House7" { yylval.ival = (int)House::h7; return HOUSE; }
"House8" { yylval.ival = (int)House::h8; return HOUSE; }
"House9" { yylval.ival = (int)House::h9; return HOUSE; }
"House10" { yylval.ival = (int)House::h10; return HOUSE; }
"House11" { yylval.ival = (int)House::h11; return HOUSE; }
"House12" { yylval.ival = (int)House::h12; return HOUSE; }

"H1" { yylval.ival = (int)House::h1; return HOUSE; }
"H2" { yylval.ival = (int)House::h2; return HOUSE; }
"H3" { yylval.ival = (int)House::h3; return HOUSE; }
"H4" { yylval.ival = (int)House::h4; return HOUSE; }
"H5" { yylval.ival = (int)House::h5; return HOUSE; }
"H6" { yylval.ival = (int)House::h6; return HOUSE; }
"H7" { yylval.ival = (int)House::h7; return HOUSE; }
"H8" { yylval.ival = (int)House::h8; return HOUSE; }
"H9" { yylval.ival = (int)House::h9; return HOUSE; }
"H10" { yylval.ival = (int)House::h10; return HOUSE; }
"H11" { yylval.ival = (int)House::h11; return HOUSE; }
"H12" { yylval.ival = (int)House::h12; return HOUSE; }

"Lagna" { yylval.ival = (int)PlanetId::Ascendant; return PLANET; }
"Ascendant" { yylval.ival = (int)PlanetId::Ascendant; return PLANET; }
"Asc" { yylval.ival = (int)PlanetId::Ascendant; return PLANET; }
"MC" { yylval.ival = (int)PlanetId::Meridian; return PLANET; }
"MediumCoeli" { yylval.ival = (int)PlanetId::Meridian; return PLANET; }
"Descendant" { yylval.ival = (int)PlanetId::Descendant; return PLANET; }
"IC" { yylval.ival = (int)PlanetId::ImumCoeli; return PLANET; }
"ImumCoeli" { yylval.ival = (int)PlanetId::ImumCoeli; return PLANET; }

"Aswini" { yylval.ival = (int)Nakshatra27::Aswini; return NAKSHATRA; }
"Bharani" { yylval.ival = (int)Nakshatra27::Bharani; return NAKSHATRA; }
"Krittika" { yylval.ival = (int)Nakshatra27::Krittika; return NAKSHATRA; }
"Rohini" { yylval.ival = (int)Nakshatra27::Rohini; return NAKSHATRA; }
"Mrigasira" { yylval.ival = (int)Nakshatra27::Mrigasira; return NAKSHATRA; }
"Arudra" { yylval.ival = (int)Nakshatra27::Ardra; return NAKSHATRA; }
"Punarvasu" { yylval.ival = (int)Nakshatra27::Punarvasu; return NAKSHATRA; }
"Pushyami" { yylval.ival = (int)Nakshatra27::Pushyami; return NAKSHATRA; }
"Aslesha" { yylval.ival = (int)Nakshatra27::Aslesha; return NAKSHATRA; }
"Makha" { yylval.ival = (int)Nakshatra27::Magha; return NAKSHATRA; }
"PurvaPhalguni" { yylval.ival = (int)Nakshatra27::Poorvaphalguni; return NAKSHATRA; }
"UttaraPhalguni" { yylval.ival = (int)Nakshatra27::Uttaraphalguni; return NAKSHATRA; }
"Hashta" { yylval.ival = (int)Nakshatra27::Hasta; return NAKSHATRA; }
"Chitta" { yylval.ival = (int)Nakshatra27::Chitra; return NAKSHATRA; }
"Swati" { yylval.ival = (int)Nakshatra27::Swati; return NAKSHATRA; }
"Vishakha" { yylval.ival = (int)Nakshatra27::Visakha; return NAKSHATRA; }
"Anuradha" { yylval.ival = (int)Nakshatra27::Anuradha; return NAKSHATRA; }
"Jeshta" { yylval.ival = (int)Nakshatra27::Jyeshta; return NAKSHATRA; }
"Moola" { yylval.ival = (int)Nakshatra27::Moola; return NAKSHATRA; }
"PurvaShadha" { yylval.ival = (int)Nakshatra27::Poorvashadha; return NAKSHATRA; }
"UttaraShadha" { yylval.ival = (int)Nakshatra27::Uttarashadha; return NAKSHATRA; }
"Sravana" { yylval.ival = (int)Nakshatra27::Sravana; return NAKSHATRA; }
"Dhanishta" { yylval.ival = (int)Nakshatra27::Dhanista; return NAKSHATRA; }
"Satabishak" { yylval.ival = (int)Nakshatra27::Satabhisha; return NAKSHATRA; }
"PurvaBhadra" { yylval.ival = (int)Nakshatra27::Poorvabhadra; return NAKSHATRA; }
"UttaraBhadra" { yylval.ival = (int)Nakshatra27::Uttarabhadra; return NAKSHATRA; }
"Revati" { yylval.ival = (int)Nakshatra27::Revati; return NAKSHATRA; }

"Rasi" { yylval.ival = (int)Varga::Rasi; return VARGA; }
"Navamsa" { yylval.ival = (int)Varga::Navamsa; return VARGA; }
"Hora" { yylval.ival = (int)Varga::Hora; return VARGA; }
"Drekkana" { yylval.ival = (int)Varga::Drekkana; return VARGA; }
"Chaturtamsa" { yylval.ival = (int)Varga::Chaturtamsa; return VARGA; }
"Sastamsa" { yylval.ival = (int)Varga::Sastamsa; return VARGA; }
"Saptamamsa" { yylval.ival = (int)Varga::Saptamamsa; return VARGA; }
"Ashtamsa" { yylval.ival = (int)Varga::Ashtamsa; return VARGA; }
"Dasamsa" { yylval.ival = (int)Varga::Dasamsa; return VARGA; }
"Dvadasamsa" { yylval.ival = (int)Varga::Dvadasamsa; return VARGA; }
"Shodasamsa" { yylval.ival = (int)Varga::Shodasamsa; return VARGA; }
"Vimsamsa" { yylval.ival = (int)Varga::Vimsamsa; return VARGA; }
"Siddhamsa" { yylval.ival = (int)Varga::Siddhamsa; return VARGA; }
"Bhamsa" { yylval.ival = (int)Varga::Bhamsa; return VARGA; }
"Trimsamsa" { yylval.ival = (int)Varga::Trimsamsa; return VARGA; }
"Chatvarimsamsa" { yylval.ival = (int)Varga::Chatvarimsamsa; return VARGA; }
"Akshavedamsa" { yylval.ival = (int)Varga::Akshavedamsa; return VARGA; }
"Shastiamsa" { yylval.ival = (int)Varga::Shastiamsa; return VARGA; }
"Ashtottaramsa" { yylval.ival = (int)Varga::Ashtottaramsa; return VARGA; }
"DvadasDvadasamsa" { yylval.ival = (int)Varga::DvadasDvadasamsa; return VARGA; }
"Bhava" { yylval.ival = (int)Varga::Bhava; return VARGA; }

"true" { yylval.ival = 1; return BOOLEAN; }
"yes" { yylval.ival = 1; return BOOLEAN; }
"false" { yylval.ival = 0; return BOOLEAN; }
"no" { yylval.ival = 0; return BOOLEAN; }

"and" return AND;
"or" return OR;
"not" return NOT;

"int" return TYPE_INT;
"double" return TYPE_DOUBLE;
"Planet" return TYPE_PLANET;
"Sign" return TYPE_SIGN;
"Nakshatra" return TYPE_NAKSHATRA;
"House" return TYPE_HOUSE;
"Varga" return TYPE_VARGA;
"Boolean" return TYPE_BOOLEAN;
"Bool" return TYPE_BOOLEAN;

"(" return LPAR;
")" return RPAR;

"return" return RETURN;
";" return COLON;
"," return COMMA;

"==" return ISEQUAL;
"=" { printf( "EQUAL\n"); return EQUAL; }
"!=" return ISNOTEQUAL;

"+" return PLUS;
"-" return MINUS;
"*" return MULT;
"/" return DIV;
"%" return MOD;

"<" return LT;
"<=" return LE;
">" return GT;
">=" return GE;

"red12" return RED12;
"red27" return RED27;

{DIGIT} {
		yylval.ival = atoi( yytext );
		return DIGIT;
	}

{FLOAT} {
		// wrong on decimal point confusion (",", "." )
		//yylval.dval = atof( yytext );
		//double a = 0.0;
		stringToDouble( yytext, yylval.dval );
		//printf( "FLOAT %s -- %f\n", yytext, yylval.dval );
		return FLOAT;
	}

{VARGADIVISION} {
	/* Varga by division number , e.g. D-12 = Dvadasamsa */
	int d = -1;
	if ( sscanf( yytext, "D-%d", &d ) == 1 )
	{
		for( int i = 0; i < (int)Varga::Last; i++ )
		{
			if ( vargaDivisions[i] == d )
			{
				yylval.ival = i;
				return VARGA;
			}
		}
	}
	errorlog << "Lexical error at line " << yylineno << ": unsupported Varga division " << yytext;
	parserErrorStatus = ExpressionErrorStatus::LexicalError;
}

\"[^"]*["] { yylval.sval = strdup( yytext+1 ); }

{IDENT} {
	yylval.sval = strdup( yytext );
	return VARIABLE;
}

"#"[^\n]*"\n" {}

"\n"      {}

{WS}     {}


. {
		errorlog << "Lexical error at line " << yylineno << " near token \"" << yytext << "\"";
		parserErrorStatus = ExpressionErrorStatus::LexicalError;
	}

%%

