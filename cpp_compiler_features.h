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

//////////////////////////////////////////////////////////////////////////

#if (_MSC_VER >= 1700)

	#define CPP_MOVE_SEMANTIC_AVAILABLE 

#endif


#if (_MSC_VER >= 1900) || !(defined _WIN32)

#define CPP_VARIADIC_TEMPLATES_AVAILABLE 
#define CPP_INITIALIZER_LISTS_AVAILABLE
#define CPP_CONSTEXPR_AVAILABLE

#endif

//////////////////////////////////////////////////////////////////////////

template <typename T>
class TypeDisplayer;

#define WATCH_TYPE( type ) \
TypeDisplayer<type> _type_displayer_;

#define WATCH_VAR_TYPE( var ) \
TypeDisplayer<decltype(var)> _type_displayer_;

//////////////////////////////////////////////////////////////////////////

#ifdef CPP_VARIADIC_TEMPLATES_AVAILABLE

template< typename T >
struct get_host_class; // most types are not ptmfs: don't implement this

template< typename CLASS, typename RETTYPE, class... ARGTYPES >
struct get_host_class< RETTYPE( CLASS::* )(ARGTYPES...) >	// implement partial specialization
{
	typedef CLASS host_class_t;
	typedef RETTYPE return_t;
};

#endif

//////////////////////////////////////////////////////////////////////////

#ifndef CPP_CONSTEXPR_AVAILABLE
    #define constexpr
#endif