#include "pch.h"
#include <boost/test/unit_test.hpp>

#include <set>
#include <codecvt>
#include <type_traits>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "mpl.h"

//////////////////////////////////////////////////////////////////////////

/* Target types:
- fundamental
- string/wstring
- CNGPString
- enum
- static array
- vector
- set
- map
- ngp_shared_array
- shared_ptr
- custom struct
*/

// custom types
enum enum_t { enum1, enum2, enum3, enum4, enum5 };

static const std::vector<int> ref_vec_of_ints = { 1, 2, 3 };
static const std::vector<enum_t> ref_vec_of_enums = { enum1, enum2, enum3 };
static const std::set<enum_t> ref_set_of_enums = { enum1, enum2, enum3 };
static const std::map<enum_t,double> ref_map = { {enum1,1.1}, {enum2,2.2}, {enum3,3.3} };
static const std::vector<std::vector<int>> ref_vec_of_vec_of_ints = { {1,2,3}, {4,5} };

//////////////////////////////////////////////////////////////////////////

static std::string _output;
static boost::property_tree::wptree _ptree;

//////////////////////////////////////////////////////////////////////////

// basic declarations
namespace prc
{
	template <class T, class = void>
	struct copy_gate
	{
		static void process( T& lhs, const T& rhs )
		{
			_output = "copy: generic";
			lhs = rhs;
		}
	};

	template <class T>
	void op_copy( T& lhs, const T& rhs )
	{
		copy_gate<T>::process( lhs, rhs );
	}

	template <class T, class = void>
	struct equal_gate
	{
		static bool process( const T& lhs, const T& rhs )
		{
			_output = "equal: generic";
			return lhs == rhs;
		}
	};

	template <class T>
	bool op_equal( const T& lhs, const T& rhs )
	{
		return equal_gate<T>::process( lhs, rhs );
	}
}

//////////////////////////////////////////////////////////////////////////

// 'copy' specific implementations
namespace prc
{
// 	template <class T>
// 	struct copy_gate< T, typename mpl::is_range<T> >
// 	{
// 		static void process( T& lhs, const T& rhs )
// 		{
// 			_output = "range";
// 			lhs.assign( rhs.begin(), rhs.end() );
// 		}
// 	};
// 
// 	template <class T>
// 	struct copy_gate< T, std::set<T> >
// 	{
// 		static void process( T& lhs, const std::set<T>& rhs )
// 		{
// 			_output = "range";
// 			lhs.assign( rhs.begin(), rhs.end() );
// 		}
// 	};
}

// 'equal' specific implementations
namespace prc
{
// 	template <class T>
// 	struct equal_gate< T, typename mpl::is_range<T> >
// 	{
// 		static bool process( const T& lhs, const T& rhs )
// 		{
// 			_output = __FUNCTION__;
// 			if ( lhs.size() != rhs.size() )
// 				return false;
// 			return std::equal( lhs.cbegin(), lhs.cend(), rhs.cbegin() );
// 		}
// 	};
}

//////////////////////////////////////////////////////////////////////////

#define TEST_COPY_EQUAL( T, ref_value ) \
	{ \
		const T rhs = (ref_value); \
		T lhs; \
		prc::op_copy( lhs, rhs ); \
		BOOST_CHECK( prc::op_equal( lhs, rhs ) ); \
	}

BOOST_AUTO_TEST_CASE( test_copy_equal )
{
	TEST_COPY_EQUAL( int, 273 );
	TEST_COPY_EQUAL( double, 3.14 );
	{
		const enum_t e = enum2;		enum_t ee;
		prc::op_copy( ee, e );
		BOOST_CHECK( prc::op_equal( e, ee ) );
	}
	{
		const std::wstring src = L"my string";			std::wstring dst;
		prc::op_copy( dst, src );
		BOOST_CHECK( prc::op_equal( dst, src ) );
	}
	{
		std::vector<int> v = { 1, 2, 3 }, vv;
		prc::op_copy( vv, v );
		BOOST_CHECK( prc::op_equal( v, vv ) );
	}
	{
		const std::string src = "my string";			std::string dst;
		prc::op_copy( dst, src );
		BOOST_CHECK( prc::op_equal( dst, src ) );
	}
}

//////////////////////////////////////////////////////////////////////////

namespace prc
{
	template <class S, class T, class = void>
	struct io_gate
	{
		static void save( S& stream, const wchar_t* tag, const T& value );
// 		{
// 			stream << value;
// 		}
		static void load( const S& stream, const wchar_t* tag, T& value );
// 		{
// 			stream >> value;
// 		}
	};

	template <class S, class T>
	void op_save( S& stream, const wchar_t* tag, const T& value )
	{
		io_gate<S,T>::save( stream, tag, value );
	}

	template <class S, class T>
	void op_load( const S& stream, const wchar_t* tag, T& value )
	{
		io_gate<S,T>::load( stream, tag, value );
	}
}

// ptree
namespace prc
{
	template <class T, class = void>
	struct io_ptree_gate
	{
		static void save( boost::property_tree::wptree& node, const T& value )
		{
			node.put_value( value );
		}
		static void load( const boost::property_tree::wptree& node, T& value )
		{
			if ( boost::optional<T> node_val = node.get_value_optional<T>() )
				value = *node_val;
		}
	};

