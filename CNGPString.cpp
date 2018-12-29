/**********************************************
  Copyright Mentor Graphics Corporation 2017

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#include "StdAfx.h"

#include <boost/scoped_array.hpp>
#include <algorithm>
#include <stdarg.h>
#include <cctype>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/if.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <CNGPStream.h>
#include <CNGPStringResourceDll.h>

#include <CNGPString.h>
#include <CNGPStringA.h>
#include "CWStringShading.h"

#ifdef _WIN32
#include <Rpc.h>
#include <atlstr.h>
#else
#include <linux_io_func.h>
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef CNGPSTRING_EMBEDED_IMPL

	#define INIT_pStr
	#define SHADING_PLACEMENT (&m_Placement)
	#define m_pStr impl()

#else

	#define INIT_pStr m_pStr =
	#define SHADING_PLACEMENT

#if defined _DEBUG && defined _WIN32
	#define new DEBUG_CLIENTBLOCK
#endif

#endif

//////////////////////////////////////////////////////////////////////////

static const wchar_ngp *lpszDummyError = ( wchar_ngp* ) L" ERROR: use c_str() ";

//////////////////////////////////////////////////////////////////////////

CNGPString::CNGPString(void) : m_lpszDummyError(lpszDummyError)
{
	INIT_pStr new SHADING_PLACEMENT CWStringShading;

#ifdef CNGPSTRING_EMBEDED_IMPL
	static_assert( sizeof(TShadingPlacement) >= sizeof(CWStringShading), "insufficient room for CWStringShading" );
	static_assert( __alignof(TShadingPlacement) == __alignof(CWStringShading), "inefficient aligning of CWStringShading member" );
#endif
}

CNGPString::CNGPString(const wchar_ngp lpsz) : m_lpszDummyError(lpszDummyError)
{
	INIT_pStr new SHADING_PLACEMENT CWStringShading;
	m_pStr->push_back ( lpsz );
}

CNGPString::CNGPString(const char lpsz) : m_lpszDummyError(lpszDummyError)
{
	INIT_pStr new SHADING_PLACEMENT CWStringShading;
	m_pStr->push_back ( lpsz );
}

CNGPString::CNGPString( const wchar_ngp * lpsz ) : m_lpszDummyError(lpszDummyError)
{
	if(lpsz)
		INIT_pStr new SHADING_PLACEMENT CWStringShading( ( const wchar_t * ) lpsz );
	else
		INIT_pStr new SHADING_PLACEMENT CWStringShading;
}

CNGPString::CNGPString( const char * lpsz ) : m_lpszDummyError(lpszDummyError)
{
//	if(lpsz)
//		INIT_pStr new SHADING_PLACEMENT CWStringShading( lpsz );
//	else
		INIT_pStr new SHADING_PLACEMENT CWStringShading;

	if( lpsz )
		AssignUTF8( lpsz );
}

CNGPString::CNGPString( const CNGPString & stringSrc ) : m_lpszDummyError(lpszDummyError)
{
	INIT_pStr new SHADING_PLACEMENT CWStringShading( *stringSrc.m_pStr );
}

#ifdef CPP_MOVE_SEMANTIC_AVAILABLE

CNGPString::CNGPString( CNGPString&& rhs )
	 : m_lpszDummyError(lpszDummyError)
{
	INIT_pStr new SHADING_PLACEMENT CWStringShading( std::move( *rhs.impl() ) );
}

CNGPString& CNGPString::operator = ( CNGPString&& rhs )
{
	*impl() = std::move( *rhs.impl() );
	return *this;
}

#endif

CNGPString::~CNGPString(void)
{
#ifdef CNGPSTRING_EMBEDED_IMPL
	m_pStr->~CWStringShading();
#else
	delete m_pStr;
#endif
}

CNGPString & CNGPString::operator =( const wchar_ngp * lpsz )
{
	if(lpsz)
		*m_pStr = ( const wchar_t* ) lpsz;
	else
		*m_pStr = L"";

	return *this;
}

CNGPString & CNGPString::operator =( const CNGPString& stringSrc )
{
	*m_pStr = *stringSrc.m_pStr;
	return *this;
}

bool CNGPString::IsEmpty() const
{
	return m_pStr->empty();
}

int CNGPString::ReverseFind( wchar_ngp ch ) const
{
	return static_cast<int> ( m_pStr->rfind( ch ) );
}

int CNGPString::Find( wchar_ngp ch, int iStart ) const
{
	return static_cast<int> ( m_pStr->find( ch, iStart ) );
}

CNGPString CNGPString::Left( unsigned int nCount ) const
{
	CNGPString str;
	str.m_pStr->assign( *m_pStr, 0, nCount );
	return str;
}

CNGPString CNGPString::Right ( unsigned int nCount ) const
{
	CNGPString str;
	if ( nCount >= GetLength ( ) ) {
		return *this;
	}
	else if ( nCount > 0 && nCount < GetLength ( ) ) {
		str.m_pStr->assign( m_pStr->end ( ) - nCount,  m_pStr->end ( ) );
	}
	return str;
}

unsigned int CNGPString::GetLength() const
{
	return static_cast<unsigned int> ( m_pStr->size() );
}

bool CNGPString::operator == ( const wchar_ngp * lpsz ) const
{
	if( !lpsz )
		return IsEmpty();

	return ( *m_pStr == ( const wchar_t* ) lpsz );
}

bool CNGPString::operator == ( const CNGPString & stringSrc ) const
{
	return ( *m_pStr == *stringSrc.m_pStr );
}

int CNGPString::CompareNoCase(const wchar_ngp *  str) const
{
	if ( !str )
	{
		if ( m_pStr->empty() )
			return 0;	//both strings are empty
		else
			return 2;	//strings are not equal
	}
	return _wcsicmp(m_pStr->c_str(), ( const wchar_t* ) str); 
}

CNGPString& CNGPString::operator+=( const CNGPString& str )
{
	m_pStr->append ( *str.m_pStr );
	return *this;
}

bool CNGPString::Load_String ( unsigned int id, void * hInstance )
{
#ifdef _WIN32
	if( !hInstance ) return false;

	CString sz;
	if ( ! sz.LoadString ( (HINSTANCE)hInstance, id ) )
		return false;

	*this = (const wchar_ngp *) sz.GetString ( );
	return true;
#else		// Unix
	return false;		// no resources in Unix...
#endif
}

bool CNGPString::Load_String ( unsigned int id, const CNGPStringResourceDll * pStringResourceDll )
{
	if( ! pStringResourceDll )
		return false;

	return pStringResourceDll->LoadResourceString( id, *this );
}

NGP_UTILS_API CNGPString operator + ( const CNGPString & string1, const CNGPString & string2 )
{
	CNGPString str( string1 );
	str.m_pStr->append ( *string2.m_pStr );
	return str;
}

NGP_UTILS_API CNGPString operator + ( const CNGPString & string1, const wchar_ngp * string2 )
{
	CNGPString str( string1 );
	str.m_pStr->append ( string2 );
	return str;
}

NGP_UTILS_API CNGPString operator + ( const wchar_ngp * string1, const CNGPString & string2 )
{
	CNGPString str( string1 );
	str.m_pStr->append ( string2.m_pStr->c_str() );
	return str;
}

wchar_ngp& CNGPString::at ( int iPos )
{
	return ( wchar_ngp& ) m_pStr->at(iPos);
}

CNGPString CNGPString::Mid ( unsigned int iFirst, unsigned int nCount ) const
{
	CNGPString str;
	if ( iFirst >= m_pStr->length ( ) )
		return str;
	if ( ( iFirst + nCount ) >  m_pStr->length ( ) )
		nCount = static_cast<int> ( m_pStr->length ( ) - iFirst );
	str.m_pStr->assign ( m_pStr->begin ( ) + iFirst, m_pStr->begin ( ) + iFirst + nCount );
	return str;
}

void CNGPString::clear ( )
{
	m_pStr->clear ( );
}

void CNGPString::Format ( const wchar_ngp* _Fmt, ... )
{
	try {
		va_list _Args;
		va_start( _Args, _Fmt );

		boost::wformat format( (const wchar_t *)_Fmt );

		while( *_Fmt != L'\0' ) {
			while( *_Fmt != L'\0' && *_Fmt != L'%' ) {
				++ _Fmt;
			}
			if( *_Fmt == L'\0' )
				break;

			++ _Fmt;
			bool bProcessed = false;

			while( *_Fmt != L'\0' && !bProcessed) {
				bProcessed = true;
				switch( *_Fmt ) {
					case L'c':
						format % (char)va_arg(_Args, int); break;
					case L's':
						format % va_arg(_Args, const wchar_t *); break;
					case L'S':
						format % va_arg(_Args, const char *); break;
					case L'd':
					case L'i':
						format % va_arg(_Args, int); break;
					case L'o':
					case L'u':
					case L'x':
					case L'X':
						format % va_arg(_Args, unsigned int); break;
                    case L'p':
                        format % va_arg(_Args, uintptr_t); break;
					case L'e':
					case L'E':
					case L'f':
					case L'g':
					case L'G':
					case L'a':
					case L'A':
						format % va_arg(_Args, double); break;
					case L'%':
						break;
					default:
						bProcessed = false; break;
				}

				++ _Fmt;
			}
		}

		va_end (_Args);

		*m_pStr = format.str().c_str();
	} catch( const boost::io::format_error & e ) {
		m_pStr->assign( L"ERROR in CNGPString::Format : " );
		m_pStr->append( (const wchar_t *)CNGPString( e.what() ).c_str() );
	}
}

int CNGPString::Find( const wchar_ngp *str, int iStart ) const
{
	return static_cast<int> ( m_pStr->find( ( const wchar_t* ) str, iStart ) );
}

CNGPString CNGPString::BuildFromGUID ( const GUID& id )
{
	_ASSERT( sizeof(id)==16 );

	try {
		boost::format format("%08x-%04x-%04x-%04x-%04x%08x");
		format % id.Data1 % id.Data2 % id.Data3;
		format % ((id.Data4[0]<<8) + id.Data4[1]);
		format % ((id.Data4[2]<<8) + id.Data4[3]);
		format % ((id.Data4[4]<<24) + (id.Data4[5]<<16) + (id.Data4[6]<<8) + id.Data4[7]);
		return CNGPString( format.str().c_str() );
	} catch( const boost::io::format_error & e ) {
		CNGPString str( (const wchar_ngp *)L"ERROR in CNGPString::BuildFromGUID : " );
		str += CNGPString( e.what() );
		return str;
	}
}

CNGPString& CNGPString::TrimLeft ( )
{
	if ( m_pStr->empty ( ) )
		return *this;
	using namespace boost::lambda;
	std::wstring::iterator i = std::find_if( m_pStr->begin( ), m_pStr->end ( ), ! bind (  &iswspace, boost::lambda::_1 ) );
	m_pStr->erase ( m_pStr->begin( ), i );
	return *this;
}

CNGPString& CNGPString::TrimRight ( )
{
	if ( m_pStr->empty ( ) )
		return *this;
	using namespace boost::lambda;
	std::wstring::reverse_iterator i = std::find_if( m_pStr->rbegin( ), m_pStr->rend ( ), ! bind (  &iswspace, boost::lambda::_1 ) );
	m_pStr->erase ( ( m_pStr->begin( ) + (  m_pStr->rend ( ) - i ) ), m_pStr->end ( ) );
	return *this;
}


CNGPString& CNGPString::Trim ( )
{
	TrimLeft ( );
	TrimRight ( );
	return *this;
}

const wchar_ngp* CNGPString::c_str() const
{
	return ( const wchar_ngp* ) m_pStr->c_str();
}

int CNGPString::Replace( wchar_ngp chOld, wchar_ngp chNew )
{
	int i = 0;
	using namespace boost::lambda;
	std::for_each ( m_pStr->begin ( ), m_pStr->end ( ), if_then ( boost::lambda::_1 == chOld, ( boost::lambda::_1 = chNew , var ( i ) ++ ) ) );
	return i;
}

CNGPString CNGPString::GetSubstr(int nStartPos, wchar_t chStart, wchar_t chEnd)
{
	CNGPString strSubstr;
	if ( GetLength() )
	{
		int nStart = Find(chStart, nStartPos);
		if ( nStart != - 1 )
		{
			int nEnd = Find(chEnd, nStart + 1 );
			if ( nEnd != -1 )
			{
				strSubstr = Mid(nStart, nEnd - nStart + 1);
			}
		}
	}
	return strSubstr;
}
int CNGPString::Remove( wchar_ngp ch )
{
	int iCount = 0;
	std::wstring::size_type i = m_pStr->find ( ch );
	while ( i != std::wstring::npos ) {
		m_pStr->erase ( i, 1 );
		iCount ++;
		i = m_pStr->find ( ch, i );
	}
	return iCount;
}

bool CNGPString::operator > ( const wchar_ngp * lpsz ) const
{
	return wcscmp( m_pStr->c_str(), (const wchar_t *)lpsz) > 0;
}

bool CNGPString::operator < ( const wchar_ngp * lpsz ) const
{
	return wcscmp( m_pStr->c_str(), (const wchar_t *)lpsz) < 0;
}

bool CNGPString::operator >= ( const wchar_ngp * lpsz ) const
{
	return wcscmp( m_pStr->c_str(), (const wchar_t *)lpsz) >= 0;
}

bool CNGPString::operator <= ( const wchar_ngp * lpsz ) const
{
	return wcscmp( m_pStr->c_str(), (const wchar_t *)lpsz) <= 0;
}

void CNGPString::Replace( const wchar_ngp* chOld, const wchar_ngp* _chNew )
{
	if ( ! chOld )
		return;

	size_t length_old = wcslen ( ( const wchar_t* ) chOld );
	if ( ! length_old )
		return;

	static const wchar_ngp* empty_string = ( const wchar_ngp* ) L"";

	const wchar_ngp* chNew = ( _chNew ) ? _chNew : empty_string;

	boost::replace_all(*m_pStr, chOld, chNew);
}

CNGPString& CNGPString::MakeUpper()
{
	if ( ! GetLength ( ) )
		return *this;

	std::vector<wchar_t> data ( m_pStr->size ( ) + 1 );
	wcscpy ( &data.front ( ), m_pStr->c_str ( ) );
	_wcsupr ( &data.front ( ) );
	*m_pStr = &data.front ( );
	return *this;
}

CNGPString& CNGPString::MakeLower()
{
	if ( ! GetLength ( ) )
		return *this;

	std::vector<wchar_t> data ( m_pStr->size ( ) + 1 );
	wcscpy ( &data.front ( ), m_pStr->c_str ( ) );
	_wcslwr ( &data.front ( ) );
	*m_pStr = &data.front ( );
	return *this;
}

bool CNGPString::serialize ( CNGPBasicArchive& archive )
{
	bool res = false;
	if ( archive.IsStoring ( ) ) {
		CNGPStringA sz ( c_str() );
		size_t length = strlen ( sz.c_str ( ) );
		ngp_shared_array<char> data;
		data.resize ( length + 1 );
		data.at ( length ) = 0;
		memcpy ( data.c_array ( ), sz.c_str ( ), length );
		res = archive.serialize ( data );
	}
	else {
		ngp_shared_array<char> data;
		res = archive.serialize ( data );
		if ( res && data.size ( ) ) {
//			*m_pStr = CWStringShading( data.begin ( ) );
			AssignUTF8( data.begin() );
		}
	}
	return res;
}

static void convert_utf8_to_utf16(const boost::uint8_t * src, boost::uint16_t * dst)
{
	do {
		if( (*src & 0xE0) == 0xE0) {
			*dst ++ = ((src[0] & 0x0F) << 12) + ((src[1] & 0x3F) << 6) + (src[2] & 0x3F);
			++ src;
			++ src;
		} else if( (*src & 0xC0) == 0xC0 ) {
			*dst ++ = ((src[0] & 0x1F) << 6) + (src[1] & 0x3F);
			++ src;
		} else /*if( *src < 0x80 )*/ {
			*dst ++ = *src;
		}
	} while( *src ++ );
}

