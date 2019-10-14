#include "pch.h"
#include <boost/test/unit_test.hpp>

#include <set>
#include <codecvt>
#include <type_traits>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/make_shared.hpp>
#include <boost/make_unique.hpp>

#include "mpl.h"

//////////////////////////////////////////////////////////////////////////

/* Target types:
+ fundamental
+ string/wstring
- CNGPString
+ enum
+ static array
+ vector
+ set
- map
- ngp_shared_array
+ shared_ptr
+ custom struct
*/

// custom types
enum enum_t { enum1, enum2, enum3, enum4, enum5 };

static const std::vector<int> ref_vec_of_ints = { 1, 2, 3 };
static const std::vector<enum_t> ref_vec_of_enums = { enum1, enum2, enum3 };
static const std::set<enum_t> ref_set_of_enums = { enum1, enum2, enum3 };
static const std::map<enum_t,double> ref_map = { {enum1,1.1}, {enum2,2.2}, {enum3,3.3} };
static const std::vector<std::vector<int>> ref_vec_of_vec_of_ints = { {1,2,3}, {4,5} };

//////////////////////////////////////////////////////////////////////////

// basic declarations
namespace prc
{
	template <class T, class = void>
	struct copy_adapter
	{
		static void process( const T& _in, T& _out )
		{
			_out = _in;
		}
	};

	template <class T, class = void>
	struct equal_adapter
	{
		static bool process( const T& lhs, const T& rhs )
		{
			return lhs == rhs;
		}
	};

	template <class S, class T, class = void>
	struct io_adapter
	{
		static void save( S& stream, const wchar_t* tag, const T& value )
		{
			static_assert( false, "cannot serialize to" );
		}
		static void load( const S& stream, const wchar_t* tag, T& value )
		{
			static_assert( false, "cannot deserialize from" );
		}
	};

	// Main 4 Ops

	template <class T>
	void op_copy( const T& _in, T& _out )
	{
		copy_adapter<T>::process( _in, _out );
	}

	template <class T>
	bool op_equal( const T& lhs, const T& rhs )
	{
		return equal_adapter<T>::process( lhs, rhs );
	}

	template <class S, class T>
	void op_save( S& stream, const wchar_t* tag, const T& value )
	{
		io_adapter<S,T>::save( stream, tag, value );
	}

	template <class S, class T>
	void op_load( const S& stream, const wchar_t* tag, T& value )
	{
		io_adapter<S,T>::load( stream, tag, value );
	}
}

//////////////////////////////////////////////////////////////////////////

namespace prc
{
	// 'copy' specific implementations

	template <class T>
	struct copy_adapter< T, typename std::enable_if_t<std::is_array_v<T>> >
	{
		static void process( const T& _in, T& _out )
		{
			std::copy_n( std::begin(_in), std::size(_in), std::begin(_out) );
		}
	};

	template <class T>
	struct copy_adapter< T, typename std::enable_if_t<mpl::is_smart_ptr<T>::value> >
	{
		static void process( const T& _in, T& _out )
		{
			if ( _in )
				_out.reset( new T::element_type( *_in ) );
			else
				_out.reset();
		}
	};

	template <class T>
	struct copy_adapter< T, typename std::enable_if_t<mpl::is_smart_ptr<typename T::value_type>::value> >
	{
		static void process( const T& _in, T& _out )
		{
			static_assert( false, "std containers of smart pointers not yet supported" );
		}
	};

	// 'equal' specific implementations

	template <class T>
	struct equal_adapter< T, typename std::enable_if_t<std::is_array_v<T>> >
	{
		static bool process( const T& lhs, const T& rhs )
		{
			return std::equal( std::begin(lhs), std::end(lhs), std::begin(rhs), std::end(rhs) );
		}
	};

	template <class T>
	struct equal_adapter< T, typename std::enable_if_t<mpl::is_smart_ptr<T>::value> >
	{
		static bool process( const T& lhs, const T& rhs )
		{
			return lhs.get() == rhs.get() ||
				lhs && rhs && equal_adapter<T::element_type>::process( *lhs, *rhs );
		}
	};

