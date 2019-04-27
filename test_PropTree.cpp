#include "stdafx.h"
#include <boost/test/unit_test.hpp>

//#include <boost/property_tree/json_parser.hpp>
#include "prop_ref_coll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum TestEnum { e_item1, e_item2, e_item3, e_item4, e_item5 };

//////////////////////////////////////////////////////////////////////////

// BOOST_AUTO_TEST_CASE(raw_WPTree_test)
// {
// 	boost::property_tree::wptree tree;
// 	boost::property_tree::wptree& root = tree.put( L"root", L"this is root" );
// 
// 	{
// 		boost::property_tree::wptree& item1 = root.add( L"array_item", L"" );
// 		item1.put( L"name", L"item1" );
// 		item1.put( L"type", 2 );
// 	}
// 	{
// 		boost::property_tree::wptree& item2 = root.add( L"array_item", L"" );
// 		item2.put( L"name", L"item1(2)" );
// 
// 		boost::property_tree::wptree item22;
// 		item22.put_value( 2 );
// 		item2.put_child( L"type", item22 );
// 	}
// 
// 	boost::property_tree::write_xml( "_raw_WPTree_test.xml", tree );
// }

//////////////////////////////////////////////////////////////////////////

#define STANDARD_PROPERTY_TEST(p1,p2,p3)		\
		BOOST_CHECK( p1.IsEqualTo( p2 ) );		\
		BOOST_CHECK( !p2.IsEqualTo( p3 ) );		\
		p2.CopyFrom( p3 );						\
		BOOST_CHECK( p2.IsEqualTo( p3 ) );		\
		boost::property_tree::wptree pt;		\
		p2.SaveToPTree( pt );					\
		p2.CopyFrom( p1 );						\
		p2.LoadFromPTree( pt );					\
		BOOST_CHECK( p2.IsEqualTo( p3 ) );		\
		ngp_file_io::INGPFileIOPtr bin_file = ngp_file_io::CreateFileIO();	\
		bin_file->open((test_dir / L"_bin_file.bin").wstring().c_str(), L"wb");		\
		p2.SaveToBinFile(bin_file);											\
		bin_file->close();													\
		p2.CopyFrom(p1);													\
		bin_file->open((test_dir / L"_bin_file.bin").wstring().c_str(), L"rb");							\
		p2.LoadFromBinFile(bin_file);										\
		BOOST_CHECK( p2.IsEqualTo( p3 ) );

std::wstring utf16_string(std::string&& utf8_string)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(utf8_string);
}

GeomRef rand_geom_ref()
{
	const char* name_base[] = { "ref", "референс" };
	const FWPostGeomReferenceType types[] = { fwPostGeomReferenceFace, fwPostGeomReferencePlane };

	GeomRef gr;
	gr.uuid.Format( L"id%d", ut_utils::rand() );
	gr.name.Format( L"%s%d", utf16_string(name_base[ut_utils::rand(2)]).c_str(), ut_utils::rand() );
	gr.type = types[ut_utils::rand(2)];

	return gr;
}

void randomize_Palette( IPlotPalette* pal )
{
	pal->SetParameter( ut_utils::rand_string(L"Param") );
	pal->SetParamMin( PARAM_MIN_MAX_VALUE(ut_utils::rand(), false, false) );
	pal->SetParamMax( PARAM_MIN_MAX_VALUE(ut_utils::rand(), false, true) );
	pal->SetCoordFrame( ut_utils::rand_string(L"CoordFrameName"), ut_utils::rand_string(L"CoordFrameUUID"), CNGPCoordinateSystem() );
	pal->SetLevelsNumber( ut_utils::rand(100) );
}

void randomize_FTPIBase( CFTPIBaseAppearance& app )
{
	randomize_Palette( app.GetTrajsColorizationPalette() );
	app.SetColorByParam( ut_utils::rand_bool() );
	app.SetColor( ut_utils::rand() );
}

void randomize( const CParticleInjectionPtr& inj )
{
	randomize_FTPIBase( *inj );
	inj->SetMaterialInfo( ut_utils::rand_string(L"Material"), FwEngineeringDBContentTypes( int(1 << (ut_utils::rand(10))) ) );
	inj->SetVelocityType( FWParticleICVelocityType( ut_utils::rand(2) ) );
	inj->SetTemperatureType( FWParticleICTemperatureType( ut_utils::rand(2) ) );
}

void randomize( const CParticleStudyPtr& ps )
{
	ps->SetMaxLength( ut_utils::rand(500) );
	ps->SetMaxTstep( ut_utils::rand(100) );
	ps->SetEnableGravity( ut_utils::rand_bool() );
	ps->SetAccretionRate( ut_utils::rand_bool() );
}

bool explicitly_equal( const IPlotPalette* p1, const IPlotPalette* p2 )
{
	if ( p1->GetVisParameter() != p2->GetVisParameter() )
		return false;
	PARAM_MIN_MAX_VALUE mm1, mm2;
	p1->GetParamMin( mm1 );
	p2->GetParamMin( mm2 );
	if ( mm1 != mm2 )
		return false;

	if ( p1->GetCoordFrameUUID() != p2->GetCoordFrameUUID() )
		return false;
// 	if ( p1->GetCoordFrameName() != p2->GetCoordFrameName() )
// 		return false;
	if ( p1->IsScaleLogarithmic() != p2->IsScaleLogarithmic() )
		return false;
	if ( p1->GetColorsNumber() != p2->GetColorsNumber() )
		return false;
	if ( p1->GetColoringScheme() != p2->GetColoringScheme() )
		return false;
	if ( p1->GetOuterMinMaxColoring() != p2->GetOuterMinMaxColoring() )
		return false;
	if ( p1->GetOuterMinColor() != p2->GetOuterMinColor() )
		return false;
	if ( p1->GetOuterMaxColor() != p2->GetOuterMaxColor() )
		return false;
	if ( p1->GetDisplayOuterColors() != p2->GetDisplayOuterColors() )
		return false;
	if ( p1->GetColorbarOrientation() != p2->GetColorbarOrientation() )
		return false;
	if ( p1->GetRotatedText() != p2->GetRotatedText() )
		return false;
	if ( p1->GetDisplayTitle() != p2->GetDisplayTitle() )
		return false;
	if ( p1->GetDisplayPlotNames() != p2->GetDisplayPlotNames() )
		return false;
	if ( p1->GetDisplayPlotsMinMaxes() != p2->GetDisplayPlotsMinMaxes() )
		return false;
	if ( p1->GetUseDefaultFont() != p2->GetUseDefaultFont() )
		return false;
	CFontDef f1, f2;
	p1->GetFont( f1 );
	p2->GetFont( f2 );
	if ( f1 != f2 )
		return false;
	if ( p1->GetAutoScaleFont() != p2->GetAutoScaleFont() )
		return false;
	if ( p1->GetBackgroundColor() != p2->GetBackgroundColor() )
		return false;
	if ( p1->GetBackgroundTransparency() != p2->GetBackgroundTransparency() )
		return false;
	if ( p1->GetWidth() != p2->GetWidth() )
		return false;
	if ( p1->GetAutoNumberOfTicks() != p2->GetAutoNumberOfTicks() )
		return false;
	if ( p1->GetNumberOfTicks() != p2->GetNumberOfTicks() )
		return false;

	return true;
}

bool explicitly_equal( const CParticleInjection& inj1, const CParticleInjection& inj2 )
{
	// INGP_FTPIBaseAppearance
	if ( !explicitly_equal( const_cast<CParticleInjection&>(inj1).GetTrajsColorizationPalette(), const_cast<CParticleInjection&>(inj2).GetTrajsColorizationPalette() ) )
		return false;
	if ( inj1.GetDisplayStyle() != inj2.GetDisplayStyle() )
		return false;
	if ( inj1.GetLineWidth() != inj2.GetLineWidth() )
		return false;
	if ( inj1.GetCrossSize() != inj2.GetCrossSize() )
		return false;
	if ( inj1.GetColor() != inj2.GetColor() )
		return false;
	if ( inj1.GetColorByParam() != inj2.GetColorByParam() )
		return false;
	if ( inj1.GetFTType() != inj2.GetFTType() )
		return false;
	if ( inj1.GetDynTrajStyle() != inj2.GetDynTrajStyle() )
		return false;
	if ( inj1.GetDynTrajWidth() != inj2.GetDynTrajWidth() )
		return false;
	if ( inj1.GetDynTrajSpacing() != inj2.GetDynTrajSpacing() )
		return false;
	
	// INGP_ParticleInjection
	if ( inj1.GetMaterial() != inj2.GetMaterial() )
		return false;
	if ( inj1.GetMaterialType() != inj2.GetMaterialType() )
		return false;
	if ( inj1.GetVelocityType() != inj2.GetVelocityType() )
		return false;
	if ( inj1.GetTemperatureType() != inj2.GetTemperatureType() )
		return false;

	return true;
}

bool explicitly_equal( const CParticleStudy& ps1, const CParticleStudy& ps2 )
{
	if ( ps1.GetInjections().size() != ps2.GetInjections().size() )
		return false;
	if ( ps1.IsParticle() != ps2.IsParticle() )
		return false;
	if ( ps1.IsGravity() != ps2.IsGravity() )
		return false;
	if ( ps1.IsAccretionRate() != ps2.IsAccretionRate() )
		return false;
	if ( ps1.IsErosionRate() != ps2.IsErosionRate() )
		return false;

	// INGP_ParticleBase
	if ( ps1.GetRelativeToAbsFrame() != ps2.GetRelativeToAbsFrame() )
		return false;
	if ( ps1.GetMaxLength() != ps2.GetMaxLength() )
		return false;
	if ( ps1.GetMaxTime() != ps2.GetMaxTime() )
		return false;
	if ( ps1.GetMaxTstep() != ps2.GetMaxTstep() )
		return false;
	if ( ps1.GetDirection() != ps2.GetDirection() )
		return false;
	if ( ps1.GetFlowTrajType() != ps2.GetFlowTrajType() )
		return false;
	if ( ps1.IsSaveToASCII() != ps2.IsSaveToASCII() )
		return false;
	if ( ps1.GetASCIIFile() != ps2.GetASCIIFile() )
		return false;

	return true;
}

