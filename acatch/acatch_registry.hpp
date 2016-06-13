/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

//-----------------------------------------------------------------------------
/// Test section information.
struct ACATCH_API TestCaseInfo
{
  TestCaseInfo( const char* aName )
      : name( aName ) {
  }

  std::string name;
};

typedef std::vector<const TestCaseInfo*> ConstTestCaseInfoRefs;

//-----------------------------------------------------------------------------
/// Interface for the test cases
class ACATCH_API ITestCase
{
public:
  ITestCase( const TestCaseInfo& aInfo )
      : mInfo( aInfo ) {
  }

  virtual ~ITestCase() {
  }

  const TestCaseInfo& testInfo() const {
    return mInfo;
  }

  virtual void setUp() = 0;
  virtual void tearDown() = 0;
  virtual void invoke() = 0;

protected:
  TestCaseInfo mInfo;
};
typedef std::unique_ptr<ITestCase> ATestCase;

typedef void(*FnPreInit)();

/// Test a class through the given function.
/// During each invocation a new instance of the class is created.
template <typename TClass>
class MethodTestCase
    : public ITestCase
{
public:
  MethodTestCase( void( TClass::*aMethod )(), const TestCaseInfo& aInfo )
      : ITestCase( aInfo )
      , mMethod( aMethod ) {
  }

  virtual void setUp() {
  }

  virtual void invoke() {
    TClass obj;
    ( obj.*mMethod )();
  }

  virtual void tearDown() {
  }

private:
  void ( TClass::*mMethod )();
};


/// Fixture based test case.
/// Only a single object is created, and the method is called for each test section.
template <typename TClass>
class FixtureTestCase
    : public ITestCase
{
public:
  FixtureTestCase( void( TClass::*aMethod )(), const TestCaseInfo &aInfo )
      : ITestCase( aInfo )
      , mMethod( aMethod )
      , mObj( nullptr ) {
  }

  virtual void setUp() {
    mObj = new TClass;
  }

  virtual void invoke() {
    ( mObj->*mMethod )();
  }

  virtual void tearDown() {
    delete mObj;
  }

private:
  TClass* mObj;
  void (TClass::*mMethod)();
};


/// Function based test case.
class ACATCH_API FunctionTestCase
    : public ITestCase
{
public:
  typedef void ( *Function )();

  FunctionTestCase( Function aFun, const TestCaseInfo& aInfo )
      : ITestCase( aInfo )
      , mFunction( aFun ) {
  }

  virtual void setUp() {
  }

  virtual void invoke() {
    mFunction();
  }

  virtual void tearDown() {
  }

private:
  Function mFunction;
};

//-----------------------------------------------------------------------------
/// Manage the registered test cases.
class ACATCH_API TestRegistry
{
public:
  enum class RunOrder {
    InLexicographicalOrder,
    InRandomOrder,
    InDeclarationOrder
  };

  TestRegistry() {
    mFunctions.reserve( 1024 );
  }

  virtual ~TestRegistry() {
  }

  TestRegistry( const TestRegistry& ) = delete;
  TestRegistry( const TestRegistry&& ) = delete;
  TestRegistry& operator=( const TestRegistry& ) = delete;

  void registerTest( ITestCase* aTestCase ) {
    mFunctions.emplace_back( aTestCase );
  }

  void registerPreInit( FnPreInit aPreInit ) {
	  mPreInit.emplace_back( aPreInit );
  }

  const std::vector<FnPreInit>& getAllPreinits() const { return mPreInit; }
  std::vector<ITestCase*> getAllTests( RunOrder aOrder ) const;

private:
  std::vector<ATestCase> mFunctions;
  std::vector<FnPreInit> mPreInit;
};

//-----------------------------------------------------------------------------
/// Helper to register test cases.
struct ACATCH_API AutoReg
{
  template <AutoReg&>
  struct ForceReference {};

  static ITestCase* mkFunctionTest( FunctionTestCase::Function aFunction, const TestCaseInfo& aInfo ) {
    return new FunctionTestCase( aFunction, aInfo );
  }

  template <typename TClass>
  static ITestCase* mkFixtureTest( void ( TClass::*aMethod )(), const TestCaseInfo& aInfo ) {
    return new FixtureTestCase<TClass>( aMethod, aInfo );
  }

  template <typename TClass>
  static ITestCase* mkMethodTest( void ( TClass::*aMethod )(), const TestCaseInfo& aInfo ) {
    return new MethodTestCase<TClass>( aMethod, aInfo );
  }

  AutoReg( ITestCase* aTestCase ) {
    if( aTestCase )
      registerTestCase( aTestCase );
  }

  AutoReg( FnPreInit aPreInit ) {
    registerPreInit( aPreInit );
  }

  AutoReg( const AutoReg& ) = delete;
  AutoReg( const AutoReg&& ) = delete;
  AutoReg& operator=( const AutoReg& ) = delete;

private:
  void registerTestCase( ITestCase* aTestCase );
  void registerPreInit( FnPreInit aPreInit );
};

} // namespace ACatch
