/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// to avoid registration name conflicts due to includes
#line 30000

namespace ACatch {
// Note: If we put this in the right place in catch_tostring, then
// we can make it an overload of Catch::toString
template <typename T1, typename T2>
struct StringMaker<std::pair<T1, T2>> {
  static std::string convert( const std::pair<T1, T2>& pair ) {
    std::ostringstream oss;
    oss << "{ "
        << toString( pair.first )
        << ", "
        << toString( pair.second )
        << " }";
    return oss.str();
  }
};
} // namespace ACatch

namespace ACatchTest {

ACATCH_TEST_CASE( "acatch.tostring_pair" ) {
  using namespace ACatch;

  ACATCH_SECTION( "pair<int,string>" ) {
    std::pair<int, std::string> value( 34, "xyzzy" );
    ACATCH_REQUIRE( EXPECT, toString( value ) == "{ 34, \"xyzzy\" }" );
  }

  ACATCH_SECTION( "pair<int,const string>" ) {
    std::pair<int, const std::string> value( 34, "xyzzy" );
    ACATCH_REQUIRE( EXPECT, toString( value ) == "{ 34, \"xyzzy\" }" );
  }

  ACATCH_SECTION( "vector<pair<string,int> >" ) {
    std::vector<std::pair<std::string, int>> pr;
    pr.push_back( std::make_pair( "green", 55 ) );
    ACATCH_REQUIRE( EXPECT, toString( pr ) == "{ { \"green\", 55 } }" );
  }

  // This is pretty contrived - I figure if this works, anything will...
  ACATCH_SECTION( "pair<pair<int,const char *,pair<string,int> >" ) {
    typedef std::pair<int, const char*> left_t;
    typedef std::pair<std::string, int> right_t;

    left_t left( 42, "Arthur" );
    right_t right( "Ford", 24 );

    std::pair<left_t, right_t> pair( left, right );
    ACATCH_REQUIRE( EXPECT, toString( pair ) == "{ { 42, \"Arthur\" }, { \"Ford\", 24 } }" );
  }
}

} // namespace ACatchTest
