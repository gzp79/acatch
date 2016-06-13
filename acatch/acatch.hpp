/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include "acatch_core.hpp"

#define ACATCH_DO_JOIN( X, Y ) ACATCH_DO2_JOIN( X, Y )
#define ACATCH_DO2_JOIN( X, Y ) X ## Y

#define ACATCH_JOIN2( X, Y ) ACATCH_DO_JOIN( X, Y )
#define ACATCH_JOIN3( X, Y, Z ) ACATCH_JOIN2( ACATCH_JOIN2( X, Y ), Z )
#define ACATCH_JOIN4( X, Y, Z, W ) ACATCH_JOIN2( ACATCH_JOIN2( ACATCH_JOIN2( X, Y ), Z ), W )
#define ACATCH_JOIN5( X, Y, Z, W, Q ) ACATCH_JOIN2( ACATCH_JOIN2( ACATCH_JOIN2( ACATCH_JOIN2( X, Y ), Z ), W ), Q )

#define ACATCH_UNIQUE_NAME_LINE2( name, line ) name ## line
#define ACATCH_UNIQUE_NAME_LINE( name, line ) ACATCH_UNIQUE_NAME_LINE2( name, line )
#define ACATCH_UNIQUE_NAME( name ) ACATCH_UNIQUE_NAME_LINE( name, __LINE__ )

// call a macro for_each argument
#define ACATCH_EXPAND( x ) x
#define ACATCH_VA_FOR_EACH_1( WHAT, x, ... ) WHAT( x )
#define ACATCH_VA_FOR_EACH_2( WHAT, x, ... ) WHAT( x ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_1( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_3( WHAT, x, ... ) WHAT( x ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_2( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_4( WHAT, x, ... ) WHAT( x ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_3( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_5( WHAT, x, ... ) WHAT( x ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_4( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_6( WHAT, x, ... ) WHAT( x ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_5( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_7( WHAT, x, ... ) WHAT( x ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_6( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_8( WHAT, x, ... ) WHAT( x ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_7( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_NARG( ... ) ACATCH_VA_FOR_EACH_NARG_( __VA_ARGS__, ACATCH_VA_FOR_EACH_RSEQ_N() )
#define ACATCH_VA_FOR_EACH_NARG_( ... ) ACATCH_EXPAND( ACATCH_VA_FOR_EACH_ARG_N( __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH_ARG_N( _1, _2, _3, _4, _5, _6, _7, _8, NAME, ... ) NAME
#define ACATCH_VA_FOR_EACH_RSEQ_N() 8, 7, 6, 5, 4, 3, 2, 1, 0
#define ACATCH_VA_FOR_EACH_CONCATENATE( x, y ) x ## y
#define ACATCH_VA_FOR_EACH_( NAME, WHAT, ... ) \
  ACATCH_EXPAND( ACATCH_VA_FOR_EACH_CONCATENATE( ACATCH_VA_FOR_EACH_, NAME )( WHAT, __VA_ARGS__ ) )
#define ACATCH_VA_FOR_EACH( WHAT, ... ) \
  ACATCH_VA_FOR_EACH_( ACATCH_VA_FOR_EACH_NARG( __VA_ARGS__ ), WHAT, __VA_ARGS__ )

#define ACATCH_EVAL_Any( expr ) acatch_internal_exprRes = acatch_internal_exprRes || expr;
#define ACATCH_EVAL_All( expr ) acatch_internal_exprRes = acatch_internal_exprRes && expr;
#define ACATCH_EXPR_STRING( expr ) acatch_internal_exprStr.add( ( ::ACatch::ExpressionCapture( #expr ) <= expr ).getCapture() );

#define ACATCH_MULTI_REQUIRE_INTERNAL( CONCAT, DEFVALUE, ... )                 \
  do {                                                                         \
    bool acatch_internal_exprRes = DEFVALUE;                                   \
    ACATCH_VA_FOR_EACH( ACATCH_JOIN2( ACATCH_EVAL_, CONCAT ), __VA_ARGS__ );   \
    ACATCH_INTERNAL_ASSERT( acatch_internal_exprRes );                         \
  } while( ::ACatch::alwaysFalse() )