	template <class T>
	struct equal_adapter< T, typename std::enable_if_t<mpl::is_smart_ptr<typename T::value_type>::value> >
	{
		static bool process( const T& lhs, const T& rhs )
		{
			static_assert(false, "std containers of smart pointers not yet supported");
		}
	};
}

//////////////////////////////////////////////////////////////////////////

// ptree
namespace prc
{
	template <class T, class = void>
	struct io_ptree_adapter
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
	struct io_adapter< boost::property_tree::wptree, T >
	{
		static void save( boost::property_tree::wptree& stream, const wchar_t* tag, const T& value )
		{
			boost::property_tree::wptree node;
			io_ptree_adapter<T>::save( node, value );
			stream.put_child( tag, node );
		}
		static void load( const boost::property_tree::wptree& stream, const wchar_t* tag, T& value )
		{
			boost::optional<const boost::property_tree::wptree&> node = stream.get_child_optional( tag );
			if ( node )
			{
				io_ptree_adapter<T>::load( *node, value );
			}
		}
	};

	template <>
	struct io_ptree_adapter< std::string >
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
	struct io_ptree_adapter< T, typename std::enable_if_t<std::is_enum_v<T>> >
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
	struct io_ptree_adapter< T, typename std::enable_if_t<std::is_array_v<T>> >
	{
		static void save( boost::property_tree::wptree& node, const T& arr )
		{
			for ( const auto& value : arr )
			{
				boost::property_tree::wptree item_node;
				io_ptree_adapter<std::remove_all_extents_t<T>>::save( item_node, value );
				node.add_child( L"item", item_node );
			}
		}
		static void load( const boost::property_tree::wptree& node, T& arr )
		{
			size_t i = 0;
			for ( const boost::property_tree::wptree::value_type& item_node : node )
			{
				if ( !( i < std::size(arr) ) )
					break;
				_ASSERTE( item_node.first == L"item" );
				io_ptree_adapter<std::remove_all_extents_t<T>>::load( item_node.second, arr[i] );
				i++;
			}
		}
	};

	template <class T>
	struct io_ptree_adapter< std::vector<T> >
	{
		using CONTAINER = std::vector<T>;
		static void save( boost::property_tree::wptree& node, const CONTAINER& container )
		{
			for ( const auto& value : container )
			{
				boost::property_tree::wptree item_node;
				io_ptree_adapter<T>::save( item_node, value );
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
				io_ptree_adapter<T>::load( item_node.second, item );
				container.push_back( item );
			}
		}
	};

	template <class T>
	struct io_ptree_adapter< std::set<T> >
	{
		using CONTAINER = std::set<T>;
		static void save( boost::property_tree::wptree& node, const CONTAINER& container )
		{
			for ( const auto& value : container )
			{
				boost::property_tree::wptree item_node;
				io_ptree_adapter<T>::save( item_node, value );
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
				io_ptree_adapter<T>::load( item_node.second, item );
				container.insert( item );
			}
		}
	};

	template <class KEY_TYPE, class VALUE_TYPE>
	struct io_ptree_adapter< std::map<KEY_TYPE,VALUE_TYPE> >
	{
		using CONTAINER = std::map<KEY_TYPE,VALUE_TYPE>;
		static void save( boost::property_tree::wptree& node, const CONTAINER& container )
		{
			for ( const auto& value : container )
			{
				boost::property_tree::wptree item_node;
				io_ptree_adapter<T>::save( item_node, value );
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
				io_ptree_adapter<T>::load( item_node.second, item );
				container.insert( item );
			}
		}
	};

	template <class T>
	struct io_ptree_adapter< T, typename std::enable_if_t<mpl::is_smart_ptr<T>::value> >
	{
		static void save( boost::property_tree::wptree& node, const T& sp )
		{
			if ( sp )
				io_ptree_adapter<T::element_type>::save( node, *sp );
		}
		static void load( const boost::property_tree::wptree& node, T& sp )
		{
			BOOST_ASSERT( std::is_default_constructible_v<T::element_type> );
			if ( boost::optional<T::element_type> node_val = node.get_value_optional<T::element_type>() )
			{
				if ( !sp )
					sp.reset( new T::element_type );
				io_ptree_adapter<T::element_type>::load( node, *sp );
			}
			//else
			//	sp.reset();
		}
	};