	template <class T>
	struct io_gate< boost::property_tree::wptree, T >
	{
		static void save( boost::property_tree::wptree& stream, const wchar_t* tag, const T& value )
		{
			boost::property_tree::wptree node;
			io_ptree_gate<T>::save( node, value );
			stream.put_child( tag, node );
		}
		static void load( const boost::property_tree::wptree& stream, const wchar_t* tag, T& value )
		{
			boost::optional<const boost::property_tree::wptree&> node = stream.get_child_optional( tag );
			if ( node )
			{
				io_ptree_gate<T>::load( *node, value );
			}
		}
	};

	template <>
	struct io_ptree_gate< std::string >
	{
		static void save( boost::property_tree::wptree& node, const std::string& value )
		{
			node.put_value<std::wstring>( std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(value) );
		}
		static void load( const boost::property_tree::wptree& node, std::string& value )
		{
			if ( boost::optional<std::wstring> node_val = node.get_value_optional<std::wstring>() )
				value = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().to_bytes( *node_val );
		}
	};

	template <class T>
	struct io_ptree_gate< T, typename std::enable_if_t<std::is_enum_v<T>> >
	{
		static void save( boost::property_tree::wptree& node, const T& value )
		{
			node.put_value( (long&)value );
		}
		static void load( const boost::property_tree::wptree& node, T& value )
		{
			if ( boost::optional<long> node_val = node.get_value_optional<long>() )
				value = (T)*node_val;
		}
	};

	template <class T>
	struct io_ptree_gate< std::vector<T> >
	{
		using CONTAINER = std::vector<T>;
		static void save( boost::property_tree::wptree& node, const CONTAINER& container )
		{
			for ( const auto& value : container )
			{
				boost::property_tree::wptree item_node;
				io_ptree_gate<T>::save( item_node, value );
				node.add_child( L"item", item_node );
			}
		}
		static void load( const boost::property_tree::wptree& node, CONTAINER& container )
		{
			container.clear();
			for ( const boost::property_tree::wptree::value_type& item_node : node )
			{
				_ASSERTE( item_node.first == L"item" );
				T item;
				io_ptree_gate<T>::load( item_node.second, item );
				container.push_back( item );
			}
		}
	};

	template <class T>
	struct io_ptree_gate< std::set<T> >
	{
		using CONTAINER = std::set<T>;
		static void save( boost::property_tree::wptree& node, const CONTAINER& container )
		{
			for ( const auto& value : container )
			{
				boost::property_tree::wptree item_node;
				io_ptree_gate<T>::save( item_node, value );
				node.add_child( L"item", item_node );
			}
		}
		static void load( const boost::property_tree::wptree& node, CONTAINER& container )
		{
			container.clear();
			for ( const boost::property_tree::wptree::value_type& item_node : node )
			{
				_ASSERTE( item_node.first == L"item" );
				T item;
				io_ptree_gate<T>::load( item_node.second, item );
				container.insert( item );
			}
		}
	};

	template <class KEY_TYPE, class VALUE_TYPE>
	struct io_ptree_gate< std::map<KEY_TYPE,VALUE_TYPE> >
	{
		using CONTAINER = std::map<KEY_TYPE,VALUE_TYPE>;
		static void save( boost::property_tree::wptree& node, const CONTAINER& container )
		{
			for ( const auto& value : container )
			{
				boost::property_tree::wptree item_node;
				io_ptree_gate<T>::save( item_node, value );
				node.add_child( L"item", item_node );
			}
		}
		static void load( const boost::property_tree::wptree& node, CONTAINER& container )
		{
			container.clear();
			for ( const boost::property_tree::wptree::value_type& item_node : node )
			{
				_ASSERTE( item_node.first == L"item" );
				T item;
				io_ptree_gate<T>::load( item_node.second, item );
				container.insert( item );
			}
		}
	};
}

#define TEST_IO_FILE( T, ref_value, tag, filename ) \
	{ \
		const T rhs = (ref_value); \
		T lhs; \
		boost::property_tree::wptree pt; \
		prc::op_save( pt, tag, rhs ); \
		prc::op_load( pt, tag, lhs ); \
		BOOST_CHECK( prc::op_equal( lhs, rhs ) ); \
		if ( filename ) { boost::property_tree::xml_writer_settings<std::wstring> xmlps; xmlps.indent_count = 4; boost::property_tree::write_xml( filename, pt, std::locale(), xmlps ); } \
	}

#define TEST_IO( T, ref_value, tag ) \
	TEST_IO_FILE( T, ref_value, tag, nullptr )

#define TEST_ALL_OPS( T, ref_value, tag ) \
	TEST_COPY_EQUAL( T, ref_value ); \
	TEST_IO_FILE( T, ref_value, tag, nullptr );


BOOST_AUTO_TEST_CASE( test_ptree )
{
	TEST_ALL_OPS( int, 273, L"my_int" );
	TEST_ALL_OPS( double, 3.14, L"my_double" );
	TEST_ALL_OPS( enum_t, enum2, L"my_enum" );
	TEST_ALL_OPS( std::wstring, L"sample string", L"my_string" );
	TEST_ALL_OPS( std::string, "sample string", L"my_string" );
	TEST_ALL_OPS( std::vector<int>, ref_vec_of_ints, L"ivec"/*, "vec_of_ints.xml"*/ );
	TEST_ALL_OPS( std::vector<enum_t>, ref_vec_of_enums, L"evec" );
	TEST_ALL_OPS( std::set<enum_t>, ref_set_of_enums, L"eset" );
//	TEST_IO( std::map<enum_t,double>, ref_map, L"map" );
	TEST_ALL_OPS( std::vector<std::vector<int>>, ref_vec_of_vec_of_ints, L"ivecvec"/*, "vec_of_vec_of_ints.xml"*/ );
}

//////////////////////////////////////////////////////////////////////////
