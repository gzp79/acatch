/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// to avoid registration name conflicts due to includes
#line 50000

namespace {
/* Minimal Allocator */
template <typename T>
struct minimal_allocator : public std::allocator<T> {
  minimal_allocator() {
  }

  template <class U>
  minimal_allocator( const minimal_allocator<U>& ) {
  }

  template <class U>
  struct rebind {
    typedef minimal_allocator<U> other;
  };

  typedef std::allocator<T> base;
  typename base::pointer
  allocate( typename base::size_type n,
            std::allocator<void>::const_pointer /*hint*/ = nullptr ) {
    return ( T* )malloc( sizeof( T ) * n );
  }

  void deallocate( typename base::pointer p, typename base::size_type /*n*/ ) {
    free( p );
  }
};
} // namespace

namespace ACatchTest {

ACATCH_TEST_CASE( "acatch.toString_vector" ) {
  using namespace ACatch;

  ACATCH_SECTION( "vector<int>" ) {
    std::vector<int> vv;
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{  }" );
    vv.push_back( 42 );
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{ 42 }" );
    vv.push_back( 250 );
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{ 42, 250 }" );
  }

  ACATCH_SECTION( "vector<string>" ) {
    std::vector<std::string> vv;
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{  }" );
    vv.push_back( "hello" );
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{ \"hello\" }" );
    vv.push_back( "world" );
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{ \"hello\", \"world\" }" );
  }

  ACATCH_SECTION( "vector<int,allocator>" ) {
    std::vector<int, minimal_allocator<int>> vv;
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{  }" );
    vv.push_back( 42 );
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{ 42 }" );
    vv.push_back( 250 );
    ACATCH_REQUIRE( EXPECT, toString( vv ) == "{ 42, 250 }" );
  }

  ACATCH_SECTION( "vector<vector<string,allocator>>" ) {
    typedef std::vector<std::string, minimal_allocator<std::string>> inner;
    typedef std::vector<inner> vector;
    vector v;
    ACATCH_REQUIRE( EXPECT, toString( v ) == "{  }" );
    v.push_back( inner{ "hello" } );
    v.push_back( inner{ "world" } );
    ACATCH_REQUIRE( EXPECT, toString( v ) == "{ { \"hello\" }, { \"world\" } }" );
  }
}

} // namespace ACatchTest

