/**********************************************
  Copyright Mentor Graphics Corporation 2016

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#include "StdAfx.h"
#include <boost/test/unit_test.hpp>
#include "../ParamPalette.h"
#include "../PaletteController.h"
#include "../PalettesManager.h"
#include "../prop_ref_coll.h"
#include "../xml_tags.h"
#include "../xml_serialization_helpers.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

const wchar_t* const Pressure_param = L"pressure";
const wchar_t* const Temperature_param = L"temperature";
const wchar_t* const Velocity_param = L"velocity";

BOOST_AUTO_TEST_CASE(test_MinMaxSettingsStrategy)
{
	{
		CParamPalette pal;
		pal.SetParameter( Pressure_param );
		const CMinMaxValue& palmin = pal.GetParamMin();
		const CMinMaxValue& palmax = pal.GetParamMax();

		BOOST_CHECK( palmin.IsFromGlobal() && !palmin.IsValueInitialized() );
		BOOST_CHECK( palmax.IsFromGlobal() && !palmax.IsValueInitialized() );

		//Exporters expect plot's palette to be initialized (even if there is global value).
		pal.InitGlobalMinMaxesIfNotYet( 1, 100000 );
		
		BOOST_CHECK( palmin.IsValueInitialized() );
		//TODO: should IsFromGlobal() be still set?
		BOOST_CHECK( palmin.IsFromGlobal() && palmax.IsFromGlobal() );
		BOOST_CHECK( palmin.IsValueInitialized() && palmax.IsValueInitialized() );
		BOOST_CHECK( !pal.HasCustomMinForParam(pal.Parameter.uuid) && !pal.HasCustomMaxForParam(pal.Parameter.uuid) );

		//During palette manipulation (via colorbar) we preserve custom min/max values, but not global/plot ones
		const PARAM_MIN_MAX_VALUE custom_pressure_max(90000, false, false);
		pal.SetParamMax( custom_pressure_max );

		BOOST_CHECK( palmax.IsCustom() );
		BOOST_CHECK( pal.HasCustomMaxForParam(pal.Parameter.uuid) );

		pal.SetParameter( Temperature_param );

		BOOST_CHECK( !palmin.IsValueInitialized() && palmin.IsFromGlobal() );
		BOOST_CHECK( !palmax.IsValueInitialized() && palmax.IsFromGlobal() );
		BOOST_CHECK( !pal.HasCustomMinForParam(pal.Parameter.uuid) );
		BOOST_CHECK( pal.HasCustomMaxForParam(Pressure_param) );
		PARAM_MIN_MAX_VALUE mmvalue;
		pal.GetParamMax( Pressure_param, mmvalue );
		BOOST_CHECK( mmvalue == custom_pressure_max );

		pal.SetParameter( Pressure_param );

		BOOST_CHECK( !palmin.IsValueInitialized() && palmin.IsFromGlobal() );
		BOOST_CHECK( palmax.IsValueInitialized() && palmax.IsCustom() );
	}
}

BOOST_AUTO_TEST_CASE(test_PalettesManagerBasics)
{
	std::unique_ptr<CPalettesManager> palMng( new CPalettesManager(nullptr) );

	{
		CParamPalette pal;
		BOOST_CHECK( !pal.ID.empty() );
	}

	{
		CPalettePtr pal = CParamPalette::Instance(false);
		BOOST_CHECK( pal->ID.empty() );
	}

	// create and register palette manually
	palette_id_t id;
	{
		CPalettePtr pal = CParamPalette::Instance(true);
		pal->SetParameter( Pressure_param );
		pal->SetParamMin( CMinMaxValue(99000,true,false) );
		pal->SetParamMax( CMinMaxValue(101000,true,false) );
		palMng->AddPalette( pal );
		BOOST_CHECK( palMng->PaletteExists( pal->ID ) );
		id = pal->ID;
	}
	CPaletteController pal_c1 = palMng->EditPalette( id );
	BOOST_CHECK( pal_c1.IsInitialized() && pal_c1.IsPaletteInitialized() );
	BOOST_CHECK( pal_c1.ID() == id );

	// create and register palette indirectly
// 	CPaletteController pal_c2( palMng.get(), nullptr );
// 	if ( IPlotPalette* pal_editor = pal_c2.GetForEditing() )
// 	{
// 		pal_editor->SetParameter( Temperature_param );
// 		pal_c2.FinishEditing();
// 	}
// 	BOOST_CHECK( palMng->PaletteExists( pal_c2.ID() ) );
// 	BOOST_CHECK( pal_c1.ID() != pal_c2.ID() );

	// share palette
	CPaletteController pal_c3 = pal_c1;
	BOOST_CHECK( pal_c3.IsInitialized() && pal_c3.IsPaletteInitialized() );
	BOOST_CHECK( pal_c3.ID() == pal_c1.ID() );

// 	pal_c3 = pal_c2;
// 	BOOST_CHECK( pal_c3.ID() == pal_c2.ID() );

	// modify existing palette and see it has merged
	if ( IPlotPalette* pal_editor = pal_c3.GetForEditing() )
	{
		pal_editor->SetParameter( Pressure_param );	// min/max are ignored in SetParameter
		pal_c3.FinishEditing();
		BOOST_CHECK( pal_c3.ID() == pal_c1.ID() );
	}
}

//////////////////////////////////////////////////////////////////////////

// Uncomment this to define oritinal behavior (before removing CParamPalette::PrepareForComparison)
// #define OBSOLETE_VIA_PREPARE_COMPARISON

#ifdef OBSOLETE_VIA_PREPARE_COMPARISON
void PrepareForComparison( CMinMaxValue& mmv )
{
	// code from CParamPalette::PrepareForComparison()
	if ( mmv.m_bIsFromGlobal || mmv.m_bIsFromPlot )
	{
		mmv.m_dblValue = 0;
		mmv.m_bIsValueInitialized = false;
	}
}
#endif

bool TestMinMaxEquality( const CMinMaxValue& lhs, const CMinMaxValue& rhs )
{
#ifdef OBSOLETE_VIA_PREPARE_COMPARISON
	CMinMaxValue m1 = lhs;
	CMinMaxValue m2 = rhs;
	PrepareForComparison( m1 );
	PrepareForComparison( m2 );
	const EPropertiesFilter filter = PF_All;
#else
	const CMinMaxValue& m1 = lhs;
	const CMinMaxValue& m2 = rhs;
	const EPropertiesFilter filter = PF_ForEqualityTest;
#endif

	CPropertyRefCollection coll1( tag_minmax, filter ), coll2( tag_minmax, filter );
	XML_serialization_helpers::Add( m1, nullptr, coll1 );
	XML_serialization_helpers::Add( m2, nullptr, coll2 );
	const bool equal_by_coll = coll1 == coll2;
	const bool equal_direct = CMinMaxValue::AreSameForFeatureComparison( m1, m2 );
	_ASSERTE( equal_by_coll == equal_direct );
	BOOST_CHECK_EQUAL( equal_by_coll, equal_direct );
	return equal_by_coll;
}

BOOST_AUTO_TEST_CASE( Test_MinMaxInFeatureComparison )
{
	const CMinMaxValue mm_empty;
	const CMinMaxValue mm_custom( 0.1, false, false );
	const CMinMaxValue mm_custom_2( 0.2, false, false );
	const CMinMaxValue mm_global_i( 0.1, true, false );
	const CMinMaxValue mm_global_i_2( 0.2, true, false );
	const CMinMaxValue mm_global_ni( PARAM_MIN_MAX_VALUE( true, false ) );
	const CMinMaxValue mm_plot_i( 0.1, false, true );
	const CMinMaxValue mm_plot_i_2( 0.2, false, true );
	const CMinMaxValue mm_plot_ni( PARAM_MIN_MAX_VALUE( false, true ) );

	// custom value
	BOOST_CHECK( TestMinMaxEquality( mm_custom, mm_custom ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_custom, mm_custom_2 ) );

	// global / from plot
	BOOST_CHECK( TestMinMaxEquality( mm_global_i, mm_global_i ) );
	BOOST_CHECK( TestMinMaxEquality( mm_global_i, mm_global_i_2 ) );
	BOOST_CHECK( TestMinMaxEquality( mm_global_i, mm_global_ni ) );
	BOOST_CHECK( TestMinMaxEquality( mm_global_ni, mm_global_ni ) );

	// global vs custom
	BOOST_CHECK( !TestMinMaxEquality( mm_global_i, mm_custom ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_global_i, mm_custom_2 ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_global_ni, mm_custom ) );

	// from plot
	BOOST_CHECK( TestMinMaxEquality( mm_plot_i, mm_plot_i ) );
	BOOST_CHECK( TestMinMaxEquality( mm_plot_i, mm_plot_i_2 ) );
	BOOST_CHECK( TestMinMaxEquality( mm_plot_i, mm_plot_ni ) );
	BOOST_CHECK( TestMinMaxEquality( mm_plot_ni, mm_plot_ni ) );

	// plot vs custom
	BOOST_CHECK( !TestMinMaxEquality( mm_plot_i, mm_custom_2 ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_plot_i, mm_custom ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_plot_ni, mm_custom ) );

	// global vs plot
	BOOST_CHECK( !TestMinMaxEquality( mm_global_i, mm_plot_i ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_global_i, mm_plot_i_2 ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_global_i, mm_plot_ni ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_global_ni, mm_plot_ni ) );

	// non-initialized
	BOOST_CHECK( TestMinMaxEquality( mm_empty, mm_empty ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_empty, mm_custom ) );
//?	BOOST_CHECK( !TestMinMaxEquality( mm_empty, mm_global_i ) );
//?	BOOST_CHECK( !TestMinMaxEquality( mm_empty, mm_global_ni ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_empty, mm_plot_i ) );
	BOOST_CHECK( !TestMinMaxEquality( mm_empty, mm_plot_ni ) );
}

//////////////////////////////////////////////////////////////////////////
