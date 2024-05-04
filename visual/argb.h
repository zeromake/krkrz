//---------------------------------------------------------------------------
/*
	TVP2 ( T Visual Presenter 2 )  A script authoring tool
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
// Structures for manipulating ARGB expression of a pixel
//---------------------------------------------------------------------------
#ifndef ARGBH
#define ARGBH

#include "tvpgl.h"
//---------------------------------------------------------------------------
#if defined(_WIN32) || defined(__GNUC__)
	// for assembler compatibility
	#pragma pack(push,1)
#endif
//---------------------------------------------------------------------------
template <typename base_type>
struct tTVPARGB
{
#if TJS_HOST_IS_LITTLE_ENDIAN
#endif
#if 1
	base_type b;
	base_type g;
	base_type r;
	base_type a;
#endif
#if 0
#if TJS_HOST_IS_BIG_ENDIAN
	base_type a;
	base_type r;
	base_type g;
	base_type b;
#endif
#endif

	typedef base_type base_int_type;


	tTVPARGB() {;}

//	tTVPARGB(const tTVPARGB & rhs)
//		{ this->operator =(rhs); }

	void Zero()
	{
		b = g = r = a = 0;
	}

	void operator += (const tTVPARGB & rhs)
		{ b += rhs.b; g += rhs.g; r += rhs.r; a += rhs.a; }

	void operator -= (const tTVPARGB & rhs)
		{ b -= rhs.b; g -= rhs.g; r -= rhs.r; a -= rhs.a; }

	void operator += (tjs_uint32 v)
	{
		b += v & 0xff;
		g += (v>>8) & 0xff;
		r += (v>>16) & 0xff;
		a += (v>>24);
	}

	void operator -= (tjs_uint32 v)
	{
		b -= v & 0xff;
		g -= (v>>8) & 0xff;
		r -= (v>>16) & 0xff;
		a -= (v>>24);
	}

//	void operator = (const tTVPARGB & rhs)
//		{ *this = rhs; }

	void average(tjs_int n)
	{
		tjs_int half_n = n >> 1;

		b /= n;
		g /= n;
		r /= n;
		a /= n;
	}

	void operator = (tjs_uint32 v)
	{
		a = v >> 24,
		r = (v >> 16) & 0xff,
		g = (v >> 8) & 0xff,
		b = v & 0xff;
	}

	operator tjs_uint32() const
	{
		return b + (g << 8) + (r << 16) + (a << 24);
	}

};
//---------------------------------------------------------------------------
// special member functions for tjs_uint8
template <>
inline void tTVPARGB<tjs_uint8>::Zero()
{
	*(tjs_uint32 *)this = 0;
}

#if TJS_HOST_IS_LITTLE_ENDIAN
template <>
inline void tTVPARGB<tjs_uint8>::operator = (tjs_uint32 v)
{
	*(tjs_uint32 *)this = v;
}

template <>
inline tTVPARGB<tjs_uint8>::operator tjs_uint32() const
{
	return *(const tjs_uint32 *)this;
}
#endif

template <>
inline void tTVPARGB<tjs_uint8>::average(tjs_int n)
{
	tjs_int half_n = n >> 1;

	tjs_int recip = (1L<<23) / n;

	b = (b+half_n)*recip >> 23;
	g = (g+half_n)*recip >> 23;
	r = (r+half_n)*recip >> 23;
	a = (a+half_n)*recip >> 23;
}

//---------------------------------------------------------------------------
// special member functions for tjs_uint16
template <>
inline void tTVPARGB<tjs_uint16>::average(tjs_int n)
{
	tjs_int half_n = n >> 1;

	tjs_int recip = (1L<<16) / n;

	b = (b+half_n)*recip >> 16;
	g = (g+half_n)*recip >> 16;
	r = (r+half_n)*recip >> 16;
	a = (a+half_n)*recip >> 16;
}

//---------------------------------------------------------------------------
// special member functions for tjs_uint32
template <>
inline void tTVPARGB<tjs_uint32>::average(tjs_int n)
{
	tjs_int half_n = n >> 1;

	b = (b+half_n)/n;
	g = (g+half_n)/n;
	r = (r+half_n)/n;
	a = (a+half_n)/n;
}


//---------------------------------------------------------------------------
// a structure delivered from tTVPARGB, using Additive-Alpha expression
// for tjs_uint32 (packed ARGB) input/output.
template <typename base_type>
struct tTVPARGB_AA : public tTVPARGB<base_type>
{
	void operator += (const tTVPARGB_AA & rhs)
		{ this->b += rhs.b; this->g += rhs.g; this->r += rhs.r; this->a += rhs.a; }

	void operator -= (const tTVPARGB_AA & rhs)
		{ this->b -= rhs.b; this->g -= rhs.g; this->r -= rhs.r; this->a -= rhs.a; }

	// Four methods, which convert itself from/to tjs_uint32 (packed ARGB),
	// are overrided.

	void operator += (tjs_uint32 v)
	{
		tjs_int aadj;
		this->a += (aadj = (v>>24));
		aadj += aadj >> 7;
		this->b += (v & 0xff) * aadj >> 8;
		this->g += ((v>>8) & 0xff) * aadj >> 8;
		this->r += ((v>>16) & 0xff) * aadj >> 8;
	}

	void operator -= (tjs_uint32 v)
	{
		tjs_int aadj;
		this->a -= (aadj = (v>>24));
		aadj += aadj >> 7;
		this->b -= (v & 0xff) * aadj >> 8;
		this->g -= ((v>>8) & 0xff) * aadj >> 8;
		this->r -= ((v>>16) & 0xff) * aadj >> 8;
	}

	void operator = (tjs_uint32 v)
	{
		this->a = v >> 24;
		tjs_int aadj = this->a + (this->a >> 7); // adjusted alpha
		this->r = ((v >> 16) & 0xff) * aadj >> 8,
		this->g = ((v >> 8) & 0xff) * aadj >> 8,
		this->b = (v & 0xff) * aadj >> 8;
	}

	operator tjs_uint32() const
	{
		tjs_uint8 *t = TVPDivTable + (this->a << 8);
		return t[this->b] + (t[this->g] << 8) + (t[this->r] << 16) + (this->a << 24);
	}

};



//---------------------------------------------------------------------------
#if defined(_WIN32) || defined(__GNUC__)
	// for assembler compatibility
	#pragma pack(pop)
#endif
//---------------------------------------------------------------------------
#endif
