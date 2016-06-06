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
class ACATCH_API ITestReport
{
public:
  virtual ~ITestReport() {
  }

  virtual void setProperty( const std::string& aProp, const std::string& aValue ) = 0;

  virtual void reportTestCases( const ConstTestCaseInfoRefs& aInfos ) = 0;

  virtual void reportTestCaseSkip( const TestCaseInfo& aInfo ) = 0;
  virtual void reportTestCaseStart( const TestCaseInfo& aInfo ) = 0;
  virtual void reportTestSectionStart( const SectionInfo& aInfo ) = 0;
  virtual void reportTestSectionSkip( const SectionInfo& aInfo ) = 0;
  virtual void reportTestSectionEnd( const SectionInfo& aInfo, TestCaseResult& aResult ) = 0;
  virtual void reportTestCaseEnd( const TestCaseInfo& aInfo, TestCaseResult& aResult ) = 0;
  virtual void reportLogNow( TestCaseResult& aResult ) = 0;

  virtual void reportTestRun( const ConstTestCaseInfoRefs& aInfos, TestRunResult& aRunResult ) = 0;
};

} // namespace ACatch
