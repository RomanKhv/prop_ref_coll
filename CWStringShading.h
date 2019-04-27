/**********************************************
  Copyright Mentor Graphics Corporation 2017

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#pragma once
#include <string>

class CWStringShading : public std::wstring
{
public:
	CWStringShading( )
	{
	}
	CWStringShading( const wchar_t * lpsz ) : std::wstring( lpsz )
	{
	}
/*	CWStringShading( const char * lpsz )
	{
		if ( ! lpsz )
			return;

#ifdef _WIN32
		int iRes = MultiByteToWideChar ( CP_THREAD_ACP, MB_PRECOMPOSED, lpsz, -1, NULL, 0 );
		if ( ! iRes )
			return;

		wchar_t *str = ( wchar_t* ) calloc ( iRes, sizeof ( wchar_t ) );
		if ( ! str )
			return;

		MultiByteToWideChar ( CP_THREAD_ACP, MB_PRECOMPOSED,  lpsz, -1, str, iRes );
		*this = str;
		free ( str );
#else
		const size_t iLen = mbstowcs(NULL, lpsz, 0) + 1;
		if( iLen<1 )
			return;

		resize( iLen-1 );

		mbstowcs( &(operator[](0)), lpsz, iLen );
#endif
	}*/
	CWStringShading( const CWStringShading & lpsz ) : std::wstring( lpsz )
	{
	}

	inline std::wstring & operator = (const wchar_t * lpsz)
	{
		return std::wstring::operator = ( lpsz );
	}

#ifdef CPP_MOVE_SEMANTIC_AVAILABLE
	CWStringShading( CWStringShading&& rhs )
		: std::wstring( std::move( rhs ) )
	{
	}

	CWStringShading& operator = (const CWStringShading& rhs)
	{
		std::wstring::operator=( rhs );
		return *this;
	}

	CWStringShading& operator = ( CWStringShading&& rhs )
	{
		std::wstring::operator=( std::move( rhs ) );
		return *this;
	}
#endif
};