#define ACATCH_MULTI_REQUIRE_EXPECT_VERBOSE( CONCAT, DEFVALUE, ... )           \
  do {                                                                         \
    bool acatch_internal_exprRes = DEFVALUE;                                   \
    ACATCH_VA_FOR_EACH( ACATCH_JOIN2( ACATCH_EVAL_, CONCAT ), __VA_ARGS__ );   \
    ::ACatch::MultiExpressionCapture acatch_internal_exprStr(                  \
      ::ACatch::MultiExpressionCapture::CONCAT );                              \
    ACATCH_VA_FOR_EACH( ACATCH_EXPR_STRING, __VA_ARGS__ );                     \
    if( acatch_internal_exprRes ) {                                            \
      ::ACatch::theACatch().handleSuccess( acatch_internal_exprStr );          \
    } else {                                                                   \
      ::ACatch::theACatch().handleFail( acatch_internal_exprStr );             \
    }                                                                          \
  } while( ::ACatch::alwaysFalse() )

#define ACATCH_MULTI_REQUIRE_EXPECT( CONCAT, DEFVALUE, ... )                   \
  do {                                                                         \
    bool acatch_internal_exprRes = DEFVALUE;                                   \
    ACATCH_VA_FOR_EACH( ACATCH_JOIN2( ACATCH_EVAL_, CONCAT ), __VA_ARGS__ );   \
    if( acatch_internal_exprRes ) {                                            \
      ::ACatch::theACatch().handleSuccess();                                   \
    } else {                                                                   \
      ::ACatch::MultiExpressionCapture acatch_internal_exprStr(                \
        ::ACatch::MultiExpressionCapture::CONCAT );                            \
      ACATCH_VA_FOR_EACH( ACATCH_EXPR_STRING, __VA_ARGS__ );                   \
      ::ACatch::theACatch().handleFail( acatch_internal_exprStr );             \
    }                                                                          \
  } while( ::ACatch::alwaysFalse() )

#define ACATCH_MULTI_REQUIRE_EXPECT_FAST( CONCAT, DEFVALUE, ... )              \
  do {                                                                         \
    bool acatch_internal_exprRes = DEFVALUE;                                   \
    ACATCH_VA_FOR_EACH( ACATCH_JOIN2( ACATCH_EVAL_, CONCAT ), __VA_ARGS__ );   \
    if( !acatch_internal_exprRes ) {                                           \
      ::ACatch::MultiExpressionCapture acatch_internal_exprStr(                \
        ::ACatch::MultiExpressionCapture::CONCAT );                            \
      ACATCH_VA_FOR_EACH( ACATCH_EXPR_STRING, __VA_ARGS__ );                   \
      ::ACatch::theACatch().handleFail( acatch_internal_exprStr );             \
    }                                                                          \
  } while( ::ACatch::alwaysFalse() )

#define ACATCH_MULTI_REQUIRE_ASSERT_VERBOSE( CONCAT, DEFVALUE, ... )           \
  do {                                                                         \
    bool acatch_internal_exprRes = DEFVALUE;                                   \
    ACATCH_VA_FOR_EACH( ACATCH_JOIN2( ACATCH_EVAL_, CONCAT ), __VA_ARGS__ );   \
    ::ACatch::MultiExpressionCapture acatch_internal_exprStr(                  \
      ::ACatch::MultiExpressionCapture::CONCAT );                              \
    ACATCH_VA_FOR_EACH( ACATCH_EXPR_STRING, __VA_ARGS__ );                     \
    if( acatch_internal_exprRes ) {                                            \
      ::ACatch::theACatch().handleSuccess( acatch_internal_exprStr );          \
    } else {                                                                   \
      ::ACatch::theACatch().handleAbort( acatch_internal_exprStr );            \
    }                                                                          \
  } while( ::ACatch::alwaysFalse() )

