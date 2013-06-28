//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief FreeType フォントドライバ
//---------------------------------------------------------------------------
#ifndef _FREETYPE_H_
#define _FREETYPE_H_


#include "CharacterData.h"
#include "FreeTypeFace.h"
//#include "NativeFreeTypeDriver.h"

#include <ft2build.h>
#include FT_FREETYPE_H

//---------------------------------------------------------------------------
#define	TVP_GET_FACE_INDEX_FROM_OPTIONS(x) ((x) & 0xff) //!< オプション整数からFaceインデックスを取り出すマクロ
#define	TVP_FACE_OPTIONS_FACE_INDEX(x)		((x) & 0xff) //!< Faceインデックスをオプション整数に変換するマクロ
#define	TVP_FACE_OPTIONS_FILE				0x00010000 //!< フォント名ではなくてファイル名によるフォントの指定を行う
#define TVP_FACE_OPTIONS_NO_HINTING			0x00020000 //!< ヒンティングを行わない
#define TVP_FACE_OPTIONS_FORCE_AUTO_HINTING	0x00020000 //!< 強制的に auto hinting を行う
#define TVP_FACE_OPTIONS_NO_ANTIALIASING	0x00040000 //!< アンチエイリアスを行わない

//---------------------------------------------------------------------------
/**
 * FreeType フォント face
 */
class tFreeTypeFace
{
	std::wstring FontName;		//!< フォント名
	tBaseFreeTypeFace * Face; //!< Face オブジェクト
	FT_Face FTFace; //!< FreeType Face オブジェクト
	tjs_uint32 Options; //!< フラグ

	typedef std::vector<FT_ULong> tGlyphIndexToCharcodeVector;
	tGlyphIndexToCharcodeVector * GlyphIndexToCharcodeVector;		//!< グリフインデックスから文字コードへの変換マップ
	tjs_int Height;		//!< フォントサイズ(高さ) in pixel

	tjs_uint (*UnicodeToLocalChar)(tjs_char in); //!< SJISなどをUnicodeに変換する関数
	tjs_char (*LocalCharToUnicode)(tjs_uint in); //!< UnicodeをSJISなどに変換する関数

public:
	tFreeTypeFace(const std::wstring &fontname, tjs_uint32 options);
	~tFreeTypeFace();

	tjs_uint GetGlyphCount();
	tjs_char GetCharcodeFromGlyphIndex(tjs_uint index);

	void GetFaceNameList(std::vector<std::wstring> &dest);

	const std::wstring& GetFontName() const { return FontName; }

	tjs_int GetHeight() { return Height; }
	void SetHeight(int height);

	void SetOption( tjs_uint32 opt ) {
		Options |= opt;
	}
	void ClearOption( tjs_uint32 opt ) {
		Options &= ~opt;
	}
	bool GetOption( tjs_uint32 opt ) const {
		return (Options&opt) == opt;
	}

	tjs_int GetAscent() const {
		tjs_int ppem = FTFace->size->metrics.y_ppem;
		tjs_int upe = FTFace->units_per_EM;
		return FTFace->ascender * ppem / upe;
	}
	void GetUnderline( tjs_int& pos, tjs_int& thickness ) const {
		tjs_int ppem = FTFace->size->metrics.y_ppem;
		tjs_int upe = FTFace->units_per_EM;
		tjs_int liney = 0; //下線の位置
		tjs_int height = FTFace->size->metrics.height * ppem / upe;
		liney = GetAscent() + 1;
		thickness = FTFace->underline_thickness * ppem / upe;
		if( liney > height ) {
			liney = height - 1;
		}
		pos = liney;
	}
	void GetStrikeOut( tjs_int& pos, tjs_int& thickness ) const {
		tjs_int ppem = FTFace->size->metrics.y_ppem;
		tjs_int upe = FTFace->units_per_EM;
		thickness = FTFace->underline_thickness * ppem / upe;
		pos = FTFace->ascender * 2 * ppem / 5 / upe;
	}
	tTVPCharacterData * GetGlyphFromCharcode(tjs_char code);
	bool GetGlyphMetricsFromCharcode(tjs_char code, tGlyphMetrics & metrics);

private:
	bool LoadGlyphSlotFromCharcode(tjs_char code);
};
//---------------------------------------------------------------------------

#endif /*_FREETYPE_H_*/