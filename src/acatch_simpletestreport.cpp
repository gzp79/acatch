/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

namespace ACatch {

SimpleTestReport::SimpleTestReport()
    : mVerbose( true )
    , mDepth( 0 ) {
}


void SimpleTestReport::setProperty( const std::string& aProp, const std::string& aValue ) {
  if( aProp == "verbose" ) {
    mVerbose = ( aValue == "true" );
  }
}


void SimpleTestReport::reportTestCases( const ConstTestCaseInfoRefs& aInfos ) {
  std::cout << "testcases: (" << aInfos.size() << ")\n";
  for( const TestCaseInfo* tc : aInfos )
    std::cout << "  " << tc->name << " \n";
}


void SimpleTestReport::reportTestCaseSkip( const TestCaseInfo& aInfo ) {
  mNames.clear();
  mNames.push_back( aInfo.name );
  mDepth = 1;
  printTestName( State::Skip );
}


void SimpleTestReport::reportTestCaseStart( const TestCaseInfo& aInfo ) {
  mNames.clear();
  mNames.push_back( aInfo.name );
  mDepth = 1;
  printTestName( State::Runing );
}


void SimpleTestReport::reportTestSectionSkip( const SectionInfo& aInfo ) {
  mNames.push_back( aInfo.name );
  printTestName( State::Skip );
}


void SimpleTestReport::reportTestSectionStart( const SectionInfo& aInfo ) {
  mNames.push_back( aInfo.name );
  ++mDepth;
  ACATCH_INTERNAL_ASSERT( mDepth == mNames.size() );
  printTestName( State::Runing );
}


void SimpleTestReport::reportTestSectionEnd( const SectionInfo& /*aInfo*/, TestCaseResult& aResult ) {
  printTestResult( aResult, true );
  --mDepth;
}


void SimpleTestReport::reportTestCaseEnd( const TestCaseInfo& /*aInfo*/, TestCaseResult& aResult ) {
  printTestResult( aResult, true );
  --mDepth;
  ACATCH_INTERNAL_ASSERT( mDepth == 0 );
}


void SimpleTestReport::reportLogNow( TestCaseResult& aResult ) {
  printTestResult( aResult, false );
}


void SimpleTestReport::reportTestRun( const ConstTestCaseInfoRefs& aInfos, TestRunResult& aRunResult ) {
  std::cout << "\nSummary: " << ( aRunResult.getResult() ? "PASSED\n" : "FAILED\n" );
  std::cout << "  Test groups:       " << aInfos.size() << "\n";
  std::cout << "  Failed tests:      " << aRunResult.getFailedTestCount() << "\n";
  std::cout << "  Passed tests:      " << aRunResult.getPassedTestCount() << "\n";
  std::cout << "  Failed assertions: " << aRunResult.getFailedAssertionCount() << "\n";
  std::cout << "  Passed assertions: " << aRunResult.getPassedAssertionCount() << "\n";
}


/// Reports the current test results
void SimpleTestReport::printTestResult( TestCaseResult& aResult, bool aCompleted ) {
  TestCaseResult::Logs logs;
  if( aResult.takeLogs( logs ) ) {
    if( aResult.isFailed() ) {
      if( aResult.isAborting() ) {
        printTestName( State::Aborted );
      } else {
        printTestName( State::Failed );
      }
    } else {
      printTestName( State::Success );
    }

    for( auto& l : logs )
      std::cout << "    " << l.second << "\n";
  } else if( !aCompleted ) {
    printTestName( State::Runing );
  }
}


/// Reports the name of the current test
void SimpleTestReport::printTestName( State aState ) {
  if( !mVerbose && ( aState == State::Runing || aState == State::Skip ) ) {
    return;
  }

  switch( aState ) {
    case State::Runing:
      std::cout << "? ";
      break;
    case State::Failed:
      std::cout << "! ";
      break;
    case State::Success:
      std::cout << "+ ";
      break;
    case State::Aborted:
      std::cout << "!!";
      break;
    case State::Skip:
      std::cout << ". ";
      break;
  }

  for( size_t i = 0; i < mNames.size(); ++i ) {
    if( i == mDepth )
      std::cout << "(";
    if( i > 0 )
      std::cout << ".";
    std::cout << mNames[i];
  }
  if( mDepth != mNames.size() )
    std::cout << ")";

  if( aState == State::Runing )
    std::cout << "\r";
  else
    std::cout << std::endl;
}


} // namespace ACatch
