#pragma once

#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>

#include "boost/test/test_tools.hpp"
#include "boost/filesystem.hpp"
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
namespace fs = boost::filesystem;

#define NGP_VERIFY(c)		(c)
#define NGP_REQUIRE(c)		if (!(c)) return;
#define NGP_REQUIRE_b(c)	if (!(c)) return false;
#define ASSERT assert

#include "cpp_compiler_features.h"

typedef wchar_t wchar_ngp;
#include "CNGPString.h"