static int calc_utf8_to_utf16_strlen( const boost::uint8_t * src )
{
	int src_len = 0;
	for( ; src[src_len] != 0; ++src_len);
	const boost::uint8_t * end = src + src_len;

	int count = 0;
	for ( ; *src != 0; ++ count ) {
		if( (*src & 0xE0) == 0xE0) {
			++ src;
			++ src;
			++ src;
		} else if( (*src & 0xC0) == 0xC0 ) {
			++ src;
			++ src;
		} else /*if( *src < 0x80 )*/ {
			++ src;
		}

		if( src > end )
			return -1;		// error
	}
	return count;
}

#ifndef _WIN32		// Linux stuff

static void convert_utf16_to_utf32(const boost::uint16_t * src, boost::uint32_t * dst)
{
	do {
		if( (*src & 0xD800) == 0xD800 ) {
			*dst ++ = ((src[0] & 0x07ff) << 10) + (src[1] & 0x07ff) + 0x10000;
			++ src;
		} else {
			*dst ++ = *src;
		}
	} while( *src ++ );
}

static int calc_utf16_to_utf32_strlen( const boost::uint16_t * src )
{
	int src_len = 0;
	for( ; src[src_len] != 0; ++src_len);
	const boost::uint8_t * end = (const boost::uint8_t *)( src + src_len );

	int count = 0;
	for ( ; *src != 0; ++ count ) {
		if ( *src >= 0xD800 && *src <= 0xDBFF ) {
			++ src;
			if ( *src < 0xDC00 || *src > 0xDFFF ) {
//				throw ( L"Trailing character in a UTF-16 surrogate pair is missing" );
				_ASSERT(0);
				return -1;
			}
			++ src;
		} else if ( *src >= 0xDC00 && *src <= 0xDFFF ) {
//			throw ( L"Trailing character in a UTF-16 surrogate pair has been found first (" + toString( *seq ) + L")" );
			_ASSERT(0);
			return -1;
		} else {
			++ src;
		}

		if( src > end )
			return -1;		// error
	}
	return count;
}

