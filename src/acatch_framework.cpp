/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

namespace ACatch {

ACatch::Framework* ACatch::Framework::sInstance = nullptr;

Framework& theACatch() {
  if( !Framework::sInstance )
    Framework::sInstance = new Framework;

  return *Framework::sInstance;
}


void theACatchShutdown() {
  if( Framework::sInstance ) {
    delete Framework::sInstance;
    Framework::sInstance = nullptr;
  }
}


Framework::Framework()
    : mTestReport( new SimpleTestReport() )
    , mTrackerContext( nullptr )
    , mTestCaseTracker( nullptr )
    , mCurrentResult( nullptr )
    , mInAssertTest( false )
    , mPreInitCompleted( false ) {
}


Framework::~Framework() {
  delete mTestReport;
}


/// Add a comma spearted list of filters to select testcases to run
void Framework::addFilter( const std::string& aPattern ) {
  std::stringstream ss( toLower( aPattern ) );
  while( ss.good() ) {
    std::string filter;
    std::getline( ss, filter, ',' );
    mPatterns.push_back( trim( filter ) );
  }
}


bool Framework::matchFilter( const std::string& aName ) {
  if( mPatterns.empty() )
    return true;

  std::string ln = toLower( aName );
  for( const auto& pattern : mPatterns ) {
    if( checkPrefix( pattern, ln ) )
      return true;
  }
  return false;
}


void Framework::registerTestCase( ITestCase* aTestCase ) {
  mTestRegistry.registerTest( aTestCase );
}


void Framework::registerPreInit( FnPreInit aPreInit ) {
  mTestRegistry.registerPreInit( aPreInit );
}


/// Execute all the preinit functions if not yet executed
void Framework::runPreinits() {
  for( auto fn : mTestRegistry.getAllPreinits() ) {
    fn();
  }
  mPreInitCompleted = true;
}


/// Execute the test-cases corresponding to the filters
bool Framework::runAllTests() {
  TestRunResult runResult;
  ConstTestCaseInfoRefs testCaseInfos;

  ACATCH_INTERNAL_ASSERT( mPreInitCompleted );

  std::vector<ITestCase*> alltests = mTestRegistry.getAllTests(
    TestRegistry::RunOrder::InLexicographicalOrder );
  for( ITestCase* tc : alltests ) {
    if( matchFilter( tc->testInfo().name ) ) {
      testCaseInfos.push_back( &tc->testInfo() );
      runTest( *tc, runResult );
    } else
      mTestReport->reportTestCaseSkip( tc->testInfo() );
  }

  mTestReport->reportTestRun( testCaseInfos, runResult );
  return runResult.getResult();
}


/// Report the testcases only without executing them.
/// The sections are not reported as they cannot be extracted without executing the tests.
void Framework::reportAllTests() {
  ConstTestCaseInfoRefs testInfos;
  std::vector<ITestCase*> alltests = mTestRegistry.getAllTests(
    TestRegistry::RunOrder::InLexicographicalOrder );
  testInfos.reserve( alltests.size() );
  for( ITestCase* tc : alltests ) {
    if( matchFilter( tc->testInfo().name ) )
      testInfos.push_back( &tc->testInfo() );
  }

  mTestReport->reportTestCases( testInfos );
}


void Framework::handleLog( const std::string& aMessage ) {
  if( !aMessage.empty() )
    mCurrentResult->logMessage( TestCaseResult::Info, aMessage );
}


void Framework::handleSuccess() {
  mCurrentResult->logSuccess();
}


void Framework::handleSuccess( const std::string& aMessage ) {
  mCurrentResult->logSuccess();
  if( !aMessage.empty() )
    mCurrentResult->logMessage( TestCaseResult::Info, aMessage );
}


void Framework::handleSuccess( const MultiExpressionCapture& aExpr ) {
  mCurrentResult->logSuccess();
  for( const auto& expr : aExpr.getExpressions() ) {
    mCurrentResult->logMessage( TestCaseResult::Info_ExprRaw, expr.raw );
    mCurrentResult->logMessage( TestCaseResult::Info_ExprExpanded, expr.expanded );
  }
}


void Framework::handleFail( const std::string& aMessage ) {
  mCurrentResult->logFail();
  if( !aMessage.empty() )
    mCurrentResult->logMessage( TestCaseResult::Error, aMessage );
}


void Framework::handleFail( const MultiExpressionCapture& aExpr ) {
  mCurrentResult->logFail();
  for( const auto& expr : aExpr.getExpressions() ) {
    mCurrentResult->logMessage( TestCaseResult::Error_ExprRaw, expr.raw );
    mCurrentResult->logMessage( TestCaseResult::Error_ExprExpanded, expr.expanded );
  }
}


void Framework::handleAbort( const std::string& aMessage ) {
  if( mCurrentResult->isAborting() ) {
    // don't throw exceptions recursively during exit from the test
    return;
  }
  mCurrentResult->logAbort();
  if( !aMessage.empty() )
    mCurrentResult->logMessage( TestCaseResult::Error, aMessage );
  throw TestFailureException();
}


void Framework::handleAbort( const MultiExpressionCapture& aExpr ) {
  if( mCurrentResult->isAborting() ) {
    // don't throw exceptions recursively during exit from the test
    return;
  }
  mCurrentResult->logAbort();
  for( const auto& expr : aExpr.getExpressions() ) {
    mCurrentResult->logMessage( TestCaseResult::Error_ExprRaw, expr.raw );
    mCurrentResult->logMessage( TestCaseResult::Error_ExprExpanded, expr.expanded );
  }
  throw TestFailureException();
}


void Framework::handleFatalErrorCondition( const std::string& aMessage ) {
  mCurrentResult->logAbort();
  handleUnfinishedSections();
  if( !aMessage.empty() )
    mCurrentResult->logMessage( TestCaseResult::Error, aMessage );
  reportNow();
  exit( -1 );
  // from signal handle it is not a good thing to throw exceptions (and not possible on some platforms)
  // and as there is no other way to inform the framework of the failure now it's better to exit (and terminate gracefully)
}


void Framework::reportNow() {
  mTestReport->reportLogNow( *mCurrentResult );
}


void Framework::runTest( ITestCase& aTestCase, TestRunResult& aRunResult ) {
  const TestCaseInfo& testInfo = aTestCase.testInfo();

  TrackerContext trackerContext;
  mTrackerContext = &trackerContext;
  mTestCaseTracker = nullptr;

  bool aborting = false;
  bool failed = false;

  aTestCase.setUp();

  trackerContext.startRun();
  do {
    TestCaseResult testResult;
    mCurrentResult = &testResult;
    trackerContext.startCycle();
    SectionAcquired sectionTracker = SectionTracker::acquire( trackerContext, testInfo.name );
    mTestCaseTracker = sectionTracker.first;
    runTestGuarded( aTestCase );
    aRunResult.add( testResult );
    failed |= testResult.isFailed();
    aborting |= testResult.isAborting();
    mCurrentResult = nullptr;
  } while( !mTestCaseTracker->isSuccessfullyCompleted() && !aborting );

  aTestCase.tearDown();

  mTestCaseTracker = nullptr;
  mTrackerContext = nullptr;
}


bool Framework::isAborting() const {
  ACATCH_INTERNAL_ASSERT( mCurrentResult );
  return mCurrentResult->isAborting();
}


bool Framework::isFailed() const {
  ACATCH_INTERNAL_ASSERT( mCurrentResult );
  return mCurrentResult->isFailed();
}


bool Framework::isRunning() const {
  return !!mCurrentResult;
}


bool Framework::isInAssertTest() const {
  return mInAssertTest;
}


void Framework::runTestGuarded( ITestCase& aActiveTestCase ) {
  mTestReport->reportTestCaseStart( aActiveTestCase.testInfo() );
  try {
    // Timer timer;
    // timer.start();
    FatalConditionHandler fatalConditionHandler; // Handle signals
    aActiveTestCase.invoke();
    fatalConditionHandler.reset();
    // duration = timer.getElapsedSeconds();
  } catch( TestFailureException ) {
    // This just means the test was aborted due to failure
  } catch( ... ) {
    mCurrentResult->logFail();
    // mCurrentResult->logMessage( exception translater );
  }

  mTestCaseTracker->close();
  handleUnfinishedSections();

  mTestReport->reportTestCaseEnd( aActiveTestCase.testInfo(), *mCurrentResult );
}


void Framework::handleUnfinishedSections() {
  // If sections ended prematurely due to an exception we stored their
  // infos here so we can tear them down outside the unwind process.
  for( std::vector<SectionInfo>::const_reverse_iterator
       it = mUnfinishedSections.rbegin(),
       itEnd = mUnfinishedSections.rend();
       it != itEnd; ++it ) {
    sectionEnded( *it );
  }
  mUnfinishedSections.clear();
}


bool Framework::sectionStarted( const SectionInfo& aSectionInfo ) {
  SectionAcquired sectionTracker = SectionTracker::acquire( *mTrackerContext, aSectionInfo.name );

  if( sectionTracker.first->isOpen() && !mPatterns.empty() ) {
    std::string name = sectionTracker.first->getFullName();
    if( !matchFilter( name ) ) {
      mTestReport->reportTestSectionSkip( aSectionInfo );
      sectionTracker.first->skip();
      return false;
    }
  }

  if( !sectionTracker.first->isOpen() )
    return false;

  mActiveSections.push_back( sectionTracker.first );
  mTestReport->reportTestSectionStart( aSectionInfo );
  return true;
}


void Framework::sectionEnded( const SectionInfo& aSectionInfo ) {
  if( !mActiveSections.empty() ) {
    mActiveSections.back()->close();
    mActiveSections.pop_back();
  }

  mTestReport->reportTestSectionEnd( aSectionInfo, *mCurrentResult );
}


void Framework::sectionEndedEarly( const SectionInfo& aSectionInfo ) {
  if( mUnfinishedSections.empty() ) {
    mActiveSections.back()->fail();
  } else
    mActiveSections.back()->close();

  mActiveSections.pop_back();
  mUnfinishedSections.push_back( aSectionInfo );
}


ACATCH_API bool isFailed() {
  return theACatch().isFailed();
}


ACATCH_API bool isAborting() {
  return theACatch().isAborting();
}


ACATCH_API void fatal( const std::string& aMessage ) {
  theACatch().handleFatalErrorCondition( aMessage );
}


} // namespace ACatch