bool explicitly_equal( const CFeaturePtr& f1, const CFeaturePtr& f2 )
{
	if ( f1->GetType() != f2->GetType() )
		return false;

	switch ( f1->GetType() )
	{
		case ft_particle_study:
			return explicitly_equal( static_cast<const CParticleStudy&>(*f1), static_cast<const CParticleStudy&>(*f2) );
		case ft_particle_injection:
			return explicitly_equal( static_cast<const CParticleInjection&>(*f1), static_cast<const CParticleInjection&>(*f2) );
	}
	return true;
}

#define CHECK_EQUAL_FEATURES( f1, f2 ) \
	BOOST_CHECK_MESSAGE( *f1==*f2,                ut_utils::to_string(f1->GetType()) ); \
	BOOST_CHECK_MESSAGE( explicitly_equal(f1,f2), ut_utils::to_string(f1->GetType()) );

//////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test_PropertiesSeparately)
{
	const fs::path test_dir = fs::current_path() / L"test_dir";
	BOOST_CHECK(filesystem_utils::create_directory(test_dir));

	// CPropertySimple<> (PROPERTY_SIMPLE_REF)
	{
		const int i1 = 1;
		int i2 = 1;
		int i3 = 2;

		CPropertySimple<int> p1( &i1, L"p1" );
		CPropertySimple<int> p2( &i2, L"p2" );
		CPropertySimple<int> p3( &i3, L"p3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}
	{
		const double d1 = 1.1111;
		double d2 = 1.1111 - EPS_10/10.;
		BOOST_CHECK(CMathUtils::AreEqual10(d1, d2));

		double d3 = 1.1111 + EPS_10;
		BOOST_CHECK(!CMathUtils::AreEqual10(d1, d3));

		CPropertySimple<double> p1( &d1, L"p1" );
		CPropertySimple<double> p2( &d2, L"p2" );
		CPropertySimple<double> p3( &d3, L"p3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}
	{
		const CNGPString s1 = L"abc";
		CNGPString s2 = L"abc";
		CNGPString s3 = L"hdfghdf";

		CPropertySimple<CNGPString> p1( &s1, L"p1" );
		CPropertySimple<CNGPString> p2( &s2, L"p2" );
		CPropertySimple<CNGPString> p3( &s3, L"p3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}

	// CPropertySimpleArr<> (PROPERTY_SIMPLEARR_REF)
	{
		const int size = 3;
		const int a1[size] = { 1, 2, 3 };
		int a2[size] = { 1, 2, 3 };
		int a3[size] = { 21, 22, 23 };

		CPropertySimpleArr<int> p1( a1, size, L"a1" );
		CPropertySimpleArr<int> p2( a2, size, L"a2" );
		CPropertySimpleArr<int> p3( a3, size, L"a3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}

	// CPropertySetOfEnum<> (PROPERTY_SETOFENUM_REF)
	{
		std::set<TestEnum> es1, es2, es3;
		es1.insert( e_item1 );		es1.insert( e_item2 );		es1.insert( e_item3 );
		es2.insert( e_item1 );		es2.insert( e_item2 );		es2.insert( e_item3 );
		es3.insert( e_item3 );		es3.insert( e_item4 );		es3.insert( e_item5 );

		CPropertySetOfEnum<TestEnum> p1( &es1, L"a1" );
		CPropertySetOfEnum<TestEnum> p2( &es2, L"a2" );
		CPropertySetOfEnum<TestEnum> p3( &es3, L"a3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}

	{
		ngp_shared_array<TestEnum> es1, es2, es3;
		es1.push_back( e_item1 );		es1.push_back( e_item2 );		es1.push_back( e_item3 );
		es2.push_back( e_item1 );		es2.push_back( e_item2 );		es2.push_back( e_item3 );
		es3.push_back( e_item3 );		es3.push_back( e_item4 );		es3.push_back( e_item5 );

		CPropertyContainerOfEnum<ngp_shared_array<TestEnum>> p1( &es1, L"a1" );
		CPropertyContainerOfEnum<ngp_shared_array<TestEnum>> p2( &es2, L"a2" );
		CPropertyContainerOfEnum<ngp_shared_array<TestEnum>> p3( &es3, L"a3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}
	{
		{
			GeomRef gr1; gr1.uuid = L"id1"; gr1.name = L"ref1"; gr1.type = fwPostGeomReferencePlane;
			GeomRef gr2; gr2.uuid = L"id2"; gr2.name = L"ref2"; gr2.type = fwPostGeomReferenceFace;
			GeomRef gr3; gr3.uuid = L"id3"; gr3.name = L"ref3"; gr3.type = fwPostGeomReferencePoint;

			std::list<GeomRef> l1, l2, l3;
			l1.push_back(gr1);		l1.push_back(gr2);
			l2.push_back(gr1);		l2.push_back(gr2);
			l3.push_back(gr3);		l3.push_back(gr3);

			CGeomRef::DECOMPOSITOR decompositor;
			CPropertyContainerOf<std::list<GeomRef>> p1( &l1, L"p1", decompositor );
			CPropertyContainerOf<std::list<GeomRef>> p2( &l2, L"p2", decompositor );
			CPropertyContainerOf<std::list<GeomRef>> p3( &l3, L"p3", decompositor );

			STANDARD_PROPERTY_TEST( p1, p2, p3 );
		}

		{
			std::vector<Point> points1(10); points1[5].set_media_e(fwPostMediaFluidSolid);
			std::vector<Point> points2(10); points2[5].set_media_e(fwPostMediaFluidSolid);
			std::vector<Point> points3(10); points3[0].set_media_e(fwPostMediaFluidSolid);

			auto decompositor = boost::bind( &XML_serialization_helpers::AddRPRPoint, _1, _2, _3 );
			CPropertyContainerOf<std::vector<Point>> p1( &points1, L"p1", decompositor );
			CPropertyContainerOf<std::vector<Point>> p2( &points2, L"p2", decompositor );
			CPropertyContainerOf<std::vector<Point>> p3( &points3, L"p3", decompositor );
			STANDARD_PROPERTY_TEST( p1, p2, p3 );
		}
	}

	{
		{
            std::list<std::wstring> c1 = { L"str1", L"str2", L"str3" };
            std::list<std::wstring> c2 = { L"str1", L"str2", L"str3" };
            std::list<std::wstring> c3 = { L"str7", L"str8", L"str9" };
			CPropertyContainerOfSimple<std::list<std::wstring>> p1 (&c1, L"tag");
			CPropertyContainerOfSimple<std::list<std::wstring>> p2 (&c2, L"tag");
			CPropertyContainerOfSimple<std::list<std::wstring>> p3 (&c3, L"tag");
			STANDARD_PROPERTY_TEST(p1, p2, p3);
		}
		{
            std::vector<std::wstring> c1 = { L"str1", L"str2", L"str3" };
            std::vector<std::wstring> c2 = { L"str1", L"str2", L"str3" };
            std::vector<std::wstring> c3 = { L"str7", L"str8", L"str9" };
			CPropertyContainerOfSimple<std::vector<std::wstring>> p1 (&c1, L"tag");
			CPropertyContainerOfSimple<std::vector<std::wstring>> p2 (&c2, L"tag");
			CPropertyContainerOfSimple<std::vector<std::wstring>> p3 (&c3, L"tag");
			STANDARD_PROPERTY_TEST(p1, p2, p3);
		}
		{
            std::vector<CNGPString> v1 = { CNGPString(L"str1"), CNGPString(L"str2"), CNGPString(L"str3") };
            std::vector<CNGPString> v2 = { CNGPString(L"str1"), CNGPString(L"str2"), CNGPString(L"str3") };
            std::vector<CNGPString> v3 = { CNGPString(L"str7"), CNGPString(L"str8"), CNGPString(L"str9") };
			CPropertyContainerOfSimple<std::vector<CNGPString>> p1 (&v1, L"tag");
			CPropertyContainerOfSimple<std::vector<CNGPString>> p2 (&v2, L"tag");
			CPropertyContainerOfSimple<std::vector<CNGPString>> p3 (&v3, L"tag");
			STANDARD_PROPERTY_TEST(p1, p2, p3);
		}
		{
            std::vector<long> c1 = { 1, 2, 3 } ;
            std::vector<long> c2 = { 1, 2, 3 };
            std::vector<long> c3 = { 3, 2, 1 };
			CPropertyContainerOfSimple<std::vector<long>> p1 (&c1, L"tag");
			CPropertyContainerOfSimple<std::vector<long>> p2 (&c2, L"tag");
			CPropertyContainerOfSimple<std::vector<long>> p3 (&c3, L"tag");
			STANDARD_PROPERTY_TEST(p1, p2, p3);
		}
		{
            std::list<double> c1 = { 1, 2, 3 };
            std::list<double>       c2 = { 1.0, 2.0, 3.0 - EPS_10 / 10. };
            const std::list<double> c3 = { 1.0, 2.0, 3.0 + EPS_10 };
			CPropertyContainerOfSimple<std::list<double>> p1 (&c1, L"tag");
			CPropertyContainerOfSimple<std::list<double>> p2 (&c2, L"tag");
			CPropertyContainerOfSimple<std::list<double>> p3 (&c3, L"tag");
			STANDARD_PROPERTY_TEST(p1, p2, p3);
		}
	}
	{
		NGPNIK3DBox b1, b2, b3;
		for (int i=0; i<6; ++i)
		{
			b1._data[i] = i;
			b2._data[i] = double(i) - EPS_10/10.;
			b3._data[i] = double(i) + EPS_10;
		}

		CPropertySimpleArr<double> p1( b1._data, sizeof(b1)/sizeof(double), L"p1" );
		CPropertySimpleArr<double> p2( b2._data, sizeof(b1)/sizeof(double), L"p2" );
		CPropertySimpleArr<double> p3( b3._data, sizeof(b1)/sizeof(double), L"p3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}
	{
		const _property<bool> _p1 = true;
		_property<bool> _p2 = _p1;
		_property<bool> _p3 = false;
		bool b = _p3;

		CPropertySimple<bool> p1( &(_p1), L"p1" );
		CPropertySimple<bool> p2( &(_p2), L"p2" );
		CPropertySimple<bool> p3( &(_p3), L"p3" );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}

	// CPropertyMapOfStruct
	{
		const CFontDef fdArial(L"Arial",10,0,false,false);
		const CFontDef fdCourier(L"Courier",12,0,false,true);
		const CFontDef fdTahoma(L"Tahoma",12,0,false,false);
		typedef std::map<std::wstring,CFontDef> map_of_struct_t;
        map_of_struct_t mf1 = { {L"key1",fdArial}, {L"key2",fdCourier} };
		map_of_struct_t mf2 = mf1;
        map_of_struct_t mf3 = { {L"key2",fdCourier}, {L"key3",fdTahoma} };

		CPropertyMapOfStruct<map_of_struct_t> p1( &mf1, L"pm1", &XML_serialization_helpers::Add, [](const map_of_struct_t::key_type& key) { return key; }, [](const std::wstring& key) { return key; } );
		CPropertyMapOfStruct<map_of_struct_t> p2( &mf2, L"pm2", &XML_serialization_helpers::Add, [](const map_of_struct_t::key_type& key) { return key; }, [](const std::wstring& key) { return key; } );
		CPropertyMapOfStruct<map_of_struct_t> p3( &mf3, L"pm3", &XML_serialization_helpers::Add, [](const map_of_struct_t::key_type& key) { return key; }, [](const std::wstring& key) { return key; } );

		STANDARD_PROPERTY_TEST( p1, p2, p3 );
	}

	{
		const CFontDef fdArial(L"Arial", 10, 0, false, false);
		const CFontDef fdCourier(L"Courier", 12, 0, false, true);
		const CFontDef fdTahoma(L"Tahoma", 12, 0, false, false);
		typedef std::map<long, CFontDef> map_of_struct_t;
        map_of_struct_t mf1 = { {1, fdArial}, {1, fdCourier}, {0, fdCourier} };
		map_of_struct_t mf2 = mf1;
        map_of_struct_t mf3 = { {2, fdCourier}, {3, fdTahoma} };

		CPropertyMapOfStruct<map_of_struct_t> p1(&mf1, L"pm1", &XML_serialization_helpers::Add, [](const long& key) { return std::to_wstring(key); }, [](const std::wstring& key) { return std::stol(key); });
		CPropertyMapOfStruct<map_of_struct_t> p2(&mf2, L"pm2", &XML_serialization_helpers::Add, [](const long& key) { return std::to_wstring(key); }, [](const std::wstring& key) { return std::stol(key); });
		CPropertyMapOfStruct<map_of_struct_t> p3(&mf3, L"pm3", &XML_serialization_helpers::Add, [](const long& key) { return std::to_wstring(key); }, [](const std::wstring& key) { return std::stol(key); });

		STANDARD_PROPERTY_TEST(p1, p2, p3);
	}

	{
		class CClass
		{
		public:
			static boost::shared_ptr<CClass> Instance() {return boost::shared_ptr<CClass>(new CClass());}

			void Init(long v1, double v2) { m_1 = v1; m_2 = v2; }
			void Init(long v1, double v2, const std::wstring& key) { m_1 = v1; m_2 = v2; m_key = key; }
			void Init()
			{
				boost::shared_ptr<CClass> c1 = CClass::Instance(); c1->Init(1, 3.0);
				boost::shared_ptr<CClass> c2 = CClass::Instance(); c2->Init(2, 2.0);
				boost::shared_ptr<CClass> c3 = CClass::Instance(); c3->Init(3, 1.0);
                m_vector = { c1,c2,c3 };
                m_list = { c1,c2,c3 };
                m_map = { {L"1", c1}, {L"2", c2}, {L"3", c3} };
			}
			bool operator==(const CClass& a) { return m_1 == a.m_1 && m_2 == a.m_2 && m_key == a.m_key; }
			void operator=(const CClass& a) { m_1 = a.m_1; m_2 = a.m_2; m_key = a.m_key; }

			const std::wstring& GetKey() const { return m_key; }

			static const std::wstring& KeyGetter(const CClass& c) {
				return c.GetKey();
			}

			struct COPY_OPERATOR {
				void operator()(boost::shared_ptr<CClass>& l, const boost::shared_ptr<CClass>& r) const { *l = *r; }
			};

			struct DECOMPOSITOR
			{
				void operator()(const CClass& c, const wchar_ngp* tag, class CPropertyTreeNode& coll) const
				{
					CPropertyTreeNode& node = coll.AddSubGroup(tag);
					node.Add(PROPERTY_SIMPLE_REF(c.m_1, L"m_1", long));
					node.Add(PROPERTY_SIMPLE_REF(c.m_2, L"m_2", double));
					node.Add(PROPERTY_SIMPLE_REF(c.m_key, L"m_key", std::wstring));
					auto key_getter = boost::bind(&CClass::KeyGetter, _1);

					node.Add(PROPERTY_CONTAINER_OF_PTRS_REF(c.m_vector, L"vector", std::vector<boost::shared_ptr<CClass>>, COPY_OPERATOR, CClass::DECOMPOSITOR));
					node.Add(PROPERTY_CONTAINER_OF_PTRS_REF(c.m_list, L"list", std::list<boost::shared_ptr<CClass>>, COPY_OPERATOR, CClass::DECOMPOSITOR));
					typedef std::map<std::wstring, boost::shared_ptr<CClass>> MAP;

					DECOMPOSITOR decompositor;
					node.Add(PROPERTY_MAP_OF_PTRS_REF(c.m_map, L"map", MAP, COPY_OPERATOR, decompositor, key_getter, CClass::Instance));
				}
			};

		private:
			CClass():m_1(0), m_2(0.0) {}

			long m_1;
			double m_2;
			std::wstring m_key;

			std::vector<boost::shared_ptr<CClass>> m_vector;
			std::list<boost::shared_ptr<CClass>> m_list;
			std::map<std::wstring, boost::shared_ptr<CClass>> m_map;
		};

		// test std::vector of smart-pointers of CClass
		{
			std::vector<boost::shared_ptr<CClass>> ptrs_vector1; {
				boost::shared_ptr<CClass> c1 = CClass::Instance(); c1->Init(1, 3.0); c1->Init();
				boost::shared_ptr<CClass> c2 = CClass::Instance(); c2->Init(2, 2.0); c2->Init();
				boost::shared_ptr<CClass> c3 = CClass::Instance(); c3->Init(3, 1.0); c3->Init();
                ptrs_vector1 = { c1, c2, c3 };
			}
			std::vector<boost::shared_ptr<CClass>> ptrs_vector2; {
				boost::shared_ptr<CClass> c1 = CClass::Instance(); c1->Init(1, 3.0); c1->Init();
				boost::shared_ptr<CClass> c2 = CClass::Instance(); c2->Init(2, 2.0); c2->Init();
				boost::shared_ptr<CClass> c3 = CClass::Instance(); c3->Init(3, 1.0); c3->Init();
				ptrs_vector2 = { c1, c2, c3 };
			}
			std::vector<boost::shared_ptr<CClass>> ptrs_vector3; {
				boost::shared_ptr<CClass> c1 = CClass::Instance(); c1->Init(1, 3.0); c1->Init();
				boost::shared_ptr<CClass> c2 = CClass::Instance(); c2->Init(2, 2.0); c2->Init();
				ptrs_vector3 = { c1, c2, c2 };
			}
			CPropertyContainerOfPtrs<std::vector<boost::shared_ptr<CClass>>, CClass::COPY_OPERATOR, CClass::DECOMPOSITOR> p1(&ptrs_vector1, L"p1");
			CPropertyContainerOfPtrs<std::vector<boost::shared_ptr<CClass>>, CClass::COPY_OPERATOR, CClass::DECOMPOSITOR> p2(&ptrs_vector2, L"p2");
			CPropertyContainerOfPtrs<std::vector<boost::shared_ptr<CClass>>, CClass::COPY_OPERATOR, CClass::DECOMPOSITOR> p3(&ptrs_vector3, L"p3");
			STANDARD_PROPERTY_TEST(p1, p2, p3);

			p2.CopyFrom(p1);
			p3.CopyFrom(p1);
			BOOST_CHECK(p2.IsEqualTo(p1) && p3.IsEqualTo(p1) && p2.IsEqualTo(p3));

			ptrs_vector1[0]->Init(999, 998);
			BOOST_CHECK(!p2.IsEqualTo(p1) && !p3.IsEqualTo(p1) && p2.IsEqualTo(p3));
		}

		// test std::map of smart-pointers of CClass
		{
			std::map<std::wstring, boost::shared_ptr<CClass>> map_ptrs1; {
				boost::shared_ptr<CClass> c1 = CClass::Instance(); c1->Init(1, 3.0, L"key1"); c1->Init();
				boost::shared_ptr<CClass> c2 = CClass::Instance(); c2->Init(2, 2.0, L"key2"); c2->Init();
				boost::shared_ptr<CClass> c3 = CClass::Instance(); c3->Init(3, 1.0, L"key3"); c3->Init();
				map_ptrs1[c1->GetKey()] = c1;
				map_ptrs1[c2->GetKey()] = c2;
				map_ptrs1[c3->GetKey()] = c3;
			}
			std::map<std::wstring, boost::shared_ptr<CClass>> map_ptrs2;{
				boost::shared_ptr<CClass> c1 = CClass::Instance(); c1->Init(1, 3.0, L"key1"); c1->Init();
				boost::shared_ptr<CClass> c2 = CClass::Instance(); c2->Init(2, 2.0, L"key2"); c2->Init();
				boost::shared_ptr<CClass> c3 = CClass::Instance(); c3->Init(3, 1.0, L"key3"); c3->Init();
				map_ptrs2[c1->GetKey()] = c1;
				map_ptrs2[c2->GetKey()] = c2;
				map_ptrs2[c3->GetKey()] = c3;
			}
			std::map<std::wstring, boost::shared_ptr<CClass>> map_ptrs3; {
				boost::shared_ptr<CClass> c1 = CClass::Instance(); c1->Init(3, 1.0, L"key1"); c1->Init();
				boost::shared_ptr<CClass> c2 = CClass::Instance(); c2->Init(2, 2.0, L"key2"); c2->Init();
				boost::shared_ptr<CClass> c3 = CClass::Instance(); c3->Init(1, 3.0, L"key3"); c3->Init();
				map_ptrs3[c1->GetKey()] = c1;
				map_ptrs3[c2->GetKey()] = c2;
				map_ptrs3[c3->GetKey()] = c3;
			}

			CClass::DECOMPOSITOR decompositor;
			auto key_getter = boost::bind(&CClass::KeyGetter, _1);
			CPropertyMapOfPtrs<std::map<std::wstring, boost::shared_ptr<CClass>>, CClass::COPY_OPERATOR> p1(&map_ptrs1, L"p1", decompositor, key_getter, CClass::Instance);
			CPropertyMapOfPtrs<std::map<std::wstring, boost::shared_ptr<CClass>>, CClass::COPY_OPERATOR> p2(&map_ptrs2, L"p2", decompositor, key_getter, CClass::Instance);
			CPropertyMapOfPtrs<std::map<std::wstring, boost::shared_ptr<CClass>>, CClass::COPY_OPERATOR> p3(&map_ptrs3, L"p3", decompositor, key_getter, CClass::Instance);
			STANDARD_PROPERTY_TEST(p1, p2, p3);

			p2.CopyFrom(p1);
			p3.CopyFrom(p1);
			BOOST_CHECK(p2.IsEqualTo(p1) && p3.IsEqualTo(p1) && p2.IsEqualTo(p3));

			map_ptrs1.begin()->second->Init(999, 998);
			BOOST_CHECK(!p2.IsEqualTo(p1) && !p3.IsEqualTo(p1) && p2.IsEqualTo(p3));
		}
	}

	{
		ngp_shared_array<char> arr1 = { 'f','f' };
		ngp_shared_array<char> arr2 = { 'f','f' };
		ngp_shared_array<char> arr3 = { 'f','\0','v' };

		CPropertyContainerOfBinary<ngp_shared_array<char>> p1(&arr1, L"p1");
		CPropertyContainerOfBinary<ngp_shared_array<char>> p2(&arr2, L"p2");
		CPropertyContainerOfBinary<ngp_shared_array<char>> p3(&arr3, L"p3");
		STANDARD_PROPERTY_TEST(p1, p2, p3);
	}
	{
		std::vector<int> arr1 = { 0, 3, 4, 5, 7, 33, 45 };
		std::vector<int> arr2 = { 0, 3, 4, 5, 7, 33, 45 };
		std::vector<int> arr3 = { 1, 9, 9, 5 };

		CPropertyContainerOfBinary<std::vector<int>> p1 (&arr1, L"p1");
		CPropertyContainerOfBinary<std::vector<int>> p2 (&arr2, L"p2");
		CPropertyContainerOfBinary<std::vector<int>> p3 (&arr3, L"p3");
		STANDARD_PROPERTY_TEST(p1, p2, p3);
	}
	{
		ngp_shared_array<ngp_shared_array<char>> arr1 = { { 'f','\0','v' },{ '\0','s','x' } };
		ngp_shared_array<ngp_shared_array<char>> arr2 = { { 'f','\0','v' },{ '\0','s','x' } };
		ngp_shared_array<ngp_shared_array<char>> arr3 = { { 'x','\0','e', 'z' },{ 'S','z','v', 'z' } };

		auto decompositor = boost::bind(&XML_serialization_helpers::AddSharedArray, _1, _2, _3);

		CPropertyContainerOf<ngp_shared_array<ngp_shared_array<char>>> p1(&arr1, L"p1", decompositor);
		CPropertyContainerOf<ngp_shared_array<ngp_shared_array<char>>> p2(&arr2, L"p2", decompositor);
		CPropertyContainerOf<ngp_shared_array<ngp_shared_array<char>>> p3(&arr3, L"p3", decompositor);
		STANDARD_PROPERTY_TEST(p1, p2, p3);
	}

	{
		std::set<long> set1 = { 0, 99, 7547, 345, 777, 777 };
		std::set<long> set2 = { 0, 99, 7547, 345, 777, 777 };
		std::set<long> set3 = { 0, 99, 7547, 300, 777, 777 };

		CPropertySetOfSimple<std::set<long>> p1(&set1, L"s1");
		CPropertySetOfSimple<std::set<long>> p2(&set2, L"s2");
		CPropertySetOfSimple<std::set<long>> p3(&set3, L"s3");
		STANDARD_PROPERTY_TEST(p1, p2, p3);
	}
}

//////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test_CollectingProperties)
{
	struct STop {
		int m_I;
		double m_D;
		std::wstring m_S;
		TestEnum m_E;
		double m_A[3];
		CFontDef m_NestedStruct;
		std::list<std::wstring> m_str_list;
		std::list<CNGPString> m_ngpstr_list;
		std::vector<double> m_dbl_list;
		ngp_shared_array<double> m_shared_arr_double;
		ngp_shared_array<CNGPString> m_shared_arr_string;
		std::map<CNGPString,CFontDef> m_map_of_structs;

		void Fill(CPropertyRefCollection& props)
		{
			props.Add( PROPERTY_SIMPLE_REF( m_I, L"int", int ) );
			props.Add( PROPERTY_SIMPLE_REF( m_D, L"double", double ) );
			props.Add( PROPERTY_SIMPLE_REF( m_S, L"string", std::wstring ) );
			props.Add( PROPERTY_ENUM_REF( m_E, L"enum", TestEnum ) );
			props.Add( PROPERTY_SIMPLEARR_REF( m_A, L"arr", double ) );
			props.Add( PROPERTY_CONTAINEROF_REF( m_str_list, L"str_list", std::list<std::wstring>) );
			props.Add( PROPERTY_CONTAINEROF_REF( m_ngpstr_list, L"ngpstr_list", std::list<CNGPString>) );
			props.Add( PROPERTY_CONTAINEROF_REF( m_dbl_list, L"dbl_list", std::vector<double>) );
			props.Add( PROPERTY_CONTAINEROF_REF( m_shared_arr_double, L"shared_arr_double", ngp_shared_array<double>) );
			props.Add( PROPERTY_CONTAINEROF_REF( m_shared_arr_string, L"shared_arr_string", ngp_shared_array<CNGPString>) );
			props.Add( PROPERTY_MAP_OF_STRUCT_REF( m_map_of_structs, L"map_of_structs", &XML_serialization_helpers::Add, [](const CNGPString& key) { return std::wstring(key.c_str()); }, [](const std::wstring& key) { return CNGPString(key.c_str()); }  ) );

			XML_serialization_helpers::Add( m_NestedStruct, L"font", props );
		}
	};

	STop s1;
	s1.m_I = 1;
	s1.m_D = 3.14;
	s1.m_S = L"тест";
	s1.m_E = e_item3;
	s1.m_A[0] = 100;
	s1.m_A[1] = 101;
	s1.m_A[2] = 102;
    s1.m_str_list = { L"asdfgh", L"qwerty", L"yuiop", L"lkjhg" };
    s1.m_ngpstr_list = { CNGPString(L"asdfgh"), CNGPString(L"qwerty"), CNGPString(L"yuiop") };
    s1.m_dbl_list = { 1000.12312312, 2, 321312.321312, 23123.657567, 0 };
	s1.m_shared_arr_double.push_back(2.2); s1.m_shared_arr_double.push_back(2.3);
	s1.m_shared_arr_string.push_back(L""); s1.m_shared_arr_string.push_back(L"aaa"); s1.m_shared_arr_string.push_back(L"bbbbb"); s1.m_shared_arr_string.push_back(L"ccccccc");
	s1.m_map_of_structs[L"font1"] = CFontDef( L"Times New Roman", 12, 255, false, false );
	s1.m_map_of_structs[L"font2"] = CFontDef( L"Tahoma", 10, 0, false, true );

	// create collection on the struct's properties
	CPropertyRefCollection props1(L"root");
	s1.Fill( props1 );

	// save collection to xml file
	boost::property_tree::wptree xmltree;
	props1.SaveToPTree( xmltree );
	//boost::property_tree::write_xml( "_PrpRefColl.xml", xmltree );

	// restore structure object from xml file
	{
		STop s2;
		CPropertyRefCollection props2(L"root");
		s2.Fill( props2 );
		props2.LoadFromPTree( xmltree );

		const bool are_equal = NGP_VERIFY( props1 == props2 );
		BOOST_CHECK( are_equal );

		BOOST_CHECK( s1.m_I == s2.m_I );
		BOOST_CHECK( s1.m_D == s2.m_D );
		BOOST_CHECK( s1.m_S == s2.m_S );
		BOOST_CHECK( s1.m_str_list == s2.m_str_list );
		BOOST_CHECK( s1.m_dbl_list == s2.m_dbl_list );
		BOOST_CHECK( std::equal(s1.m_shared_arr_double.begin(), s1.m_shared_arr_double.end(), s2.m_shared_arr_double.begin()) );
		BOOST_CHECK( std::equal(s1.m_shared_arr_string.begin(), s1.m_shared_arr_string.end(), s2.m_shared_arr_string.begin()) );
	}

	// test binary serialization
	{
		const fs::path test_dir = fs::current_path() / L"test_dir";
		BOOST_CHECK(filesystem_utils::create_directory(test_dir));

		props1.SaveToBinaryFile( test_dir / utf16_string("_бин_file.bin") );
		STop s3;
		CPropertyRefCollection props3(L"root");
		s3.Fill( props3 );
		props3.LoadFromBinaryFile( test_dir / utf16_string("_бин_file.bin") );

		const bool are_equal = NGP_VERIFY( props1 == props3 );
		BOOST_CHECK( are_equal );
	}
}

class CGeomManager:
	public CNGPAPIBaseClass<CGeomManager>,
	public ngp_rpr::INGPGeomManager
{
public:
	DECLARE_NGP_FACTORY_METHOD(Instance, CGeomManager);
	virtual ~CGeomManager() {}

	NGP_BEGIN_INTERFACE_MAP
		NGP_INTERFACE_ENTRY(INGPUnknown)
		NGP_INTERFACE_ENTRY(ngp_rpr::INGPGeomManager) 
		NGP_END_INTERFACE_MAP

	//INGPGeomManager
	virtual IGDBLightPtr GetGDBLight() const { return IGDBLightPtr();}
	virtual INGPGeometryDatabasePtr GetGeometryDB() const { return INGPGeometryDatabasePtr(); }
	virtual CNGPString GetGeometryDBVersionID() const { return CNGPString(); }  
	virtual INGPUIExcelParamPtr GetEDBMaterialParameter(FWParameterType param_type, const wchar_ngp* mat_uuid, FwEngineeringDBContentTypes mat_type) const { return INGPUIExcelParamPtr(); }
	virtual bool CollectReferences(const ngp_shared_array<ngp_rpr::GeomRef>& refs, INGPProgress* pProgress, bool combine, ngp_shared_array<ngp_rpr::INGPGeomManager::ReferencesLinks>& RefLinks) { return true; }
	virtual bool GetDestGeomRef(const ngp_rpr::GeomRef& src_ref, bool update_ref_plane, bool allow_activate_config, const ngp_rpr::INGPGeomManager* dest_geom_mngr, ngp_rpr::GeomRef& dest_ref) const { return true; }
	virtual bool PrepareGeometry_ParticleStudy(const ngp_shared_array<ngp_shared_array<ngp_rpr::GeomRef>>&, ngp_shared_array<INGPSlvFacePtr>& faces) { return true; }
	virtual bool GetUpdateCADGeometry(bool bSaveGeomRefAttr, const NGPVisu::ICADGeometryPtr& geom, bool update_appearance) const { return true; }
	virtual bool IsGDBLightPossible(const vwr_io::RESULTS_FILE_INFO& info) const override { return false; };
	virtual CNGPString FindAndCreateReferenceByGeomPath(const CADGeomPath&) override { return CNGPString(); }
	virtual bool UpdateRefPlaneParams(const wchar_t* ref_uuid, FWPostGeomReferenceType ref_type, bool correct_pos, NGPPlaneGeomReference& plane) const override { return false; }

private:
	CGeomManager() {}
};

BOOST_AUTO_TEST_CASE(test_FeaturesSerialization)
{
	auto Init_CropRegion = [](const CFeaturePtr& feat, double val)
	{
		CCropedPlotPtr crop_plot = boost::dynamic_pointer_cast<CCropedPlot>(feat);
		if (crop_plot)
		{
			NGPNIK3DBox box;
			box[0] = -val; box[1] = val;
			box[2] = -val; box[3] = val;
			box[4] = -val; box[5] = val;
			boost::array<bool, 6> croppedSides = { false, true, false, false, false, false };
			crop_plot->SetCropData(croppedSides, box);
		}
	};

	//////////////////////////////////////////////////////////////////////////

	BOOST_CHECK(true);
	const fs::path test_dir = fs::current_path() / L"test_dir";
	BOOST_CHECK(filesystem_utils::create_directory(test_dir));

	boost::shared_ptr<CGeomManager> geom_manager = CGeomManager::Instance();

	CRPRProjectPtr proj1;
	CRPRProjectPtr proj2;

	boost::system::error_code ec;
	fs::remove_all(test_dir / CRPRDocument::rpr_data_dir, ec);

	DocumentInitializer doc_init;
	doc_init.name = L"doc_name1";
	doc_init.directory = test_dir.c_str();
	CRPRDocumentPtr doc1 = CRPRDocument::Instance(doc_init);
	ngp_rpr::GetCreatePostprocessor()->CreateDocument(doc_init);

	doc_init.name = L"doc_name2";
	doc_init.directory = test_dir.c_str();
	CRPRDocumentPtr doc2 = CRPRDocument::Instance(doc_init);
	ngp_rpr::GetCreatePostprocessor()->CreateDocument(doc_init);

	ConfigurationInitializer cfg_init;
	cfg_init.name = L"cgf1_name";
	cfg_init.geom_mngr = geom_manager;
	CRPRConfigurationPtr cfg = boost::static_pointer_cast<CRPRConfiguration>(doc1->AddUpdateConfiguration(cfg_init));

	{
		ProjectInitializer proj_init;
		proj_init.uuid = L"proj1_uuid";
		proj_init.name = L"proj1_name";
		proj_init.geom_mngr = geom_manager;
		proj1 = boost::static_pointer_cast<CRPRProject>(cfg->AddUpdateProject(proj_init));

		ProjectData data;
		data.CompDomain.Box[0] = -1; data.CompDomain.Box[1] = 1;
		data.CompDomain.Box[2] = -1; data.CompDomain.Box[3] = 1;
		data.CompDomain.Box[4] = -1; data.CompDomain.Box[5] = 1; 
		data.ModelBox[0] = -2; data.ModelBox[1] = 2;
		data.ModelBox[2] = -2; data.ModelBox[3] = 2;
		data.ModelBox[4] = -2; data.ModelBox[5] = 2;
		data.CDOverallDimension = 4;
		data.ModelOverallDimension = 6;
		data.ModelName = L"doc_name1";
		data.ConfigName = cfg_init.name;
		data.ProjectName = proj_init.name;
		proj1->PutProjectData(data, false);
	}

	{
		ProjectInitializer proj_init;
		proj_init.uuid = L"proj2_uuid";
		proj_init.name = L"proj2_name";
		proj_init.geom_mngr = geom_manager;
		proj2 = boost::static_pointer_cast<CRPRProject>(cfg->AddUpdateProject(proj_init));

		ProjectData data;
		data.CompDomain.Box[0] = -2; data.CompDomain.Box[1] = 2;
		data.CompDomain.Box[2] = -2; data.CompDomain.Box[3] = 2;
		data.CompDomain.Box[4] = -2; data.CompDomain.Box[5] = 2; 
		data.ModelBox[0] = -3; data.ModelBox[1] = 3;
		data.ModelBox[2] = -3; data.ModelBox[3] = 3;
		data.ModelBox[4] = -3; data.ModelBox[5] = 3;
		data.CDOverallDimension = 4.5;
		data.ModelOverallDimension = 6.5;
		data.ModelName = L"doc_name1";
		data.ConfigName = cfg_init.name;
		data.ProjectName = proj_init.name;
		proj2->PutProjectData(data, false);
	}

	BOOST_CHECK(doc1->Save());
	BOOST_CHECK(doc2->Save());

	INGP_DocumentPtr loaded_doc1 = ngp_rpr::GetCreatePostprocessor()->LoadDocument(test_dir.wstring().c_str(), L"doc_name1");
	BOOST_CHECK(loaded_doc1->FindProject(L"proj1_uuid"));
	BOOST_CHECK(loaded_doc1->FindConfiguration(L"cgf1_name")->FindProject(L"proj1_uuid")->GetProjectData()->GetCDOverallDim() == 4);
	BOOST_CHECK(loaded_doc1->FindConfiguration(L"cgf1_name")->FindProject(L"proj2_uuid")->GetProjectData()->GetCDOverallDim() == 4.5);
	BOOST_CHECK(loaded_doc1->GetName() == L"doc_name1");

	INGP_DocumentPtr loaded_doc2 = ngp_rpr::GetCreatePostprocessor()->LoadDocument(test_dir.wstring().c_str(), L"doc_name2");
	BOOST_CHECK(loaded_doc2);
	BOOST_CHECK(loaded_doc2->GetName() == L"doc_name2");

	//////////////////////////////////////////////////////////////////////////

	ngp_shared_array<INGP_FeaturePtr> features;

	for (int i = 1; i < 2; ++i)
	{
		for (FeatureType f = ft_cut_plot; f < ft_COUNT; f = FeatureType(f + 1))
		{
//			BOOST_TEST_MESSAGE("Test feature, type = " << f);

			CFeaturePtr feat1 = feature_utils::CreateFeature(f);
			BOOST_CHECK(feat1);

			{
				INGP_FeaturePtr ifeature;
				feat1->QueryInterface( boost::any(&ifeature) );
				_ASSERTE(ifeature);
				BOOST_CHECK( ifeature );
				CFeaturePtr casted_feat = feature_utils::CastFeature( ifeature );
				BOOST_CHECK_EQUAL( casted_feat.get(), feat1.get() );
			}

			features.push_back(feat1);

			Init_CropRegion(feat1, 0.2);
			feat1->PutInterpolateResults(false); //invert def value

			if (f != ft_particle_injection)
				const_cast<feature_id_t&>(feat1->GetUUID()) = (boost::wformat(L"uuid_%i") % (f*i)).str().c_str();

			const_cast<CNGPString&>(feat1->Name()) = (boost::wformat(L"name_%i") % (f*i)).str().c_str();

			const GeomRef gr1 = rand_geom_ref();
			const GeomRef gr2 = rand_geom_ref();
			const GeomRef gr3 = rand_geom_ref();

			VisParameter param4;
			param4.uuid = utf16_string("параметр4").c_str();
			VisParameter param3;
			param3.uuid = utf16_string("параметр3").c_str();
			VisParameter param2;
			param2.uuid = utf16_string("параметр2").c_str();
			VisParameter param1;
			param1.uuid = L"param_uuid1";

			#define ADD_GEOM_REFS(plot) const_cast<GEOM_REFS&>(plot->GetRefs()) = { CGeomRef::Instance(gr1), CGeomRef::Instance(gr2), CGeomRef::Instance(gr3) }
			#define ADD_PARAMS(plot) const_cast<VIS_PARAMS&>(plot->GetParams()) = { param1, param2, param3 }
			#define ADD_FLDS(plot) const_cast<std::vector<std::wstring>&>(plot->GetFlds()) = { L"fld1", utf16_string("флд"), L"fld3" }
			#define ADD_POINTS(plot) const_cast<std::vector<Point>&>(plot->GetPoints()) = { Point(), Point(), Point() }
			#define SET_UUID(plot, u) const_cast<feature_id_t&>(plot->GetUUID()) = u;

			switch (f) {
			case ft_cut_plot: {
				CCutPlotPtr plot = boost::static_pointer_cast<CCutPlot>(feat1);
				plot->Medias.insert(fwPostMediaFluid);
				plot->Medias.insert(fwPostMediaSolid);
				ADD_GEOM_REFS(plot);
				break;}
			case ft_isosurface: {
				CIsosurfacePlotPtr plot = boost::static_pointer_cast<CIsosurfacePlot>(feat1);
				plot->PlotParam.uuid = L"Pressure";
				plot->IsoValues.push_back(110000);
				plot->ColoringPalette.SetParameter( L"Temp" );
				break;}
			case ft_goals_plot: {
				CGoalPlotPtr plot = boost::static_pointer_cast<CGoalPlot>(feat1);
                const_cast<GOALS_UUIDS&>(plot->_GetGoals()) = { L"goal_uuid1", utf16_string("цель_uuid2").c_str(), L"goal_uuid3" };
				break;}
			case ft_surf_params:
			case ft_volume_params: {
				CSurfaceParametersPlotPtr plot = boost::static_pointer_cast<CSurfaceParametersPlot>(feat1);
				ADD_PARAMS(plot); ADD_FLDS(plot); ADD_GEOM_REFS(plot);
				plot->SetFullTask(true);
				break;}				
			case ft_point_param: {
				CPointParametersPlotPtr plot = boost::static_pointer_cast<CPointParametersPlot>(feat1);
				ADD_PARAMS(plot); ADD_GEOM_REFS(plot); ADD_POINTS(plot); ADD_FLDS(plot);
				break;}
			case ft_xy_plot: {
				CXYPlotPtr plot = boost::static_pointer_cast<CXYPlot>(feat1);
				ADD_GEOM_REFS(plot); ADD_PARAMS(plot);
				break;}
			case ft_flow_trajectrories: {
				CFlowTrajectoriesPlotPtr plot = boost::static_pointer_cast<CFlowTrajectoriesPlot>(feat1);
				ADD_GEOM_REFS(plot);
				ADD_POINTS(plot);
                const_cast<VIS_PARAMS&>(plot->GetFluidResParams()) = { param1, param2, param3 };
                const_cast<VIS_PARAMS&>(plot->GetParticleParams()) = { param1, param2, param3 };
				break;}
			case ft_particle_study:
				{
					CParticleStudyPtr plot = boost::static_pointer_cast<CParticleStudy>(feat1);
                    const_cast<VIS_PARAMS&>(plot->GetFluidResParams()) = { param1, param2, param3 };
                    const_cast<VIS_PARAMS&>(plot->GetParticleParams()) = { param1, param2, param3 };
					Init_CropRegion(plot, 0.2);
					randomize( plot );

					CParticleInjectionPtr inj1 = plot->_CreateInjection(L"inj1_uuid");
					CParticleInjectionPtr inj2 = plot->_CreateInjection(L"inj2_uuid");
					ADD_GEOM_REFS(inj1); ADD_POINTS(inj1);
					ADD_GEOM_REFS(inj2); ADD_POINTS(inj2);
					Init_CropRegion(inj1, 0.2);
					Init_CropRegion(inj2, 0.3);
					randomize( inj1 );
					randomize( inj2 );

					inj2->SetUUID(L"inj2_uuid_new");

					BOOST_CHECK(plot->GetInjections().size() == 2);
					BOOST_CHECK(plot->_GetInjection(L"inj1_uuid"));
					BOOST_CHECK(plot->_GetInjection(L"inj2_uuid_new"));
					BOOST_CHECK(inj1->GetUUID() == L"inj1_uuid");
					BOOST_CHECK(inj2->GetUUID() == L"inj2_uuid_new");

					CParticleICPtr ic1 = CParticleIC::Instance();
					CParticleICPtr ic2 = CParticleIC::Instance();
					SET_UUID(ic1, L"ic1_uuid"); SET_UUID(ic2, L"ic2_uuid");
                    const_cast<INITIAL_CONDITIONS&>(inj1->GetInitialConditions()) = { ic1, ic2 };
                    const_cast<INITIAL_CONDITIONS&>(inj2->GetInitialConditions()) = { ic2, ic1 };

					CParticleBCPtr bc1 = CParticleBC::Instance();
					CParticleBCPtr bc2 = CParticleBC::Instance();
					ADD_GEOM_REFS(bc1);	ADD_GEOM_REFS(bc2);
					SET_UUID(bc1, L"bc1_uuid"); SET_UUID(bc2, L"bc2_uuid");

					CExcelParameterPtr ex1 = CExcelParameter::Instance();
					const_cast<EXCEL_PARAMS&>(bc1->GetParams())[FWParameterType(-1)] = ex1;

					const_cast<BC_MAP&>(plot->GetBoundaryConditions())[bc1->GetUUID()] = bc1;
					const_cast<BC_MAP&>(plot->GetBoundaryConditions())[bc2->GetUUID()] = bc2;
					break;
				}
			case ft_mesh_plot:
				{
					CMeshPlotPtr plot = boost::static_pointer_cast<CMeshPlot>(feat1);
					ADD_GEOM_REFS(plot);
					break;
				}
			case ft_particle_injection:
				{
					CParticleInjectionPtr inj = boost::static_pointer_cast<CParticleInjection>(feat1);
					inj->SetUUID(L"INJ_UUID");

					ADD_GEOM_REFS(inj); ADD_POINTS(inj);
					randomize(inj);

					CParticleStudyPtr ps = inj->GetPS();
					BOOST_CHECK(ps);
					SET_UUID(ps, L"ps_uuid");
					Init_CropRegion(ps, 0.2);
					ps->SetMaxLength(500);

					inj->Init(ps.get()); //for ps_uuid

					CParticleInjectionPtr inj_from_ps = ps->GetInjection(L"INJ_UUID");
					BOOST_CHECK(inj_from_ps);

					INGP_ParticleInjectionPtr inj_intrf;
					inj->QueryInterface(boost::any(&inj_intrf));
					BOOST_CHECK(inj_intrf);

					INGP_ParticleStudyPtr ps_intrf;
					ps->QueryInterface(boost::any(&ps_intrf));
					BOOST_CHECK(ps_intrf);
					break;
				}
			case ft_ansys_export:
				{
					CAnsysExportPtr ansys = boost::static_pointer_cast<CAnsysExport>(feat1);
					ansys->SetUUID(CNGPString::BuildFromGUID(guid_utils::CreateUUID()));

					ngp_shared_array<INGPExportToAnsys_def::SParamValueFilter> filts;
					INGPExportToAnsys_def::SParamValueFilter f;
					f.m_bIsResetedMaxValue = f.m_bIsResetedMinValue = false;
					f.m_sParUUID = CNGPString::BuildFromGUID(guid_utils::CreateUUID());
					f.m_max_val = 1.22;
					f.m_min_val = 0.88;
					filts.push_back(f);
					f.m_max_val *= 1.22;
					f.m_min_val *= 0.88;
					filts.push_back(f);
					ansys->SetParamValFilters(filts);

					ansys->SetPath2ResFile(fs::temp_directory_path().wstring().c_str());
					ADD_PARAMS(ansys);

					break;
				}
			case ft_flux_balance:
				{
					CFluxBalancePtr fb = boost::static_pointer_cast<CFluxBalance>(feat1);

					ngp_shared_array<long> nodes;
					for (int i = 0; i < 4; ++i)
					{
						nodes.push_back(i);

						if (INGP_FluxBalance::NodeLayout* node = fb->AddNodeLayout(i))
						{
							node->x = i * 7;
							node->y = i * 7;
							node->appearance.at(0).DisplayTitle = (std::wstring(L"Node") + std::to_wstring(i)).c_str();
							node->appearance.at(0).Font.Name = (std::wstring(L"Font") + std::to_wstring(i)).c_str();
						}

						fb->AddNodesGroup(i, nodes);
					}

					INGP_FluxBalance::GraphStructure structure;
					structure.m_nodes.resize(10);
					structure.m_nodes_names.resize(10);
					for (int i = 0; i < structure.m_nodes.size(); ++i)
					{
						structure.m_nodes.at(i) = nodes;
						structure.m_nodes_names.at(i) = (std::wstring(L"Node") + std::to_wstring(i)).c_str();
					}
					fb->SetGraphStructure(structure);

					break;
				}
			case ft_feature_proxy:
				{
					CFeatureProxyPtr fp = boost::static_pointer_cast<CFeatureProxy>(feat1);
					ADD_GEOM_REFS(fp);
					break;
				}
			}

			//////////////////////////////////////////////////////////////////////////

            std::wstring xml_file_name = utf16_string((boost::format("feature тип №%i") % f).str());
            const fs::path xml_file = fs::path(xml_file_name).replace_extension("xml");
			feat1->SaveToFile(test_dir / xml_file);

			CFeaturePtr feat2 = feature_utils::CreateFeature(f);
			feat2->LoadFromFile(test_dir / xml_file);
			CHECK_EQUAL_FEATURES( feat1, feat2 );

			{
				ngp_shared_array<char> data1;
				feat1->Serialize(true, data1);

				CFeaturePtr feat3 = feature_utils::CreateFeature(f);
				BOOST_CHECK(feat3->Serialize(false, data1));
				CHECK_EQUAL_FEATURES( feat1, feat3 );

				ngp_shared_array<char> data2;
				BOOST_CHECK(feat3->Serialize(true, data2));
				BOOST_CHECK(data1.size() == data2.size() && std::equal(data1.begin(), data1.end(), data2.begin()));
			}

			fs::path _saved_file;
			BOOST_CHECK(CFeature::SaveFeature(*feat1, test_dir, xml_file_name, _saved_file));
			CFeaturePtr loaded_feat1 = CFeature::LoadFeature(test_dir, xml_file_name);
			BOOST_CHECK(loaded_feat1);
			BOOST_CHECK(loaded_feat1->GetUUID() == feat1->GetUUID() && loaded_feat1->GetType() == feat1->GetType());
			CHECK_EQUAL_FEATURES( feat1, loaded_feat1 );
			CHECK_EQUAL_FEATURES( loaded_feat1, feat2 );

			CFeaturePtr clone1 = feat1->_Clone();
			CFeaturePtr clone2 = feat2->_Clone();
			CHECK_EQUAL_FEATURES( feat1, clone1 );
			CHECK_EQUAL_FEATURES( feat2, clone2 );
			CHECK_EQUAL_FEATURES( clone1, clone2 );

			//////////////////////////////////////////////////////////////////////////

			CFeaturePtr proj_clone = feat1->CopyToProject(proj1.get(), *proj2, false);
			BOOST_CHECK(proj_clone);

			//////////////////////////////////////////////////////////////////////////

			BOOST_CHECK(proj2->UpdateFeatureCopy(proj_clone, feat1, feat2, proj1));

			//////////////////////////////////////////////////////////////////////////

			CFeaturePtr empty_feat = feature_utils::CreateFeature(f);
			if (f == ft_particle_injection)
			{
				empty_feat->SetUUID(L"INJ_UUID");
			}

			BOOST_CHECK(empty_feat->Assign(*feat1));
			CHECK_EQUAL_FEATURES(empty_feat, feat1);

			//////////////////////////////////////////////////////////////////////////
		}
	}

	//////////////////////////////////////////////////////////////////////////

	const fs::path save_feats_dir = test_dir / L"save_features";
	filesystem_utils::create_directory(save_feats_dir);
	BOOST_CHECK(ngp_rpr::GetCreatePostprocessor()->SaveFeatures(save_feats_dir.wstring().c_str(), features));
	for (int f = 0; f < features.size(); ++f) {	BOOST_CHECK(fs::exists(path_utils::concat(save_feats_dir / features[f]->GetUUID().c_str(), CFeature::feat_file_ext))); }

	ngp_shared_array<CNGPString> feats_uuids;
	for (const auto& f : features) feats_uuids.push_back(f->GetUUID());
	ngp_shared_array<INGP_FeaturePtr> loaded_feats;
	BOOST_CHECK(ngp_rpr::GetCreatePostprocessor()->LoadFeatures(save_feats_dir.wstring().c_str(), feats_uuids, loaded_feats));
	BOOST_CHECK(feats_uuids.size() == loaded_feats.size());

	const fs::path save_feats_dir1 = test_dir / L"save_features" / L"1";
	filesystem_utils::create_directory(save_feats_dir1);
	const fs::path save_feats_dir2 = test_dir / L"save_features" / L"2";
	filesystem_utils::create_directory(save_feats_dir2);
	ngp_shared_array<ngp_rpr::FeatureStore> feats_store;
	for (int f = 0; f < features.size(); ++f) {
		ngp_rpr::FeatureStore st; st.m_feat = &features.at(f); st.m_dir = save_feats_dir1.c_str();
		feats_store.push_back(st);
	}
	for (int f = 0; f < features.size(); ++f) {
		ngp_rpr::FeatureStore st; st.m_feat = &features.at(f); st.m_dir = save_feats_dir2.c_str();
		feats_store.push_back(st);
	}

	BOOST_CHECK(ngp_rpr::GetCreatePostprocessor()->SaveFeatures(feats_store));
	for (int f = 0; f < features.size(); ++f) { BOOST_CHECK(fs::exists(path_utils::concat(save_feats_dir1 / features[f]->GetUUID().c_str(), CFeature::feat_file_ext))); }
	for (int f = 0; f < features.size(); ++f) {	BOOST_CHECK(fs::exists(path_utils::concat(save_feats_dir2 / features[f]->GetUUID().c_str(), CFeature::feat_file_ext))); }

	feats_store.clear();
	ngp_shared_array<INGP_FeaturePtr> load_feats;
	load_feats.resize(features.size()*2);
	for (int f = 0; f < features.size(); ++f) {
		ngp_rpr::FeatureStore st; st.m_feat = &load_feats.at(f); st.m_feat_uuid = features[f]->GetUUID(); st.m_dir = save_feats_dir1.c_str();
		feats_store.push_back(st);
	}
	for (int f = 0; f < features.size(); ++f) {
		ngp_rpr::FeatureStore st; st.m_feat = &load_feats.at(features.size() + f); st.m_feat_uuid = features[f]->GetUUID(); st.m_dir = save_feats_dir2.c_str();
		feats_store.push_back(st);
	}

	BOOST_CHECK(ngp_rpr::GetCreatePostprocessor()->LoadFeatures(feats_store));
	for (int f = 0; f < load_feats.size(); ++f) { BOOST_CHECK(load_feats[f]); }

	{
		ngp_shared_array<INGP_FeaturePtr> feats;
		BOOST_CHECK(CFeature::LoadFeatures(save_feats_dir1, feats, false));
	}

	load_feats.clear();
	loaded_feats.clear();
	features.clear();

	//////////////////////////////////////////////////////////////////////////

	BOOST_CHECK_EQUAL(CFeature::InstanceCount(), 0);

	/////////////////////////////////////////////////////////////////////////////////

	class CClass1
	{
	public:
		bool m_bool = false;
		long m_long = 1;
		std::wstring m_wstring = L"WSTRING1";
		CNGPString m_ngpstring = L"NGPSTRING1";

		bool Save(wptree& tree) const
		{
			CPropertyRefCollection coll( tag_feature );

			const long version = 1;
			coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));
			CollectProperties( coll );

			coll.SaveToPTree( tree );
			return true;
		}

		bool Load(const wptree& tree)
		{
			CPropertyRefCollection coll( tag_feature );

			const long version = 0;
			coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));
			CollectProperties( coll );

			coll.LoadFromPTree( tree );
			return true;
		}

		void CollectProperties( CPropertyTreeNode& coll ) const
		{
			coll.Add(PROPERTY_SIMPLE_REF(m_bool, L"tag_bool", bool));
			coll.Add(PROPERTY_SIMPLE_REF(m_long, L"tag_long", long));
			coll.Add(PROPERTY_SIMPLE_REF(m_wstring, L"tag_wstring", std::wstring));
			coll.Add(PROPERTY_SIMPLE_REF(m_ngpstring, L"tag_ngpstring", CNGPString));
		}
	};

	//////////////////////////////////////////////////////////////////////////

	class CClass2
	{
	public:
		long m_long;
		std::wstring m_wstring;

		CClass2(): m_long(2), m_wstring(L"WSTRING2") {}

		bool Save(wptree& tree) const
		{
			CPropertyRefCollection coll( tag_feature );

			const long version = 2;
			coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));
			CollectProperties( coll );
			coll.SaveToPTree( tree );
			return true;
		}

		bool Load(const wptree& tree)
		{
			CPropertyRefCollection coll( tag_feature );

			const long version = 0;
			coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));
			CollectProperties( coll );
			coll.LoadFromPTree( tree );
			return true;
		}

		void CollectProperties( CPropertyTreeNode& coll ) const
		{
			coll.Add(PROPERTY_SIMPLE_REF(m_wstring, L"tag_wstring", std::wstring));
			coll.Add(PROPERTY_SIMPLE_REF(m_long, L"tag_long", long));
		}
	};

	//////////////////////////////////////////////////////////////////////////

	class CClass3
	{
	public:
		bool m_bool = false;
		long m_long = 3;
		std::wstring m_wstring = L"WSTRING3";
		std::wstring m_wstring3;
		CNGPString m_ngpstring = L"NGPSTRING3";
		FWUnitsEnum m_enum = fwIDR_area;
		std::vector<std::wstring> m_vector;

		long m_def_inited = 0;
		const long DefLongValue = 127;

		CClass3() {
            m_vector = { L"str1", L"str2", L"str3" };
		}

		bool Save(wptree& tree) const
		{
			CPropertyRefCollection coll( tag_feature );

			const long version = 3;
			coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));
			CollectProperties( coll );
			coll.SaveToPTree( tree );
			return true;
		}

		bool Load(const wptree& tree)
		{
			CPropertyRefCollection coll( tag_feature );

			const long version = 0;
			coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));
			CollectProperties( coll );
			coll.LoadFromPTree( tree );

			if (version < 3) //old
			{
				m_wstring3 = m_wstring; //conversion
			}

			return true;
		}

		void CollectProperties( CPropertyTreeNode& coll ) const
		{
			coll.Add(PROPERTY_ENUM_REF(m_enum, L"tag_enum", FWUnitsEnum));
			coll.Add(PROPERTY_SIMPLE_REF(m_ngpstring, L"tag_ngpstring", CNGPString));
			coll.Add(PROPERTY_SIMPLE_REF(m_bool, L"tag_bool", bool));
			coll.Add(PROPERTY_SIMPLE_REF(m_long, L"tag_long", long));
			coll.Add(PROPERTY_SIMPLE_REF(m_wstring, L"tag_wstring", std::wstring));
			coll.Add(PROPERTY_CONTAINEROF_REF(m_vector, L"tag_vector", std::vector<std::wstring>));
			coll.Add(PROPERTY_SIMPLE_REF_DEFINIT(m_def_inited, L"tag_def_inited", long, DefLongValue));
		}
	};

	//////////////////////////////////////////////////////////////////////////

	{
		CClass1 obj1;
		wptree tag1;
		BOOST_CHECK(obj1.Save(tag1));
		const std::wstring xml_obj1 = L"obj1.xml";
		write_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag1);

		CClass2 obj2;
		wptree tag2;
		read_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag2);
		BOOST_CHECK(obj2.Load(tag2));

		BOOST_CHECK(obj2.m_long == obj1.m_long);
		BOOST_CHECK(obj2.m_wstring == obj1.m_wstring);
	}

	//////////////////////////////////////////////////////////////////////////

	{
		CClass1 obj1;
		wptree tag1;
		BOOST_CHECK(obj1.Save(tag1));
		const std::string xml_obj1 = "obj1.xml";
		write_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag1);

		CClass3 obj3;
		wptree tag3;
		read_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag3);
		BOOST_CHECK(obj3.Load(tag3));

		BOOST_CHECK(obj3.m_bool == obj1.m_bool);
		BOOST_CHECK(obj3.m_long == obj1.m_long);
		BOOST_CHECK(obj3.m_wstring == obj1.m_wstring);
		BOOST_CHECK(obj3.m_ngpstring == obj1.m_ngpstring);
		CClass3 obj3_def;
		BOOST_CHECK(obj3.m_enum == obj3_def.m_enum);
		BOOST_CHECK(obj3.m_vector == obj3_def.m_vector);
		BOOST_CHECK(obj3.m_wstring3 == obj1.m_wstring);
		BOOST_CHECK(obj3.m_def_inited == obj3.DefLongValue);
	}

	//////////////////////////////////////////////////////////////////////////

	CProjectDataPtr project_data = CProjectData::Instance();
	BOOST_CHECK(project_data->SaveToFile(test_dir / L"project_data.dat"));
	BOOST_CHECK(project_data->LoadFromFile(test_dir / L"project_data.dat"));

	//////////////////////////////////////////////////////////////////////////

	ResFileKey key1;
	key1.m_keys[0] = L"str1_0";
	key1.m_keys[1] = L"str1_1";
	key1.m_keys[2] = L"str1_2";
	key1.m_keys[4] = L"str1_4";
	key1.m_keys[rfk_geom_data_base_ver] = CNGPString::BuildFromGUID(guid_utils::CreateUUID());

	ResFileKey key2;
	key2.m_keys[1] = L"str2_1";
	key2.m_keys[3] = L"str2_3";

	key1 |= key2;

	BOOST_CHECK(key1.m_keys[0] == L"str1_0");
	BOOST_CHECK(key1.m_keys[1] == L"str1_1");
	BOOST_CHECK(key1.m_keys[2] == L"str1_2");
	BOOST_CHECK(key1.m_keys[3] == L"str2_3");
	BOOST_CHECK(key1.m_keys[4] == L"str1_4");

	ngp_file_io::INGPFileIOPtr file = ngp_file_io::CreateFileIO();
	BOOST_CHECK(file->open((test_dir / L"key.bin").wstring().c_str(), L"wb"));
	BOOST_CHECK(key1.save(file));
	file.reset();
	ResFileKey key_saved;
	file = ngp_file_io::CreateFileIO();
	BOOST_CHECK(file->open((test_dir / L"key.bin").wstring().c_str(), L"rb"));
	BOOST_CHECK(key_saved.load(file));
	BOOST_CHECK(key1.is_equal(key_saved));
	file.reset();

	ResFileKey ref_params_key;
	for (eCollectRefParamsOptions p = CRP_TEMPERATURE; p < CRP_LAST; p = eCollectRefParamsOptions(p * 2)) {
		BOOST_CHECK(ngp_rpr::ResFileKey::ref_params_opt_to_key(p) != ngp_rpr::rfk_invalid);
		ref_params_key.m_keys[ngp_rpr::ResFileKey::ref_params_opt_to_key(p)] = CNGPString::BuildFromGUID(guid_utils::CreateUUID());
	}

	for (ResFileKeyItem i = rfk_feat_ver; i < rfk_count; i = ResFileKeyItem(i + 1))	{
		ngp_rpr::ResFileKey::is_external(i);
	}

	//////////////////////////////////////////////////////////////////////////

	ResFileKey k1;
	k1.m_transient_time = 0.5;
	for (ResFileKeyItem i = rfk_feat_ver; i < rfk_count; i = ResFileKeyItem(i + 1))
	{
		k1.m_keys[i] = CNGPString::BuildFromGUID(guid_utils::CreateUUID());
		k1.m_nested_keys.push_back(key1);
	}
	k1.m_keys_arr[rfka_tracer_study_sfld].push_back(CNGPString::BuildFromGUID(guid_utils::CreateUUID()));
	k1.m_keys_arr[rfka_tracer_study_sfld].push_back(CNGPString::BuildFromGUID(guid_utils::CreateUUID()));

	{
		{
			std::wfstream fstr((test_dir / L"key1.txt").string().c_str(), std::wfstream::out|std::wfstream::trunc);
			BOOST_CHECK(fstr.is_open());
			bool res = k1.serialize<std::wfstream, std::wstring>(fstr, true, true, [](const std::wstring& str) { return str.c_str(); } );
			BOOST_CHECK(res);
		}

		ResFileKey k2;
		{
			std::wfstream fstr((test_dir / L"key1.txt").string().c_str(), std::wfstream::in);
			bool res = k2.serialize<std::wfstream, std::wstring>(fstr, false, true, [](const std::wstring& str) { return str.c_str(); } );
			BOOST_CHECK(res);
		}

		BOOST_CHECK(k1.is_equal(k2));
	}