static void convert_utf32_to_utf16(const boost::uint32_t * src, boost::uint16_t * dst)
{
	do {
		if ( *src < 0x10000 ) {
			*dst ++ = *src;
		} else {
			*dst ++ = 0xD800 - ( 0x10000 >> 10 ) + static_cast< boost::uint16_t >( (*src) >> 10 );
			*dst ++ = 0xDC00 + static_cast< boost::uint16_t >( (*src) & 0x3FF );
		}
	} while( *src ++ );
}

static int calc_utf32_to_utf16_strlen(const boost::uint32_t * src)
{
	int count = 0;
	while( *src ) {
		if( *src < 0x10000 )
			count += 1;
		else
			count += 2;

		++ src;
	}
	return count;
}

#endif

void CNGPString::AssignUTF8(const char * src)
{
	const int utf16_len = calc_utf8_to_utf16_strlen( reinterpret_cast<const boost::uint8_t *>( src ) );
	if( utf16_len <= 0 ) {
		clear();
		return;
	}
	boost::scoped_array< boost::uint16_t > dstStr( new boost::uint16_t [ utf16_len + 1 ] );
	convert_utf8_to_utf16( reinterpret_cast<const boost::uint8_t *>( src ), dstStr.get() );

	AssignUTF16( dstStr.get() );
}