#define ACATCH_MULTI_REQUIRE_ASSERT( CONCAT, DEFVALUE, ... )                   \
  do {                                                                         \
    bool acatch_internal_exprRes = DEFVALUE;                                   \
    ACATCH_VA_FOR_EACH( ACATCH_JOIN2( ACATCH_EVAL_, CONCAT ), __VA_ARGS__ );   \
    if( acatch_internal_exprRes ) {                                            \
      ::ACatch::theACatch().handleSuccess();                                   \
    } else {                                                                   \
      ::ACatch::MultiExpressionCapture acatch_internal_exprStr(                \
        ::ACatch::MultiExpressionCapture::CONCAT );                            \
      ACATCH_VA_FOR_EACH( ACATCH_EXPR_STRING, __VA_ARGS__ );                   \
      ::ACatch::theACatch().handleAbort( acatch_internal_exprStr );            \
    }                                                                          \
  } while( ::ACatch::alwaysFalse() )

#define ACATCH_MULTI_REQUIRE_ASSERT_FAST( CONCAT, DEFVALUE, ... )              \
  do {                                                                         \
    bool acatch_internal_exprRes = DEFVALUE;                                   \
    ACATCH_VA_FOR_EACH( ACATCH_JOIN2( ACATCH_EVAL_, CONCAT ), __VA_ARGS__ );   \
    if( !acatch_internal_exprRes ) {                                           \
      ::ACatch::MultiExpressionCapture acatch_internal_exprStr(                \
        ::ACatch::MultiExpressionCapture::CONCAT );                            \
      ACATCH_VA_FOR_EACH( ACATCH_EXPR_STRING, __VA_ARGS__ );                   \
      ::ACatch::theACatch().handleAbort( acatch_internal_exprStr );            \
    }                                                                          \
  } while( ::ACatch::alwaysFalse() )

// test framework API macros

#define ACATCH_PREINIT()                                                    \
  static void ACATCH_UNIQUE_NAME( acatch_preinit )( );                      \
  namespace {                                                               \
  ::ACatch::AutoReg ACATCH_UNIQUE_NAME( acatch_internal_Autoregister )(     \
    ACATCH_UNIQUE_NAME( acatch_preinit ) );                                 \
  }                                                                         \
  static void ACATCH_UNIQUE_NAME( acatch_preinit )( )

#define ACATCH_TEST_CASE( NAME )                                                         \
  static void ACATCH_UNIQUE_NAME( acatch_internal_TestCase )( );                         \
  namespace {                                                                            \
  ::ACatch::AutoReg ACATCH_UNIQUE_NAME( acatch_internal_Autoregister )(                  \
    ::ACatch::AutoReg::mkFunctionTest( &ACATCH_UNIQUE_NAME( acatch_internal_TestCase )   \
                                       , ::ACatch::TestCaseInfo( NAME ) ) );             \
  }                                                                                      \
  static void ACATCH_UNIQUE_NAME( acatch_internal_TestCase )( )
#define ACATCH_DISABLE_TEST_CASE( ... )                                        \
  static void ACATCH_UNIQUE_NAME( acatch_internal_TestCase )( )

#define ACATCH_TEST_CASE_FIXTURE( QUALIFIEDMETHOD, NAME )                      \
  namespace {                                                                  \
  ::ACatch::AutoReg ACATCH_UNIQUE_NAME( acatch_internal_TestCase )(            \
    ::ACatch::AutoReg::mkFixtureTest( QUALIFIEDMETHOD                          \
                                      , ::ACatch::TestCaseInfo( NAME ) ) );    \
  }
#define ACATCH_DISABLE_TEST_CASE_FIXTURE( ... )

#define ACATCH_TEST_CASE_METHOD( QUALIFIEDMETHOD, NAME )                      \
  namespace {                                                                 \
  ::ACatch::AutoReg ACATCH_UNIQUE_NAME( acatch_internal_TestCase )(         \
    ::ACatch::AutoReg::mkMethodTest( QUALIFIEDMETHOD                        \
                                     , ::ACatch::TestCaseInfo( NAME ) ) );  \
  }
#define ACATCH_DISABLE_TEST_CASE_METHOD( ... )

