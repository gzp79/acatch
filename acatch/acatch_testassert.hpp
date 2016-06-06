/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

namespace ACatch {

class ACATCH_API TestAssertGuard {
public:
  TestAssertGuard() {
    theACatch().mInAssertTest = true;
  }

  ~TestAssertGuard() {
    theACatch().mInAssertTest = false;
  }
};

namespace CheckAssert {

/// Accept all assert
class ACATCH_API AcceptAll {
public:
  static bool check( const TestAssert& ) {
    return true;
  }
};

} // namespace CheckAssert

} // namespace ACatch
