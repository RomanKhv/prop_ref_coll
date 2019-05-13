#include "stdafx.h"
#include <codecvt>
#include <boost/test/unit_test.hpp>

//#include <boost/property_tree/json_parser.hpp>
#include "prop_ref_coll.h"
#include "prop_ref_coll_wrappers.h"
#include "NGP_structs.h"
#include "xml_tags.h"

#define EPS_10				1e-10

enum FWUnitsEnum
{
	fwIDR_nothing					= 0,
	fwIDR_acceleration				= 1,
	fwIDR_angle						= 2,
	fwIDR_area						= 3,
	fwIDR_density					= 4,
	fwIDR_dynamic_viscosity			= 5,
	fwIDR_energy					= 6,
	fwIDR_force						= 7,
	fwIDR_heat_flow					= 8,
	fwIDR_kinematic_viscosity		= 9,
};

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

//////////////////////////////////////////////////////////////////////////

void AddSharedArray( const ngp_shared_array<char>& array, const wchar_ngp* tag, CPropertyTreeNode& coll )
{
	CPropertyTreeNode& node = tag ? coll.AddSubGroup(tag) : coll;
	node.Add( PROPERTY_CONTAINEROFBINARY_REF(array, L"char_array", ngp_shared_array<char> ) );
}

//////////////////////////////////////////////////////////////////////////

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
		//ngp_file_io::INGPFileIOPtr bin_file = ngp_file_io::CreateFileIO();	\
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

#define CHECK_EQUAL_FEATURES( f1, f2 ) \
	BOOST_CHECK_MESSAGE( *f1==*f2,                ut_utils::to_string(f1->GetType()) ); \
	BOOST_CHECK_MESSAGE( explicitly_equal(f1,f2), ut_utils::to_string(f1->GetType()) );

//////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE(test_PropertiesSeparately)
{
	const fs::path test_dir = fs::current_path() / L"test_dir";
	/*BOOST_CHECK*/(fs::create_directory(test_dir));

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
//		BOOST_CHECK(CMathUtils::AreEqual10(d1, d2));

		double d3 = 1.1111 + EPS_10;
//		BOOST_CHECK(!CMathUtils::AreEqual10(d1, d3));

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

		auto decompositor = boost::bind(&AddSharedArray, _1, _2, _3);

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
		//CFontDef m_NestedStruct;
		std::list<std::wstring> m_str_list;
		std::list<CNGPString> m_ngpstr_list;
		std::vector<double> m_dbl_list;
		ngp_shared_array<double> m_shared_arr_double;
		ngp_shared_array<CNGPString> m_shared_arr_string;
		//std::map<CNGPString,CFontDef> m_map_of_structs;

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
			//props.Add( PROPERTY_MAP_OF_STRUCT_REF( m_map_of_structs, L"map_of_structs", &XML_serialization_helpers::Add, [](const CNGPString& key) { return std::wstring(key.c_str()); }, [](const std::wstring& key) { return CNGPString(key.c_str()); }  ) );

			//XML_serialization_helpers::Add( m_NestedStruct, L"font", props );
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
	//s1.m_map_of_structs[L"font1"] = CFontDef( L"Times New Roman", 12, 255, false, false );
	//s1.m_map_of_structs[L"font2"] = CFontDef( L"Tahoma", 10, 0, false, true );

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
		/*BOOST_CHECK*/(fs::create_directory(test_dir));

		props1.SaveToBinaryFile( test_dir / utf16_string("_бин_file.bin") );
		STop s3;
		CPropertyRefCollection props3(L"root");
		s3.Fill( props3 );
		props3.LoadFromBinaryFile( test_dir / utf16_string("_бин_file.bin") );

		const bool are_equal = NGP_VERIFY( props1 == props3 );
		//BOOST_CHECK( are_equal );
	}
}

BOOST_AUTO_TEST_CASE(test_FeaturesSerialization)
{
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
//		write_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag1);

		CClass2 obj2;
		wptree tag2;
		//read_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag2);
		//BOOST_CHECK(obj2.Load(tag2));

		//BOOST_CHECK(obj2.m_long == obj1.m_long);
		//BOOST_CHECK(obj2.m_wstring == obj1.m_wstring);
	}

	//////////////////////////////////////////////////////////////////////////

	{
		CClass1 obj1;
		wptree tag1;
		BOOST_CHECK(obj1.Save(tag1));
		const std::string xml_obj1 = "obj1.xml";
//		write_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag1);

		CClass3 obj3;
		wptree tag3;
		//read_xml(CNGPStringA((test_dir / xml_obj1.c_str()).c_str()).c_str(), tag3);
		//BOOST_CHECK(obj3.Load(tag3));

		//BOOST_CHECK(obj3.m_bool == obj1.m_bool);
		//BOOST_CHECK(obj3.m_long == obj1.m_long);
		//BOOST_CHECK(obj3.m_wstring == obj1.m_wstring);
		//BOOST_CHECK(obj3.m_ngpstring == obj1.m_ngpstring);
		//CClass3 obj3_def;
		//BOOST_CHECK(obj3.m_enum == obj3_def.m_enum);
		//BOOST_CHECK(obj3.m_vector == obj3_def.m_vector);
		//BOOST_CHECK(obj3.m_wstring3 == obj1.m_wstring);
		//BOOST_CHECK(obj3.m_def_inited == obj3.DefLongValue);
	}
}
