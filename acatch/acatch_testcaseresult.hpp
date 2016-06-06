/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

//-----------------------------------------------------------------------------
/// Store the result of the current test case. Thread-safe multiple thread may
/// log/check the result at the same time.
struct TestCaseResult
{
public:
  enum ELog {
    Info,
    Info_ExprRaw,
    Info_ExprExpanded,
    Warning,
    Error,
    Error_ExprRaw,
    Error_ExprExpanded,

  };
  typedef std::pair<ELog, std::string> Log;
  typedef std::vector<Log> Logs;

  TestCaseResult()
      : mHasNew( false )
      , mFails( 0 )
      , mSuccess( 0 ) {
  }

  void logFail() {
    mHasNew.store( true, std::memory_order_relaxed );
    mFails.fetch_add( 2, std::memory_order_relaxed );
  }

  void logAbort() {
    mHasNew.store( true, std::memory_order_relaxed );
    mFails.fetch_or( 0x1, std::memory_order_relaxed );
  }

  void logSuccess() {
    mHasNew.store( true, std::memory_order_relaxed );
    mSuccess.fetch_add( 1, std::memory_order_relaxed );
  }

  bool isFailed() const {
    return mFails.load( std::memory_order_relaxed ) > 0;
  }

  bool isAborting() const {
    return mFails.load( std::memory_order_relaxed ) & 0x1;
  }

  int getFailCount() const {
    return mFails.load( std::memory_order_relaxed ) / 2;
  }

  int getSuccessCount() const {
    return mSuccess.load( std::memory_order_relaxed );
  }

  void logMessage( ELog aLog, const std::string& aMessage ) {
    std::lock_guard<std::mutex> lg( mLogMutex );
    mLogs.emplace_back( aLog, aMessage );
  }

  bool takeLogs( Logs& aLogs ) {
    {
      std::lock_guard<std::mutex> lg( mLogMutex );
      std::swap( mLogs, aLogs );
    }
    return mHasNew.exchange( false, std::memory_order_relaxed ) || !aLogs.empty();
  }

protected:
  std::atomic<bool> mHasNew;  ///< Indicates when state should be reported
  std::atomic<uint> mFails;   ///< number of fails * 2 + isAborted
  std::atomic<uint> mSuccess; ///< number of succeeded test, fast tests are not logged
  std::mutex mLogMutex;       ///< guard log buffer as other thread may add logs while the test-runner tries to print them
  std::vector<Log> mLogs;
};

//-----------------------------------------------------------------------------
class TestRunResult
{
public:
  TestRunResult()
      : mPassedTestCount( 0 )
      , mFailedTestCount( 0 )
      , mPassedAssertionCount( 0 )
      , mFailedAssertionCount( 0 ) {
  }

  void add( const TestCaseResult& aResult ) {
    mPassedAssertionCount += aResult.getSuccessCount();

    if( aResult.isFailed() ) {
      mFailedTestCount++;
      mFailedAssertionCount += aResult.getFailCount();
    } else {
      mPassedTestCount++;
    }
  }

  int getPassedTestCount() const {
    return mPassedTestCount;
  }

  int getFailedTestCount() const {
    return mFailedTestCount;
  }

  int getPassedAssertionCount() const {
    return mPassedAssertionCount;
  }

  int getFailedAssertionCount() const {
    return mFailedAssertionCount;
  }

  bool getResult() const {
    return mFailedTestCount == 0;
  }

protected:
  int mPassedTestCount;     ///< number of passed test cases
  int mFailedTestCount;     ///< number of failed test cases
  int mPassedAssertionCount;///< number of successful assertions (excluding the FAST checks)
  int mFailedAssertionCount;///< number of failed assertions (including aborts)
};

} // namespace ACatch
