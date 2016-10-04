/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

// configuration defines:
//#define ACATCH_API                   ... api linkage
//#define ACATCH_INTERNAL_ASSERT(...)  ... assert used for the internal erros of the test framework
//#define ACATCH_SELFTEST              ... enable self test
//#define ACATCH_SELFTEST_MUSTFAIL     ... enable self test those are successfull on failure
//#define ACATCH_BREAK                 ... the os dependent break-on-debugger command (nop by default)

#include "acatch_config.hpp"

#if !defined( ACATCH_API ) || !defined( ACATCH_INTERNAL_ASSERT ) || !defined( ACATCH_BREAK )
#  error "Some required define was not provided"
#endif

#include <algorithm>
#include <atomic>
#include <mutex>
#include <memory>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace ACatch {

class ACATCH_API Framework;
extern ACATCH_API Framework& theACatch();
extern ACATCH_API void theACatchShutdown();
extern ACATCH_API bool isFailed();
extern ACATCH_API bool isAborting();
extern ACATCH_API void fatal( const std::string& aMessage );

enum EBreak {
  Break_Never,
  Break_Critical,
  Break_Abort,
  Break_Fail,
};

inline bool constexpr alwaysTrue() {
  return true;
}


inline bool constexpr alwaysFalse() {
  return false;
}


struct TestFailureException {};

/// During test for assertion this exception is thrown
struct TestAssert
{
  TestAssert() {
  }

  TestAssert( const std::string& aMsg )
      : msg( aMsg ) {
  }

  std::string msg;
};

typedef unsigned int uint;

} // namespace ACatch

#include "acatch/acatch_string.hpp"
#include "acatch/acatch_tostring.hpp"
#include "acatch/acatch_expressioncapture.hpp"
#include "acatch/acatch_fatalcondition.hpp"
#include "acatch/acatch_registry.hpp"
#include "acatch/acatch_section.hpp"
#include "acatch/acatch_testcaseresult.hpp"
#include "acatch/acatch_testcasetracker.hpp"
#include "acatch/acatch_testreport.hpp"
#include "acatch/acatch_framework.hpp"
#include "acatch/acatch_testassert.hpp"

#include "acatch/acatch_simpletestreport.hpp"
