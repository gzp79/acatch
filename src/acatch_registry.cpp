/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

namespace ACatch {

struct ITestCase_LexSort {
  bool operator()( const ITestCase* a1, const ITestCase* a2 ) const {
    const std::string n1 = a1->testInfo().name;
    const std::string n2 = a2->testInfo().name;
    int ncmp = n1.compare( n2 );
    if( ncmp != 0 )
      return ncmp < 0;

    //"stable" sort
    return a1 < a2;
  }
};

std::vector<ITestCase*> TestRegistry::getAllTests( RunOrder aOrder ) const {
  std::vector<ITestCase*> tests;
  tests.reserve( mFunctions.size() );
  for( const ATestCase& tc : mFunctions ) {
    tests.push_back( tc.get() );
  }

  switch( aOrder ) {
  case RunOrder::InLexicographicalOrder:
    std::sort( tests.begin(), tests.end(), ITestCase_LexSort() );
    break;

  case RunOrder::InRandomOrder:
    std::random_shuffle( tests.begin(), tests.end() );
    break;

  case RunOrder::InDeclarationOrder:
    // already in declaration order
    break;
  }

  return tests;
}

void AutoReg::registerTestCase( ITestCase* aTestCase ) {
  theACatch().registerTestCase( aTestCase );
}

void AutoReg::registerPreInit( FnPreInit aPreInit ) {
	theACatch().registerPreInit( aPreInit );
}

} // namespace ACatch
