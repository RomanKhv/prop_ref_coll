#include "StdAfx.h"
#include <boost/test/unit_test.hpp>

using boost::unit_test::test_suite;

test_suite* init_unit_test_suite( int argc, char* argv[] )
{
	test_suite* ts = BOOST_TEST_SUITE("prop_ref_coll tests");
	return ts;
}