#ifdef _WIN32
	{
		{
			CFile theFile;
			theFile.Open((test_dir / L"key3.bin").wstring().c_str(), CFile::modeCreate|CFile::modeWrite);
			CArchive archive1(&theFile, CArchive::store);
			bool res = k1.serialize<CArchive, CString>(archive1, true, false, [](const CString& str) { return str.GetString(); } );
			BOOST_CHECK(res);
		}

		ResFileKey k2;
		{
			CFile theFile;
			theFile.Open((test_dir / L"key3.bin").wstring().c_str(), CFile::modeRead);
			CArchive archive2(&theFile, CArchive::load);
			bool res = k2.serialize<CArchive, CString>(archive2, false, false, [](const CString& str) { return str.GetString(); } );
			BOOST_CHECK(res);
		}

		BOOST_CHECK(k1.is_equal(k2));

		ResFileKey k3;
		{
			CFile theFile;
			theFile.Open((test_dir / L"key3.bin").wstring().c_str(), CFile::modeRead);
			CArchive archive2(&theFile, CArchive::load);
			bool res = k3.serialize<CArchive, CString>(archive2, false, false, [](const CString& str) { return str.GetString(); } ); 
			BOOST_CHECK(res);
		}
	}
#endif

	//////////////////////////////////////////////////////////////////////////

	boost::filesystem::remove_all(test_dir);
}

