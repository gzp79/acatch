/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// to avoid registration name conflicts due to includes
#line 60000

namespace ACatch {

struct has_toString {};
struct has_maker {};
struct has_maker_and_toString {};

inline std::string toString( const has_toString& ) {
  return "toString( has_toString )";
}

inline std::string toString( const has_maker_and_toString& ) {
  return "toString( has_maker_and_toString )";
}

template <>
struct StringMaker<has_maker> {
  static std::string convert( const has_maker& ) {
    return "StringMaker<has_maker>";
  }
};

template <>
struct StringMaker<has_maker_and_toString> {
  static std::string convert( const has_maker_and_toString& ) {
    return "StringMaker<has_maker_and_toString>";
  }
};
} // namespace ACatch

namespace ACatchTest {

ACATCH_TEST_CASE( "acatch.toString_which" ) {
  using namespace ACatch;

  // Call the overload
  ACATCH_SECTION( "toString( has_toString )" ) {
    has_toString item;
    ACATCH_REQUIRE( EXPECT, toString( item ) == "toString( has_toString )" );
  }

  // Call the overload
  ACATCH_SECTION( "toString( has_maker )" ) {
    has_maker item;
    ACATCH_REQUIRE( EXPECT, toString( item ) == "StringMaker<has_maker>" );
  }

  // Call the overload
  ACATCH_SECTION( "toString( has_maker_and_toString )" ) {
    has_maker_and_toString item;
    ACATCH_REQUIRE( EXPECT, toString( item ) == "toString( has_maker_and_toString )" );
  }

  // Vectors...
  ACATCH_SECTION( "toString( vectors<has_toString> )" ) {
    std::vector<has_toString> v( 1 );
    ACATCH_REQUIRE_ANY( EXPECT,
        toString( v ) == "{ {?} }",                        // gcc, clang
        toString( v ) == "{ toString( has_toString ) }" ); // msvc
  }

  ACATCH_SECTION( "toString( vectors<has_maker> )" ) {
    std::vector<has_maker> v( 1 );
    ACATCH_REQUIRE( EXPECT, toString( v ) == "{ StringMaker<has_maker> }" );
  }

  ACATCH_SECTION( "toString( vectors<has_maker_and_toString> )" ) {
    std::vector<has_maker_and_toString> v( 1 );
    ACATCH_REQUIRE_ANY( EXPECT,
        toString( v ) == "{ toString( has_maker_and_toString ) }", // gcc, clang
        toString( v ) == "{ StringMaker<has_maker_and_toString> }" ); // msvc
  }

  ACATCH_SECTION( "toString( vectors<has_maker>(2) )" ) {
    std::vector<has_maker> v( 2 );
    ACATCH_REQUIRE( EXPECT, toString( v ) == "{ StringMaker<has_maker>, StringMaker<has_maker> }" );
  }

  ACATCH_SECTION( "toString( vectors<has_maker_and_toString>(2) )" ) {
    std::vector<has_maker_and_toString> v( 2 );
    ACATCH_REQUIRE_ANY( EXPECT,
        toString( v ) == "{ StringMaker<has_maker_and_toString>, StringMaker<has_maker_and_toString> }", // gcc, clang
        toString( v ) == "{ toString( has_maker_and_toString ), toString( has_maker_and_toString ) }" ); // msvc
  }
}

} // namespace ACatchTest
