#include "stdafx.h"
#include <codecvt>
#include <boost/test/unit_test.hpp>
#include "prop_ref_coll.h"
#include "prop_ref_coll_wrappers2.h"

//////////////////////////////////////////////////////////////////////////

template <typename T>
CProperty* wrap( T& var, const wchar_ngp* xml_tag )
{
	return new CPropertySimple<T>( &var, xml_tag );
}

//////////////////////////////////////////////////////////////////////////

enum TestEnum { e_item1, e_item2, e_item3, e_item4, e_item5 };

BOOST_AUTO_TEST_CASE( Test_Add )
{
	struct Struct
	{
		// atom types
		int _I = 0;
		double _D = 0;
		std::wstring m_S;
		CNGPString _ngpS;
		TestEnum _Enum = e_item1;
		// container types
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
			props.Add( wrap( _I, L"int" ) );
			props.Add( wrap( _D, L"double" ) );
			props.Add( wrap( m_S, L"string" ) );
			props.Add( wrap( _ngpS, L"CNGPString" ) );
			props.Add( wrap( _Enum, L"enum" ) );
			//props.Add( PROPERTY_SIMPLEARR_REF( m_A, L"arr", double ) );
			//props.Add( PROPERTY_CONTAINEROF_REF( m_str_list, L"str_list", std::list<std::wstring>) );
			//props.Add( PROPERTY_CONTAINEROF_REF( m_ngpstr_list, L"ngpstr_list", std::list<CNGPString>) );
			//props.Add( PROPERTY_CONTAINEROF_REF( m_dbl_list, L"dbl_list", std::vector<double>) );
			//props.Add( PROPERTY_CONTAINEROF_REF( m_shared_arr_double, L"shared_arr_double", ngp_shared_array<double>) );
			//props.Add( PROPERTY_CONTAINEROF_REF( m_shared_arr_string, L"shared_arr_string", ngp_shared_array<CNGPString>) );
			//props.Add( PROPERTY_MAP_OF_STRUCT_REF( m_map_of_structs, L"map_of_structs", &XML_serialization_helpers::Add, [](const CNGPString& key) { return std::wstring(key.c_str()); }, [](const std::wstring& key) { return CNGPString(key.c_str()); }  ) );

			//XML_serialization_helpers::Add( m_NestedStruct, L"font", props );
		}
	};

	Struct s1;
	s1._I = 1;
	s1._D = 3.14;
	s1.m_S = L"тест";
	s1._ngpS = L"ngp string";
	s1._Enum = e_item3;
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

	// restore structure object from xml file
	{
		Struct s2;
		CPropertyRefCollection props2(L"root");
		s2.Fill( props2 );
		props2.LoadFromPTree( xmltree );

		const bool are_equal = NGP_VERIFY( props1 == props2 );
		BOOST_CHECK( are_equal );

		BOOST_CHECK( s1._I == s2._I );
		BOOST_CHECK( s1._D == s2._D );
		BOOST_CHECK( s1.m_S == s2.m_S );
		BOOST_CHECK( s1._ngpS == s2._ngpS );
		BOOST_CHECK( s1._Enum == s2._Enum );
		//BOOST_CHECK( s1.m_str_list == s2.m_str_list );
		//BOOST_CHECK( s1.m_dbl_list == s2.m_dbl_list );
		//BOOST_CHECK( std::equal(s1.m_shared_arr_double.begin(), s1.m_shared_arr_double.end(), s2.m_shared_arr_double.begin()) );
		//BOOST_CHECK( std::equal(s1.m_shared_arr_string.begin(), s1.m_shared_arr_string.end(), s2.m_shared_arr_string.begin()) );
	}
}