BOOST_AUTO_TEST_CASE(test_CopyFromFT)
{
	CParticleStudyPtr ps = CParticleStudy::Instance();
	CParticleInjectionPtr inj = ps->_CreateInjection(L"inj uuid");
	CFlowTrajectoriesPlotPtr ft = CFlowTrajectoriesPlot::Instance();

	inj->CopyFromFT( *ft );
	BOOST_CHECK( inj->GetType() == ft_particle_injection );

	CPropertyRefCollection l_coll(tag_feature), r_coll(tag_feature);
	inj->CFTPIBase::CollectProperties( l_coll, CollectOptions(PF_Assignable) );
	ft->CFTPIBase::CollectProperties( r_coll, CollectOptions(PF_Assignable) );
	BOOST_CHECK( l_coll == r_coll );
}

BOOST_AUTO_TEST_CASE(Test_Feature_Serialization_Memory)
{
    using namespace ngp_file_io;
    ngp_shared_array<char> buf;

    GUID id = guid_utils::CreateUUID();
    CNGPString sid = CNGPString(guid_utils::to_string(id).c_str());

    {
        CFeaturePtr feat = feature_utils::CreateFeature(ft_isosurface);
        feat->SetUUID(sid);
        feat->SetMovingReference(true);
        feat->SetMovingDisplay(false);

        auto io = CreateFileIO();
        auto scp = make_mem_write_scope(io.get(), buf);
        BOOST_CHECK(feat->Save(io));
    }
    {
        auto io = CreateFileIO();
        auto scp = make_mem_read_scope(io.get(), buf);

        CFeaturePtr feat = CFeature::LoadFeature(io);
        BOOST_CHECK(feat);
        if (feat)
        {
            BOOST_CHECK(feat->GetType() == ft_isosurface);
            BOOST_CHECK(feat->GetUUID() == sid);
            BOOST_CHECK(feat->GetMovingReference() == true);
            BOOST_CHECK(feat->GetMovingDisplay() == false);
        }
    }
}
