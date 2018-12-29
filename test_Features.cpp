/**********************************************
  Copyright Mentor Graphics Corporation 2018

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#include "StdAfx.h"
#include <boost/test/unit_test.hpp>
#include "../CutPlot.h"
#include "../SurfacePlot.h"
#include "../MeshPlot.h"
#include "../FlowTrajectoriesPlot.h"
#include "../ParticleStudy.h"
#include "../GoalPlot.h"
#include "../IsosurfacePlot.h"
#include "../SurfaceParametersPlot.h"
#include "../PointParametersPlot.h"
#include "../XYPlot.h"
#include "../ParticleInjection.h"
#include "../RayPlot.h"
#include "../AnsysExport.h"
#include "../AbaqusExport.h"
#include "../MechanicaExport.h"
#include "../MeshTextExport.h"
#include "../MpcciExport.h"
#include "../NastranExport.h"
#include "../FluxBalance.h"
#include "../feature_utils.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1900
#define  BOOST_CHECK_STATIC_VECTOR( c ) BOOST_CHECK(c);
#else
#define  BOOST_CHECK_STATIC_VECTOR( c )
#endif

//////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(Test_Cutplot_Elements)
{
	CCutPlotPtr plot = CCutPlot::Instance();
	plot->Contours_Display = true;
	plot->Contours_ColorByParam = true;
	plot->Isolines_Display = true;
	plot->Isolines_ColorByParam = true;
	plot->Vectors_Display = true;
	plot->Vectors_ColorByParam = false;
	plot->ESSSLIC_Display = false;
	plot->ESSSLIC_ColorByParam = true;
	plot->Mesh_Display = true;
	plot->OutLines_Display = true;
	{
		std::list<EGraphicalPlotElement> elements;
		plot->GetElements( elements, EC_AllDisplayable );
		BOOST_CHECK_EQUAL( elements.size(), 7 );
		BOOST_CHECK( Contains(elements, avContours) );
		BOOST_CHECK( Contains(elements, avIsolines) );
		BOOST_CHECK( Contains(elements, avVectors) );
		BOOST_CHECK( Contains(elements, avESSSLIC) );
		BOOST_CHECK( Contains(elements, avMesh) );
		BOOST_CHECK( Contains(elements, avBkgd) );
		BOOST_CHECK( Contains(elements, avOutlines) );
		BOOST_CHECK_STATIC_VECTOR( elements.size() <= plot_elements_t().capacity() );
		// boost::container::static_vector<>::capacity() is unavailable in VS2015
	}
	{
		std::list<EGraphicalPlotElement> elements;
		plot->GetElements( elements, EC_AllWithPalettes );
		BOOST_CHECK_EQUAL( elements.size(), 6 );
		BOOST_CHECK( Contains(elements, avContours) );
		BOOST_CHECK( Contains(elements, avIsolines) );
		BOOST_CHECK( Contains(elements, avVectors) );
		BOOST_CHECK( Contains(elements, avVectorGlyph) );
		BOOST_CHECK( Contains(elements, avESSSLIC) );
		BOOST_CHECK( Contains(elements, avESSSLICGlyph) );
		BOOST_CHECK_STATIC_VECTOR( elements.size() <= plot_elements_t().capacity() );
	}
	{
		std::list<EGraphicalPlotElement> elements;
		plot->GetElements( elements, EC_AllPainting );
		BOOST_CHECK_EQUAL( elements.size(), 4 );
		BOOST_CHECK( Contains(elements, avContours) );
		BOOST_CHECK( Contains(elements, avIsolines) );
		BOOST_CHECK( Contains(elements, avVectors) );
		BOOST_CHECK( Contains(elements, avESSSLIC) );
		BOOST_CHECK_STATIC_VECTOR( elements.size() <= plot_elements_t().capacity() );
	}
	{
		std::list<EGraphicalPlotElement> elements;
		plot->GetElements( elements, EC_Checked_Painting );
		BOOST_CHECK_EQUAL( elements.size(), 3 );
		BOOST_CHECK( Contains(elements, avContours) );
		BOOST_CHECK( Contains(elements, avIsolines) );
		BOOST_CHECK( Contains(elements, avVectors) );
	}
	{
		std::list<EGraphicalPlotElement> elements;
		plot->GetElements( elements, EC_Checked_ColoredByParam );
		BOOST_CHECK_EQUAL( elements.size(), 2 );
		BOOST_CHECK( Contains(elements, avContours) );
		BOOST_CHECK( Contains(elements, avIsolines) );
	}
}

BOOST_AUTO_TEST_CASE(Test_Mesh_Elements)
{
	CMeshPlotPtr plot = CMeshPlot::Instance();
	plot->SetDisplayContours( true );
	plot->SetColorByParam( true );
	plot->SetPlotsDisplayMesh( true );
	plot->SetCutWithSection( true );
	plot->SetChannelsDisplayMesh( true );
	{
		std::list<EGraphicalPlotElement> elements;
		plot->GetElements( elements, EC_AllDisplayable );
		BOOST_CHECK_STATIC_VECTOR( elements.size() <= plot_elements_t().capacity() );
	}
}

BOOST_AUTO_TEST_CASE( Test_AsParticleBase )
{
	{
		CFlowTrajectoriesPlotPtr ft = CFlowTrajectoriesPlot::Instance();
		CFeature* const f = ft.get();
		BOOST_CHECK( feature_utils::AsParticleBase( f ) == ft.get() );
	}
	{
		CParticleStudyPtr ps = CParticleStudy::Instance();
		CFeature* const f = ps.get();
		BOOST_CHECK( feature_utils::AsParticleBase( f ) == ps.get() );
	}
}

BOOST_AUTO_TEST_CASE(Test_QI)
{
	for (FeatureType t = ft_FIRST; t <= ft_LAST; ++((int&)t))
	{
		CFeaturePtr feat = feature_utils::CreateFeature(t);
		switch (t)
		{
		case ngp_rpr::ft_cut_plot:
			{
				{ INGPUnknownPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,				INGP_CutPlot>::value)); }
				{ INGP_FeaturePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,				INGP_CutPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot,		INGP_CutPlot>::value)); }
				{ INGP_PlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot,					INGP_CutPlot>::value)); }
				{ INGP_ParametersPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot,		INGP_CutPlot>::value)); }
				{ INGP_PointsPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot,			INGP_CutPlot>::value)); }
				{ INGP_GeomPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GeomPlot,				INGP_CutPlot>::value)); }
				{ INGP_CutPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_CutPlot,				INGP_CutPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCADGeometry,		INGP_CutPlot>::value)); }
				{ INGP_UseSlidingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseSliding,			INGP_CutPlot>::value)); }
				{ INGP_UseSymmetryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseSymmetry,			INGP_CutPlot>::value)); }
				{ INGP_UseCroppingPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping,			INGP_CutPlot>::value)); }
				{ INGP_ChartPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,			INGP_CutPlot>::value)); }
				{ INGP_GoalsContainerPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,		INGP_CutPlot>::value)); }
				{ INGP_FTPIBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase,			INGP_CutPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i; feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance,	INGP_CutPlot>::value)); }
				{ INGP_ParticleBasePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase,		INGP_CutPlot>::value)); }
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,			INGP_CutPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseInterpolateResults,INGP_CutPlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_DisplayBoundaryLayer,	INGP_CutPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_surf_plot:
			{
				{ INGPUnknownPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,				INGP_SurfacePlot>::value)); }
				{ INGP_FeaturePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,				INGP_SurfacePlot>::value)); }
				{ INGP_GraphicalPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot,		INGP_SurfacePlot>::value)); }
				{ INGP_PlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot,					INGP_SurfacePlot>::value)); }
				{ INGP_ParametersPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot,		INGP_SurfacePlot>::value)); }
				{ INGP_PointsPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot,			INGP_SurfacePlot>::value)); }
				{ INGP_GeomPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GeomPlot,				INGP_SurfacePlot>::value)); }
				{ INGP_SurfacePlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_SurfacePlot,			INGP_SurfacePlot>::value)); }
				{ INGP_UseCADGeometryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCADGeometry,		INGP_SurfacePlot>::value)); }
				{ INGP_UseSlidingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseSliding,			INGP_SurfacePlot>::value)); }
				{ INGP_UseSymmetryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseSymmetry,			INGP_SurfacePlot>::value)); }
				{ INGP_UseCroppingPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping,			INGP_SurfacePlot>::value)); }
				{ INGP_ChartPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,			INGP_SurfacePlot>::value)); }
				{ INGP_GoalsContainerPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,		INGP_SurfacePlot>::value)); }
				{ INGP_FTPIBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase,			INGP_SurfacePlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i; feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance,	INGP_SurfacePlot>::value)); }
				{ INGP_ParticleBasePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase,		INGP_SurfacePlot>::value)); }
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_SurfacePlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseInterpolateResults,INGP_SurfacePlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,	INGP_SurfacePlot>::value)); }
				break;
			}
		case ngp_rpr::ft_isosurface:
			{
				{ INGPUnknownPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown, INGP_IsosurfacePlot>::value)); }
				{ INGP_FeaturePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature, INGP_IsosurfacePlot>::value)); }
				{ INGP_GraphicalPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot, INGP_IsosurfacePlot>::value)); }
				{ INGP_PlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_Plot, INGP_IsosurfacePlot>::value)); }
				{ INGP_ParametersPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot, INGP_IsosurfacePlot>::value)); }
				{ INGP_PointsPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot, INGP_IsosurfacePlot>::value)); }
				{ INGP_GeomPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot, INGP_IsosurfacePlot>::value)); }
				{ INGP_IsosurfacePlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_IsosurfacePlot, INGP_IsosurfacePlot>::value)); }
				{ INGP_UseCADGeometryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCADGeometry, INGP_IsosurfacePlot>::value)); }
				{ INGP_UseSlidingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding, INGP_IsosurfacePlot>::value)); }
				{ INGP_UseSymmetryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry, INGP_IsosurfacePlot>::value)); }
				{ INGP_UseCroppingPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping, INGP_IsosurfacePlot>::value)); }
				{ INGP_ChartPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot, INGP_IsosurfacePlot>::value)); }
				{ INGP_GoalsContainerPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer, INGP_IsosurfacePlot>::value)); }
				{ INGP_FTPIBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase, INGP_IsosurfacePlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i; feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance, INGP_IsosurfacePlot>::value)); }
				{ INGP_ParticleBasePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase, INGP_IsosurfacePlot>::value)); }
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,	INGP_IsosurfacePlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i; feat->QueryInterface(&i);BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseInterpolateResults,INGP_IsosurfacePlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,INGP_IsosurfacePlot>::value)); }
				break;
			}
		case ngp_rpr::ft_goals_plot:
			{
				{ INGPUnknownPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown, INGP_GoalPlot>::value)); }
				{ INGP_FeaturePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature, INGP_GoalPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot, INGP_GoalPlot>::value)); }
				{ INGP_PlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_Plot, INGP_GoalPlot>::value)); }
				{ INGP_ParametersPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot, INGP_GoalPlot>::value)); }
				{ INGP_PointsPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot, INGP_GoalPlot>::value)); }
				{ INGP_GeomPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot, INGP_GoalPlot>::value)); }
				{ INGP_GoalPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GoalPlot, INGP_GoalPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCADGeometry, INGP_GoalPlot>::value)); }
				{ INGP_UseSlidingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding, INGP_GoalPlot>::value)); }
				{ INGP_UseSymmetryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry, INGP_GoalPlot>::value)); }
				{ INGP_UseCroppingPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCropping, INGP_GoalPlot>::value)); }
				{ INGP_ChartPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ChartPlot, INGP_GoalPlot>::value)); }
				{ INGP_GoalsContainerPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GoalsContainer, INGP_GoalPlot>::value)); }
				{ INGP_FTPIBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase, INGP_GoalPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i; feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance, INGP_GoalPlot>::value)); }
				{ INGP_ParticleBasePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase, INGP_GoalPlot>::value)); }
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,	INGP_GoalPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);BOOST_CHECK(!(std::is_base_of<INGP_UseInterpolateResults,INGP_GoalPlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,INGP_GoalPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_surf_params:
		case ngp_rpr::ft_volume_params:
			{
				{ INGPUnknownPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,					INGP_SurfaceParametersPlot>::value)); }
				{ INGP_FeaturePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,					INGP_SurfaceParametersPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot,			INGP_SurfaceParametersPlot>::value)); }
				{ INGP_PlotPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot,						INGP_SurfaceParametersPlot>::value)); }
				{ INGP_ParametersPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ParametersPlot,			INGP_SurfaceParametersPlot>::value)); }
				{ INGP_PointsPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_GeomPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_SurfaceParametersPlotPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_SurfaceParametersPlot,	INGP_SurfaceParametersPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCADGeometry,			INGP_SurfaceParametersPlot>::value)); }
				{ INGP_UseSlidingPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_UseSymmetryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseSymmetry,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_UseCroppingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCropping,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_ChartPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_GoalsContainerPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,			INGP_SurfaceParametersPlot>::value)); }
				{ INGP_FTPIBasePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance,		INGP_SurfaceParametersPlot>::value)); }
				{ INGP_ParticleBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase,			INGP_SurfaceParametersPlot>::value)); }
				{ INGP_UseAllFacesPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,				INGP_SurfaceParametersPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseInterpolateResults,	INGP_SurfaceParametersPlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,	INGP_SurfaceParametersPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_point_param:
			{
				{ INGPUnknownPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown, INGP_PointParametersPlot>::value)); }
				{ INGP_FeaturePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature, INGP_PointParametersPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GraphicalPlot, INGP_PointParametersPlot>::value)); }
				{ INGP_PlotPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot, INGP_PointParametersPlot>::value)); }
				{ INGP_ParametersPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ParametersPlot, INGP_PointParametersPlot>::value)); }
				{ INGP_PointsPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_PointsPlot, INGP_PointParametersPlot>::value)); }
				{ INGP_GeomPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot, INGP_PointParametersPlot>::value)); }
				{ INGP_PointParametersPlotPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_PointParametersPlot, INGP_PointParametersPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCADGeometry, INGP_PointParametersPlot>::value)); }
				{ INGP_UseSlidingPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseSliding, INGP_PointParametersPlot>::value)); }
				{ INGP_UseSymmetryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry, INGP_PointParametersPlot>::value)); }
				{ INGP_UseCroppingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCropping, INGP_PointParametersPlot>::value)); }
				{ INGP_ChartPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot, INGP_PointParametersPlot>::value)); }
				{ INGP_GoalsContainerPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer, INGP_PointParametersPlot>::value)); }
				{ INGP_FTPIBasePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase, INGP_PointParametersPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance, INGP_PointParametersPlot>::value)); }
				{ INGP_ParticleBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase, INGP_PointParametersPlot>::value)); }
				{ INGP_UseAllFacesPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,	INGP_PointParametersPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseInterpolateResults,INGP_PointParametersPlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,INGP_PointParametersPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_xy_plot:
			{
				{ INGPUnknownPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown, INGP_XYPlot>::value)); }
				{ INGP_FeaturePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature, INGP_XYPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot, INGP_XYPlot>::value)); }
				{ INGP_PlotPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot, INGP_XYPlot>::value)); }
				{ INGP_ParametersPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ParametersPlot, INGP_XYPlot>::value)); }
				{ INGP_PointsPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot, INGP_XYPlot>::value)); }
				{ INGP_GeomPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot, INGP_XYPlot>::value)); }
				{ INGP_XYPlotPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_XYPlot, INGP_XYPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCADGeometry, INGP_XYPlot>::value)); }
				{ INGP_UseSlidingPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseSliding, INGP_XYPlot>::value)); }
				{ INGP_UseSymmetryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry, INGP_XYPlot>::value)); }
				{ INGP_UseCroppingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCropping, INGP_XYPlot>::value)); }
				{ INGP_ChartPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ChartPlot, INGP_XYPlot>::value)); }
				{ INGP_GoalsContainerPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer, INGP_XYPlot>::value)); }
				{ INGP_FTPIBasePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase, INGP_XYPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance, INGP_XYPlot>::value)); }
				{ INGP_ParticleBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase, INGP_XYPlot>::value)); }
				{ INGP_UseAllFacesPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,	INGP_XYPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseInterpolateResults,INGP_XYPlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_DisplayBoundaryLayer,	INGP_XYPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_flow_trajectrories:
			{
				{ INGPUnknownPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,				INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_FeaturePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,				INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot,		INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_PlotPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot,					INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_ParametersPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot,		INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_PointsPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_PointsPlot,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_GeomPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_FlowTrajectoriesPlotPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_FlowTrajectoriesPlot, INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCADGeometry,		INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_UseSlidingPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_UseSymmetryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_UseCroppingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_ChartPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_GoalsContainerPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,		INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_FTPIBasePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_FTPIBase,				INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_FTPIBaseAppearance,	INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_ParticleBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ParticleBase,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_UseAllFacesPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,			INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseInterpolateResults,INGP_FlowTrajectoriesPlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,INGP_FlowTrajectoriesPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_particle_injection:
			{
				{ INGPUnknownPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,				INGP_ParticleInjection>::value)); }
				{ INGP_FeaturePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,				INGP_ParticleInjection>::value)); }
				{ INGP_GraphicalPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot,		INGP_ParticleInjection>::value)); }
				{ INGP_PlotPtr i;					feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot,					INGP_ParticleInjection>::value)); }
				{ INGP_ParametersPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot,		INGP_ParticleInjection>::value)); }
				{ INGP_PointsPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_PointsPlot,			INGP_ParticleInjection>::value)); }
				{ INGP_GeomPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot,			INGP_ParticleInjection>::value)); }
				{ INGP_ParticleInjectionPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ParticleInjection,	INGP_ParticleInjection>::value)); }
				{ INGP_UseCADGeometryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCADGeometry,		INGP_ParticleInjection>::value)); }
				{ INGP_UseSlidingPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding,			INGP_ParticleInjection>::value)); }
				{ INGP_UseSymmetryPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry,			INGP_ParticleInjection>::value)); }
				{ INGP_UseCroppingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping,			INGP_ParticleInjection>::value)); }
				{ INGP_ChartPlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,			INGP_ParticleInjection>::value)); }
				{ INGP_GoalsContainerPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,		INGP_ParticleInjection>::value)); }
				{ INGP_FTPIBasePtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_FTPIBase,				INGP_ParticleInjection>::value)); }
				{ INGP_FTPIBaseAppearancePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_FTPIBaseAppearance,	INGP_ParticleInjection>::value)); }
				{ INGP_ParticleBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase,		INGP_ParticleInjection>::value)); }
				{ INGP_UseAllFacesPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,			INGP_ParticleInjection>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseInterpolateResults,INGP_ParticleInjection>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;	feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,INGP_ParticleInjection>::value)); }
				break;
			}
		case ngp_rpr::ft_particle_study:
			{
				{ INGPUnknownPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,				INGP_ParticleStudy>::value)); }
				{ INGP_FeaturePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,				INGP_ParticleStudy>::value)); }
				{ INGP_GraphicalPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GraphicalPlot,		INGP_ParticleStudy>::value)); }
				{ INGP_PlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_Plot,				INGP_ParticleStudy>::value)); }
				{ INGP_ParametersPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot,		INGP_ParticleStudy>::value)); }
				{ INGP_PointsPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot,			INGP_ParticleStudy>::value)); }
				{ INGP_GeomPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot,			INGP_ParticleStudy>::value)); }
				{ INGP_ParticleStudyPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ParticleStudy,		INGP_ParticleStudy>::value)); }
				{ INGP_UseCADGeometryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCADGeometry,		INGP_ParticleStudy>::value)); }
				{ INGP_UseSlidingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding,			INGP_ParticleStudy>::value)); }
				{ INGP_UseSymmetryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry,			INGP_ParticleStudy>::value)); }
				{ INGP_UseCroppingPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping,			INGP_ParticleStudy>::value)); }
				{ INGP_ChartPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,			INGP_ParticleStudy>::value)); }
				{ INGP_GoalsContainerPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,		INGP_ParticleStudy>::value)); }
				{ INGP_FTPIBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase,			INGP_ParticleStudy>::value)); }
				{ INGP_FTPIBaseAppearancePtr i; feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance,	INGP_ParticleStudy>::value)); }
				{ INGP_ParticleBasePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_ParticleBase,			INGP_ParticleStudy>::value)); }
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,			INGP_ParticleStudy>::value)); }
				{ INGP_UseInterpolateResultsPtr i;feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseInterpolateResults,INGP_ParticleStudy>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,INGP_ParticleStudy>::value)); }
				break;
			}
		case ngp_rpr::ft_particle_bound_cond:
			break;
		case ngp_rpr::ft_mesh_plot:
			{
				{ INGPUnknownPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,				INGP_MeshPlot>::value)); }
				{ INGP_FeaturePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,				INGP_MeshPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot,		INGP_MeshPlot>::value)); }
				{ INGP_PlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot,				INGP_MeshPlot>::value)); }
				{ INGP_ParametersPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot,		INGP_MeshPlot>::value)); }
				{ INGP_PointsPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot,			INGP_MeshPlot>::value)); }
				{ INGP_GeomPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot,			INGP_MeshPlot>::value)); }
				{ INGP_MeshPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_MeshPlot,				INGP_MeshPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCADGeometry,		INGP_MeshPlot>::value)); }
				{ INGP_UseSlidingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding,			INGP_MeshPlot>::value)); }
				{ INGP_UseSymmetryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry,			INGP_MeshPlot>::value)); }
				{ INGP_UseCroppingPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping,			INGP_MeshPlot>::value)); }
				{ INGP_ChartPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,			INGP_MeshPlot>::value)); }
				{ INGP_GoalsContainerPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,		INGP_MeshPlot>::value)); }
				{ INGP_FTPIBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase,			INGP_MeshPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i; feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBaseAppearance,	INGP_MeshPlot>::value)); }
				{ INGP_ParticleBasePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase,		INGP_MeshPlot>::value)); }
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_MeshPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseInterpolateResults,INGP_MeshPlot>::value)); }
				{ INGP_DisplayBoundaryLayerPtr i;feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_DisplayBoundaryLayer,INGP_MeshPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_global_min_max:
			break;
		case ngp_rpr::ft_ref_geom_tess_cut_plot:
			break;
		case ngp_rpr::ft_ref_geom_tess_surf_plot:
			break;
		case ngp_rpr::ft_cut_plot_matrix:
			break;
		case ngp_rpr::ft_surf_plot_matrix:
			break;
		case ngp_rpr::ft_xy_plot_geometry:
			break;
		case ngp_rpr::ft_points_define:
			break;
		case ngp_rpr::ft_ray_plot:
			{
				{ INGPUnknownPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGPUnknown,				INGP_RayPlot>::value)); }
				{ INGP_FeaturePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Feature,				INGP_RayPlot>::value)); }
				{ INGP_GraphicalPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_GraphicalPlot,		INGP_RayPlot>::value)); }
				{ INGP_PlotPtr i;				feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_Plot,					INGP_RayPlot>::value)); }
				{ INGP_ParametersPlotPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParametersPlot,		INGP_RayPlot>::value)); }
				{ INGP_PointsPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_PointsPlot,			INGP_RayPlot>::value)); }
				{ INGP_GeomPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GeomPlot,			INGP_RayPlot>::value)); }
				{ INGP_RayPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_RayPlot,				INGP_RayPlot>::value)); }
				{ INGP_UseCADGeometryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseCADGeometry,		INGP_RayPlot>::value)); }
				{ INGP_UseSlidingPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSliding,			INGP_RayPlot>::value)); }
				{ INGP_UseSymmetryPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseSymmetry,			INGP_RayPlot>::value)); }
				{ INGP_UseCroppingPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseCropping,			INGP_RayPlot>::value)); }
				{ INGP_ChartPlotPtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ChartPlot,			INGP_RayPlot>::value)); }
				{ INGP_GoalsContainerPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_GoalsContainer,		INGP_RayPlot>::value)); }
				{ INGP_FTPIBasePtr i;			feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_FTPIBase,				INGP_RayPlot>::value)); }
				{ INGP_FTPIBaseAppearancePtr i; feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_FTPIBaseAppearance,	INGP_RayPlot>::value)); }
				{ INGP_ParticleBasePtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_ParticleBase,		INGP_RayPlot>::value)); }
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseAllFaces,			INGP_RayPlot>::value)); }
				{ INGP_UseInterpolateResultsPtr i;	feat->QueryInterface(&i);BOOST_CHECK(!i);	BOOST_CHECK(!(std::is_base_of<INGP_UseInterpolateResults,INGP_RayPlot>::value)); }
				break;
			}
		case ngp_rpr::ft_ansys_export:
			{
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_AnsysExport>::value)); }
				break;
			}
		case ngp_rpr::ft_mesh_text_export:
			{
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_MeshTextExport>::value)); }
				break;
			}
		case ngp_rpr::ft_nastran_export:
			{
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_NastranExport>::value)); }
				break;
			}
		case ngp_rpr::ft_mechanica_export:
			{
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_MechanicaExport>::value)); }
				break;
			}
		case ngp_rpr::ft_abaqus_export:
			{
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_AbaqusExport>::value)); }
				break;
			}
		case ngp_rpr::ft_ensight_export:
			break;
		case ngp_rpr::ft_mpcci_export:
			{
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_MpcciExport>::value)); }
				break;
			}
		case ngp_rpr::ft_paraview_export:
			break;
		case ngp_rpr::ft_xy_plot_matrix:
			break;
		case ngp_rpr::ft_point_param_matrix:
			break;
		case ngp_rpr::ft_noise_prediction:
			break;
		case ngp_rpr::ft_flux_balance:
			{
				{ INGP_UseAllFacesPtr i;		feat->QueryInterface(&i);	BOOST_CHECK(i);		BOOST_CHECK((std::is_base_of<INGP_UseAllFaces,			INGP_FluxBalance>::value)); }
				break;	
			}
		case ngp_rpr::ft_scene:
			break;
		default:
			break;
		}
	}
}
