/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

namespace ACatch {

class TestAssertGuard;

/// The test framework. Singleton that manage all the test cases.
class ACATCH_API Framework
{
public:
  Framework();
  ~Framework();

  Framework( const Framework& ) = delete;
  Framework( const Framework&& ) = delete;
  Framework& operator=( const Framework& ) = delete;

  void addFilter( const std::string& aPattern );
  bool matchFilter( const std::string& aName );

  void setBreak( EBreak aBreak );

  void registerTestCase( ITestCase* aTestCase );
  void registerPreInit( FnPreInit aPreInit );
  void runPreinits();
  bool runAllTests();
  void reportAllTests();

  bool isAborting() const;
  bool isFailed() const;
  bool isRunning() const;
  bool isInAssertTest() const;

  void handleLog( const std::string& aMessage );
  void handleSuccess();
  void handleSuccess( const std::string& aMessage );
  void handleSuccess( const MultiExpressionCapture& aExpr );
  void handleFail( const std::string& aMessage );
  void handleFail( const MultiExpressionCapture& aExpr );
  void handleAbort( const std::string& aMessage );
  void handleAbort( const MultiExpressionCapture& aExpr );
  void handleFatalErrorCondition( const std::string& aMessage );

  void reportNow();

  ITestReport* getTestReport() const {
    return mTestReport;
  }

private:
  EBreak mBreakOnError;
  std::vector<std::string> mPatterns;
  TestRegistry mTestRegistry;
  ITestReport* mTestReport;

  bool mPreInitCompleted;
  bool mInAssertTest;
  TrackerContext* mTrackerContext;
  ITracker* mTestCaseTracker;
  TestCaseResult* mCurrentResult;
  std::vector<SectionInfo> mUnfinishedSections;
  std::vector<ITracker*> mActiveSections;

  void runTest( ITestCase& aTestCase, TestRunResult& aRunResult );
  void runTestGuarded( ITestCase& aTestCase );
  void handleUnfinishedSections();
  bool sectionStarted( const SectionInfo& aSectionInfo );
  void sectionEnded( const SectionInfo& aSectionInfo );
  void sectionEndedEarly( const SectionInfo& aSectionInfo );

private:
  static Framework* sInstance;

  friend Framework& theACatch();
  friend void theACatchShutdown();
  friend class Section;
  friend class TestAssertGuard;
};

} // namespace ACatch