/// Define a a section block within a test-case.
#define ACATCH_SECTION( name )                                                 \
  if( const ACatch::Section & ACATCH_UNIQUE_NAME( acatch_internal_Section ) =  \
        ::ACatch::SectionInfo( name ) )                                        \
    if( ACATCH_UNIQUE_NAME( acatch_internal_Section ) )

/// Disable a a section block within a test-case.
#define ACATCH_DISABLE_SECTION( ... )  \
  if( ::ACatch::alwaysFalse() )

/// Log an expression
#define ACATCH_CAPTURE( expr )                                                 \
  do {                                                                         \
    std::stringstream ACATCH_UNIQUE_NAME( acatch_internal_ExprCapture );       \
    ACATCH_UNIQUE_NAME( acatch_internal_ExprCapture ) << "\"" << #expr         \
                                                      << "\" = ";              \
    ACATCH_UNIQUE_NAME( acatch_internal_ExprCapture ) << ( expr );             \
    ::ACatch::theACatch().handleLog(                                           \
      ACATCH_UNIQUE_NAME( acatch_internal_ExprCapture ).str() );               \
  } while( ACatch::alwaysFalse() )

/// The testing macros
/// TYPE:
/// * EXPECT            on failure: log expression, continue test case; on success: increment counter
/// * EXPECT_VERBOSE    on failure: log expression, continue test case; on success: log expression
/// * EXPECT_FAST       on failure: log expression, continue test case; on success: do nothing
/// * ASSERT            on failure: log expression, abort test case; on success: increment counter
/// * ASSERT_VERBOSE    on failure: log expression, abort test case; on success: log expression
/// * ASSERT_FAST       on failure: log expression, abort test case; on success: do nothing
/// * INTERNAL          used to signal internal (test framework) errors
#define ACATCH_REQUIRE( TYPE, expr )    ACATCH_JOIN2( ACATCH_MULTI_REQUIRE_, TYPE )( Any, false, expr )
#define ACATCH_REQUIRE_ANY( TYPE, ... ) ACATCH_JOIN2( ACATCH_MULTI_REQUIRE_, TYPE )( Any, false, __VA_ARGS__ )
#define ACATCH_REQUIRE_ALL( TYPE, ... ) ACATCH_JOIN2( ACATCH_MULTI_REQUIRE_, TYPE )( All, true, __VA_ARGS__ )

/// Log user messages and states
#define ACATCH_FAIL( msg )  ::ACatch::theACatch().handleFail( msg )
#define ACATCH_ABORT( msg ) ::ACatch::theACatch().handleAbort( msg )
#define ACATCH_WARN( msg )  ::ACatch::theACatch().handleLog( msg )
#define ACATCH_INFO( msg )  ::ACatch::theACatch().handleLog( msg )

/// Helper to check for asserts in the code
#define ACATCH_SECTION_ASSERT_BEGIN( msg ) ACATCH_SECTION( msg ) { ::ACatch::TestAssertGuard ACATCH_UNIQUE_NAME( acatch_test_guard ); try {
#define ACATCH_SECTION_ASSERT_END( assertFilter ) ACATCH_FAIL( "Assert was required" ); } catch( ::ACatch::TestAssert capturedAssert ) { ACATCH_REQUIRE( ASSERT, ::ACatch::CheckAssert::assertFilter.check( capturedAssert ) ); } }
#define ACATCH_TRIGGER_TESTASSERT( msg ) throw ::ACatch::TestAssert( msg )

/// Report the result of the current test case any time
#define ACATCH_REPORT_NOW ::ACatch::theACatch().reportNow()


#ifdef ACATCH_SELFTEST
#  include "acatch/test/test_exceptiontests.ipp"
#  include "acatch/test/test_parttracker.ipp"
#  include "acatch/test/test_tostringpair.ipp"
#  include "acatch/test/test_tostringtuple.ipp"
#  include "acatch/test/test_tostringvector.ipp"
#  include "acatch/test/test_tostringwhich.ipp"
#endif // ACATCH_SELFTEST
