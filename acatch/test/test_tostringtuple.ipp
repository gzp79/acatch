/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// to avoid registration name conflicts due to includes
#line 40000

namespace ACatchTest {

ACATCH_TEST_CASE( "acatch.toString_tuple" ) {
  using namespace ACatch;

  ACATCH_SECTION( "tuple<>" ) {
    typedef std::tuple<> type;
    ACATCH_REQUIRE( EXPECT, "{ }" == toString( type{} ) );
    type value{};
    ACATCH_REQUIRE( EXPECT, "{ }" == toString( value ) );
  }

  ACATCH_SECTION( "tuple<int>" ) {
    typedef std::tuple<int> type;
    ACATCH_REQUIRE( EXPECT, "{ 0 }" == toString( type{ 0 } ) );
  }

  ACATCH_SECTION( "tuple<float,int>" ) {
    typedef std::tuple<float, int> type;
    ACATCH_REQUIRE( EXPECT, "1.2f" == toString( float( 1.2 ) ) );
    ACATCH_REQUIRE( EXPECT, "{ 1.2f, 0 }" == toString( type{ 1.2f, 0 } ) );
  }

  ACATCH_SECTION( "tuple<string,string>" ) {
    typedef std::tuple<std::string, std::string> type;
    ACATCH_REQUIRE( EXPECT, "{ \"hello\", \"world\" }"
                                == toString( type{ "hello", "world" } ) );
  }

  ACATCH_SECTION( "tuple<tuple<int>,tuple<>,float>" ) {
    typedef std::tuple<std::tuple<int>, std::tuple<>, float> type;
    type value{ std::tuple<int>{ 42 }, {}, 1.2f };
    ACATCH_REQUIRE( EXPECT, "{ { 42 }, { }, 1.2f }" == toString( value ) );
  }

  ACATCH_SECTION( "tuple<nullptr,int,const char *>" ) {
    typedef std::tuple<std::nullptr_t, int, const char*> type;
    type value{ nullptr, 42, "Catch me" };
    ACATCH_REQUIRE( EXPECT, "{ nullptr, 42, \"Catch me\" }" == toString( value ) );
  }
}

} // namespace ACatchTest