	template <class T>
	struct io_ptree_adapter< T, typename std::void_t<decltype(std::declval<T>().save( std::declval<boost::property_tree::wptree&>() ) ),
												     decltype(std::declval<T>().load( std::declval<const boost::property_tree::wptree&>() ) )> >
	{
		static void save( boost::property_tree::wptree& node, const T& value )
		{
			value.save( node );
		}
		static void load( const boost::property_tree::wptree& node, T& value )
		{
			value.load( node );
		}
	};

	template <class T>
	struct io_ptree_adapter< T, typename std::void_t<decltype(save_to_ptree( std::declval<boost::property_tree::wptree&>(), std::declval<const T&>() )),
												     decltype(load_from_ptree( std::declval<const boost::property_tree::wptree&>(), std::declval<T&>() ))> >
	{
		static void save( boost::property_tree::wptree& node, const T& value )
		{
			save_to_ptree( node, value );
		}
		static void load( const boost::property_tree::wptree& node, T& value )
		{
			load_from_ptree( node, value );
		}
	};
}

//////////////////////////////////////////////////////////////////////////

#define TEST_COPY_EQUAL( T, ref_value ) \
	{ \
		const T src( ref_value ); \
		T dst; \
		prc::op_copy( src, dst ); \
		BOOST_CHECK( prc::op_equal( src, dst ) ); \
	}

#define TEST_IO_FILE( T, ref_value, filename ) \
	{ \
		const T rhs( ref_value ); \
		T lhs; \
		boost::property_tree::wptree pt; \
		prc::op_save( pt, L"tag", rhs ); \
		prc::op_load( pt, L"tag", lhs ); \
		BOOST_CHECK( prc::op_equal( lhs, rhs ) ); \
		if ( filename ) { boost::property_tree::xml_writer_settings<std::wstring> xmlps; xmlps.indent_count = 4; boost::property_tree::write_xml( filename, pt, std::locale(), xmlps ); } \
	}

#define TEST_IO( T, ref_value ) \
	TEST_IO_FILE( T, ref_value, nullptr )

#define TEST_ALL_OPS( T, ref_value ) \
	TEST_COPY_EQUAL( T, ref_value ); \
	TEST_IO_FILE( T, ref_value, nullptr );


BOOST_AUTO_TEST_CASE( test_std_types )
{
	TEST_ALL_OPS( int, 273 );
	TEST_ALL_OPS( double, 3.14 );
	TEST_ALL_OPS( enum_t, enum2 );
	TEST_ALL_OPS( std::wstring, L"sample string" );
	TEST_ALL_OPS( std::string, "sample string" );
	TEST_ALL_OPS( std::vector<int>, ref_vec_of_ints/*, "vec_of_ints.xml"*/ );
	TEST_ALL_OPS( std::vector<enum_t>, ref_vec_of_enums );
	TEST_ALL_OPS( std::set<enum_t>, ref_set_of_enums );
	//TEST_IO( std::map<enum_t,double>, ref_map, L"map" );
	TEST_ALL_OPS( std::vector<std::vector<int>>, ref_vec_of_vec_of_ints/*, "vec_of_vec_of_ints.xml"*/ );
	TEST_ALL_OPS( std::shared_ptr<int>, std::make_shared<int>( 273 ) );
	TEST_ALL_OPS( std::unique_ptr<int>, std::make_unique<int>( 273 ) );
	TEST_ALL_OPS( boost::shared_ptr<int>, boost::make_shared<int>( 273 ) );
	TEST_ALL_OPS( boost::scoped_ptr<int>, ( new int(273) ) );
	{
		const std::vector<std::shared_ptr<int>> src = { std::make_shared<int>( 1 ), std::make_shared<int>( 2 ), std::make_shared<int>( 3 ) };
// 		{
// 			std::vector<std::shared_ptr<int>> dst;
// 			prc::op_copy( src, dst );
// 			BOOST_REQUIRE_EQUAL( dst.size(), src.size() );
// 			BOOST_CHECK( dst[0].get() != src[0].get() );
// 			BOOST_CHECK( prc::op_equal( src, dst ) );
// 		}
// 		{
// 			std::vector<std::shared_ptr<int>> dst;
// 			boost::property_tree::wptree pt;
// 			prc::op_save( pt, L"vec_sp", src );
// 			prc::op_load( pt, L"vec_sp", dst );
// 			BOOST_REQUIRE_EQUAL( dst.size(), src.size() );
// 			BOOST_CHECK( dst[0].get() != src[0].get() );
// 			BOOST_CHECK( prc::op_equal( src, dst ) );
// 		}
	}
	{
		const int src[3] = { 1, 2, 3 };
		{
			int dst[3];
			prc::op_copy( src, dst );
			BOOST_CHECK( prc::op_equal( src, dst ) );
		}
		{
			int dst[3];
			boost::property_tree::wptree pt;
			prc::op_save( pt, L"tag", src );
			prc::op_load( pt, L"tag", dst );
			BOOST_CHECK( prc::op_equal( src, dst ) );
		}
	}
}