void CNGPString::AssignUTF16(const boost::uint16_t * src)
{
#ifdef _WIN32
	_ASSERT( sizeof(wchar_t)==2 );

	// this is simple
	*this = src;
#else		// Unix
	_ASSERT( sizeof(wchar_t)==4 );

	const int utf32_len = calc_utf16_to_utf32_strlen(src);
	if( utf32_len <= 0 ) {
		clear();
		return;
	}
	boost::scoped_array< boost::uint32_t > dstStr( new boost::uint32_t [ utf32_len + 1 ] );
	convert_utf16_to_utf32( src, dstStr.get() );
	*m_pStr = (const wchar_t *)( dstStr.get() );
#endif		// _WIN32
}

unsigned int CNGPString::GetUTF16(boost::uint16_t * dst) const
{
#ifdef _WIN32
	_ASSERT( sizeof(wchar_t)==2 );

	if(dst==NULL)
		return GetLength() + 1;

	if( empty() ) {
		*dst = 0;
	} else {
		boost::uint16_t * src = (boost::uint16_t *)c_str();
		do {
			*dst ++ = *src;
		} while( *src++ );
	}
#else		// Unix
	_ASSERT( sizeof(wchar_t)==4 );

	if(dst==NULL)
		return calc_utf32_to_utf16_strlen( (boost::uint32_t *)c_str() ) + 1;

	if( empty() ) {
		*dst = 0;
	} else {
		convert_utf32_to_utf16( (boost::uint32_t *)c_str(), dst );
	}
#endif		// _WIN32

	return 0;
}
