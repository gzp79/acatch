/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// to avoid registration name conflicts due to includes
#line 10000

namespace ACatchTest {

namespace {

inline int thisThrows() {
  using namespace ACatch;
  if( alwaysTrue() )
    throw std::domain_error( "expected exception" );
  return 1;
}

class CustomException {
public:
  CustomException( const std::string& msg )
      : m_msg( msg ) {
  }

  std::string getMessage() const {
    return m_msg;
  }

private:
  std::string m_msg;
};

} // namespace

#ifdef ACATCH_SELFTEST_MUSTFAIL
ACATCH_TEST_CASE( "fail_acatch.exception1" ) {
  using namespace ACatch;

  ACATCH_SECTION( "unexpected" ) {
    if( alwaysTrue() )
      throw std::domain_error( "unexpected exception" );
  }
}

ACATCH_TEST_CASE( "fail_acatch.exception2" ) {
  ACATCH_SECTION( "require_unexpected" ) {
    ACATCH_REQUIRE( EXPECT, thisThrows() == 0 );
    ACATCH_FAIL( "This should never happen" );
  }
}
#endif // ACATCH_SELFTEST_MUSTFAIL

ACATCH_TEST_CASE( "acatch.exception" ) {
  ACATCH_SECTION( "cought" ) {
    try {
      throw std::domain_error( "unexpected exception" );
    } catch( ... ) {
    }
  }
}

} // namespace ACatchTest
