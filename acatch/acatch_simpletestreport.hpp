/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

//-----------------------------------------------------------------------------
/// Internal class to track a tree of sections
class ACATCH_API SimpleTestReport
    : public ITestReport
{
public:
  SimpleTestReport();

  virtual void setProperty( const std::string& aProp, const std::string& aValue ) override;

  virtual void reportTestCases( const ConstTestCaseInfoRefs& aInfos ) override;

  virtual void reportTestCaseSkip( const TestCaseInfo& aInfo ) override;
  virtual void reportTestCaseStart( const TestCaseInfo& aInfo ) override;
  virtual void reportTestSectionStart( const SectionInfo& aInfo ) override;
  virtual void reportTestSectionSkip( const SectionInfo& aInfo ) override;
  virtual void reportTestSectionEnd( const SectionInfo& aInfo, TestCaseResult& aResult ) override;
  virtual void reportTestCaseEnd( const TestCaseInfo& aInfo, TestCaseResult& aResult ) override;
  virtual void reportLogNow( TestCaseResult& aResult ) override;

  virtual void reportTestRun( const ConstTestCaseInfoRefs& aInfos, TestRunResult& aRunResult ) override;


protected:
  enum class State {
    Runing,
    Failed,
    Success,
    Aborted,
    Skip,

    List,
  };

  std::vector<std::string> mNames;
  bool mVerbose;
  size_t mDepth;

  void printTestResult( TestCaseResult& aResult, bool aCompleted );
  void printTestName( State aState );
};

} // namespace ACatch