class MyClass_builtin
{
public:
	MyClass_builtin() = default;
	explicit MyClass_builtin(enum_t e) : _EValue(e) {}

	bool operator==( const MyClass_builtin& rhs ) const { return _EValue == rhs._EValue; }

	void save( boost::property_tree::wptree& tree ) const
	{
		tree.add( L"e-value", (long&) _EValue );
		tree.add( L"i-value", _IValue );
	}

	void load( const boost::property_tree::wptree& tree )
	{
		if ( boost::optional<const boost::property_tree::wptree&> item_node = tree.get_child_optional( L"e-value" ) )
			if ( boost::optional<long> node_val = item_node->get_value_optional<long>() )
				_EValue = (enum_t)*node_val;
		if ( boost::optional<const boost::property_tree::wptree&> item_node = tree.get_child_optional( L"i-value" ) )
			if ( boost::optional<int> node_val = item_node->get_value_optional<int>() )
				_IValue = *node_val;
	}

private:
	enum_t _EValue = enum2;
	int _IValue = 7;
};

class MyClass_external
{
public:
	MyClass_external() = default;
	explicit MyClass_external(enum_t e) : _EValue(e) {}

	bool operator==( const MyClass_external& rhs ) const { return _EValue == rhs._EValue; }

	enum_t _EValue = enum2;
	int _IValue = 7;
};

void save_to_ptree( boost::property_tree::wptree& tree, const MyClass_external& obj )
{
	tree.add( L"e-value", (long&) obj._EValue );
	tree.add( L"i-value", obj._IValue );
}

void load_from_ptree( const boost::property_tree::wptree& tree, MyClass_external& obj )
{
	if ( boost::optional<const boost::property_tree::wptree&> item_node = tree.get_child_optional( L"e-value" ) )
		if ( boost::optional<long> node_val = item_node->get_value_optional<long>() )
			obj._EValue = (enum_t)*node_val;
	if ( boost::optional<const boost::property_tree::wptree&> item_node = tree.get_child_optional( L"i-value" ) )
		if ( boost::optional<int> node_val = item_node->get_value_optional<int>() )
			obj._IValue = *node_val;
}

BOOST_AUTO_TEST_CASE( Test_Custom_Struct )
{
	{
		const MyClass_builtin src( enum3 );
		TEST_COPY_EQUAL( MyClass_builtin, src );
		{
			MyClass_builtin dst;
			boost::property_tree::wptree pt;
			prc::op_save( pt, L"tag", src );
			prc::op_load( pt, L"tag", dst );
// 			src.save( pt, L"tag" );
// 			dst.load( pt, L"tag" );
			BOOST_CHECK( prc::op_equal( dst, src ) );
		}
	}
	{
		const MyClass_external src( enum3 );
		TEST_COPY_EQUAL( MyClass_external, src );
		{
			MyClass_external dst;
			boost::property_tree::wptree pt;
			prc::op_save( pt, L"tag", src );
			prc::op_load( pt, L"tag", dst );
			BOOST_CHECK( prc::op_equal( dst, src ) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
