/**********************************************
  Copyright Mentor Graphics Corporation 2018

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#pragma once

#include <guiddef.h>

#ifdef _WIN32
#define CNGPSTRING_EMBEDED_IMPL
#endif

/**
\brief This class is used to pass safely unicode string between dlls.
*/
class CNGPString
{
public:
	CNGPString( void );
	CNGPString( const wchar_ngp * lpsz );
	CNGPString( const wchar_ngp lpsz );
	CNGPString( const char lpsz );
	CNGPString( const char * lpMultiByteStr );
	CNGPString( const CNGPString & stringSrc );
	~CNGPString();

	CNGPString & operator = ( const wchar_ngp * lpsz );
	CNGPString & operator = ( const CNGPString & stringSrc );

	void AssignUTF8(const char * src);
	// special functions for windows-linux conversion due to different sizes of wchar_t
	void AssignUTF16(const boost::uint16_t * src);
	unsigned int GetUTF16(boost::uint16_t * dst) const;

	operator const wchar_ngp * () const { return c_str(); }
	bool IsEmpty() const;
	int ReverseFind( wchar_ngp ch ) const;
	int Find( wchar_ngp ch, int iStart = 0 ) const;
	int Find( const wchar_ngp *str, int iStart = 0 ) const;
	CNGPString Left( unsigned int nCount ) const;
	CNGPString Right ( unsigned int nCount ) const;
	CNGPString Mid ( unsigned int iFirst, unsigned int nCount ) const;
	CNGPString Mid ( unsigned int iFirst ) const
	{
		return Mid(iFirst, GetLength()-iFirst);
	}
	unsigned int GetLength( ) const;
	CNGPString& operator+=( const CNGPString& str );
	wchar_ngp& at ( int iPos );
	void Format ( const wchar_ngp* pszFormat, ... );
	CNGPString& TrimLeft ( );
	CNGPString& TrimRight ( );
	CNGPString& Trim ( );
	/**
	Converts the CNGPString object to an uppercase string.
	*/
	CNGPString& MakeUpper();
	/**
	Converts the CNGPString object to an lowercase string.
	*/
	CNGPString& MakeLower();
	/**
	Replaces a characterwith another.
	*/
	int Replace( wchar_ngp chOld, wchar_ngp chNew );
	/**
	Replaces a substring with another.
	*/
	void Replace( const wchar_ngp* chOld, const wchar_ngp* chNew );
	/**
	Get substring
	*/
	CNGPString GetSubstr(int nStartPos, wchar_t chStart, wchar_t chEnd);
	/**
	Removes all instances of the specified character from the string.
	*/
	int Remove( wchar_ngp chOld );
	/**
	*/

	static CNGPString BuildFromGUID (  const GUID& id );

	bool operator == ( const wchar_ngp * lpsz ) const;
	bool operator == ( const CNGPString & stringSrc ) const;
	bool operator != ( const wchar_ngp * lpsz ) const
	{
		return ! operator == (lpsz);
	}

	bool operator > ( const wchar_ngp * lpsz ) const;
	bool operator > ( const CNGPString & a ) const { return operator>( a.c_str() ); }
	bool operator < ( const wchar_ngp * lpsz ) const;
	bool operator < ( const CNGPString & a ) const { return operator<( a.c_str() ); }
	bool operator >= ( const wchar_ngp * lpsz ) const;
	bool operator >= ( const CNGPString & a ) const { return operator>=( a.c_str() ); }
	bool operator <= ( const wchar_ngp * lpsz ) const;
	bool operator <= ( const CNGPString & a ) const { return operator<=( a.c_str() ); }

	const wchar_ngp* c_str() const;
	bool empty() const { return IsEmpty(); }
	void clear();
	int CompareNoCase(const wchar_ngp * str) const;
	bool EqualNoCase(const wchar_ngp * str) const { return CompareNoCase(str)==0; }
	friend CNGPString operator + ( const CNGPString & string1, const CNGPString & string2 );
	friend CNGPString operator + ( const CNGPString & string1, const wchar_ngp * string2 );
	friend CNGPString operator + ( const wchar_ngp * string1, const CNGPString & string2 );

#ifdef CPP_MOVE_SEMANTIC_AVAILABLE
	CNGPString ( CNGPString&& );
	CNGPString& operator = ( CNGPString&& );
#endif

	struct isEqualNoCaseFunctor
	{
	public:
		isEqualNoCaseFunctor(const wchar_ngp * lpszStr) : m_lpszStr( lpszStr )
		{
		}

		bool operator() (const CNGPString & str) const
		{ 
			return str.EqualNoCase(m_lpszStr);
		}
	private:
		const wchar_ngp * m_lpszStr;
	};
	struct compare_no_case:std::binary_function<CNGPString,CNGPString, bool> 
	{
	public:
		bool operator( ) ( const CNGPString& a, const CNGPString& b ) const
		{
			return ( a.CompareNoCase ( b ) < 0 ) ? true : false;
		}
	};

	struct equal_no_case :
		std::binary_function<CNGPString, CNGPString, bool>
	{
	public:
		bool operator() (const CNGPString& a, const CNGPString& b) const
		{
			return a.EqualNoCase(b);
		}
	};

	struct hash
	{
		size_t operator() (const CNGPString& szStr) const
		{
			size_t seed = 0;
			for (int i = 0, end_i = szStr.GetLength(); i < end_i; i++) {
				boost::hash_combine(seed, szStr[i]);
			}
			return seed;
		}
	};

	struct hash_lower
	{
		size_t operator() (const CNGPString& szStr) const
		{
			CNGPString copy(szStr);
			copy.MakeLower();

			size_t seed = 0;
			for (int i = 0, end_i = copy.GetLength(); i < end_i; i++) {
				boost::hash_combine(seed, copy[i]);
			}
			return seed;
		}
	};

	template<class S> void load(S& s)
	{
		CNGPStringA strA;
		strA.load(s);
		AssignUTF8( strA.c_str() );
	}

	template<class S> void save(S& s) const
	{
		const CNGPStringA strA( *this );
		strA.save( s );
	}

	void show(std::ostream& s) const
	{
		s << "CNGPString: ";
		const int len = GetLength();
		if( len == 0 ) {
			s << "[empty]";
		} else {
//			s << CNGPStringA( *this ).c_str();
		}
	}

private:
	const wchar_ngp * const m_lpszDummyError;

#ifndef CNGPSTRING_EMBEDED_IMPL
	class CWStringShading *m_pStr;
#else
	class CWStringShading* impl() { return (CWStringShading*)&m_Placement; }
	const class CWStringShading* impl() const { return (const CWStringShading*)&m_Placement; }
#ifdef _WIN32
#ifdef _WIN64
	typedef void* TShadingPlacement[5];
#else
	typedef void* TShadingPlacement[6];
#endif
#else
	typedef void* TShadingPlacement[1];
#endif
	TShadingPlacement m_Placement;
#endif
};
