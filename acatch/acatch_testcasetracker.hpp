/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

//-----------------------------------------------------------------------------
/// Base interface for the tracking node
struct ITracker {
  ITracker() = default;
  virtual ~ITracker() {
  }

  ITracker( const ITracker& ) = delete;
  ITracker( ITracker&& ) = delete;
  ITracker& operator=( const ITracker& ) = delete;

  // static queries
  virtual std::string name() const = 0;

  // dynamic queries
  virtual bool isComplete() const = 0; // Successfully completed or failed
  virtual bool isSuccessfullyCompleted() const = 0;
  virtual bool isOpen() const = 0; // Started but not complete
  virtual bool hasChildren() const = 0;
  virtual std::string getFullName() const = 0;

  virtual ITracker& parent() = 0;
  virtual ITracker* pparent() const = 0;

  // actions
  virtual void close() = 0; // Successfully complete
  virtual void skip() = 0;
  virtual void fail() = 0;
  virtual void markAsNeedingAnotherRun() = 0;

  virtual void addChild( std::shared_ptr<ITracker> const& child ) = 0;
  virtual ITracker* findChild( std::string const& name ) = 0;
  virtual void openChild() = 0;

  // debug helper
  virtual void showTree( ITracker* aCurrentTracker, int aDepth ) = 0;
};

//-----------------------------------------------------------------------------
/// Store the tracking tree
class TrackerContext {

  enum RunState { NotStarted, Executing, CompletedCycle };

  std::shared_ptr<ITracker> mRootTracker;
  ITracker* mCurrentTracker;
  RunState mRunState;

public:
  TrackerContext()
      : mCurrentTracker( nullptr )
      , mRunState( NotStarted ) {
  }

  ITracker& startRun();

  void endRun() {
    mRootTracker.reset();
    mCurrentTracker = nullptr;
    mRunState = NotStarted;
  }

  void startCycle() {
    mCurrentTracker = mRootTracker.get();
    mRunState = Executing;
  }

  void completeCycle() {
    mRunState = CompletedCycle;
  }

  bool completedCycle() const {
    return mRunState == CompletedCycle;
  }

  ITracker& currentTracker() {
    return *mCurrentTracker;
  }

  void setCurrentTracker( ITracker* tracker ) {
    mCurrentTracker = tracker;
  }

  void showTree( const char* aMsg ) {
    std::cout << "\n----------------------\n";
    std::cout << aMsg << "\n";
    mRootTracker->showTree( mCurrentTracker, 0 );
    std::cout << "----------------------\n\n";
  }
};

//-----------------------------------------------------------------------------

class TrackerBase : public ITracker {
public:
  TrackerBase( const std::string& name, TrackerContext& ctx, ITracker* parent )
      : mName( name )
      , mCtx( ctx )
      , mParent( parent )
      , mCycleState( NotStarted ) {
  }

  virtual std::string name() const override {
    return mName;
  }

  virtual bool isComplete() const override {
    return mCycleState == CompletedSuccessfully || mCycleState == Failed
           || mCycleState == Skipped;
  }

  //  virtual bool isNew() const override {
  //return !isComplete() && mCycleState != NeedsAnotherRun;
  //}

  virtual bool isSuccessfullyCompleted() const override {
    return mCycleState == CompletedSuccessfully;
  }

  virtual bool isOpen() const override {
    return mCycleState != NotStarted && !isComplete();
  }

  virtual bool hasChildren() const override {
    return !mChildren.empty();
  }

  virtual void addChild( std::shared_ptr<ITracker> const& child ) override {
    mChildren.push_back( child );
  }

  virtual ITracker* findChild( std::string const& name ) override {
    Children::const_iterator it = std::find_if(
        mChildren.begin(), mChildren.end(), TrackerHasName( name ) );
    return ( it != mChildren.end() ) ? it->get() : nullptr;
  }

  virtual std::string getFullName() const override {
    const ITracker* p = this;
    std::string res;
    while( p->pparent() ) {
      if( res.empty() )
        res = p->name();
      else
        res = p->name() + "." + res;

      p = p->pparent();
    }
    return res;
  }

  virtual ITracker* pparent() const override {
    return mParent;
  }

  virtual ITracker& parent() override {
    assert( mParent ); // Should always be non-null except for root
    return *mParent;
  }

  virtual void openChild() override {
    if( mCycleState != ExecutingChildren ) {
      mCycleState = ExecutingChildren;
      if( mParent )
        mParent->openChild();
    }
  }

  void open() {
    mCycleState = Executing;
    moveToThis();
    if( mParent )
      mParent->openChild();
  }

  virtual void close() override {
    // Close any still open children (e.g. generators)
    while( &mCtx.currentTracker() != this )
      mCtx.currentTracker().close();

    switch( mCycleState ) {
    case NotStarted:
    case CompletedSuccessfully:
    case Failed:
    case Skipped:
      throw std::logic_error( "Illogical state" );

    case NeedsAnotherRun:
      break;

    case Executing:
      mCycleState = CompletedSuccessfully;
      break;

    case ExecutingChildren:
      if( mChildren.empty() || mChildren.back()->isComplete() )
        mCycleState = CompletedSuccessfully;
      break;

    default:
      throw std::logic_error( "Unexpected state" );
    }
    moveToParent();
    mCtx.completeCycle();
  }

  virtual void skip() override {
    // Close any still open children (e.g. generators)
    while( &mCtx.currentTracker() != this )
      mCtx.currentTracker().close();

    switch( mCycleState ) {
    case NotStarted:
    case CompletedSuccessfully:
    case Failed:
    case Skipped:
    case ExecutingChildren:
      throw std::logic_error( "Illogical state" );

    case NeedsAnotherRun:
      break;

    case Executing:
      mCycleState = Skipped;
      break;

    default:
      throw std::logic_error( "Unexpected state" );
    }
    moveToParent();
  }

  virtual void fail() override {
    mCycleState = Failed;
    if( mParent )
      mParent->markAsNeedingAnotherRun();
    moveToParent();
    mCtx.completeCycle();
  }

  virtual void markAsNeedingAnotherRun() override {
    mCycleState = NeedsAnotherRun;
  }

  virtual void showTree( ITracker* aCurrentTracker, int aDepth ) override {
    std::cout << ( aCurrentTracker == this ? "!" : " " );
    for( int i = 0; i < aDepth; ++i )
      std::cout << " ";

    std::cout << name() << " (";
    switch( mCycleState ) {
    case NotStarted:
      std::cout << "NotStarted)\n";
      break;
    case Executing:
      std::cout << "ExecutingChildren)\n";
      break;
    case ExecutingChildren:
      std::cout << "ExecutingChildren)\n";
      break;
    case NeedsAnotherRun:
      std::cout << "NeedsAnotherRun)\n";
      break;
    case CompletedSuccessfully:
      std::cout << "CompletedSuccessfully)\n";
      break;
    case Failed:
      std::cout << "Failed)\n";
      break;
    case Skipped:
      std::cout << "Skipped)\n";
      break;
    }

    for( const auto& ch : mChildren ) {
      ch->showTree( aCurrentTracker, aDepth + 1 );
    }
  }

protected:
  enum CycleState {
    NotStarted,
    Executing,
    ExecutingChildren,
    NeedsAnotherRun,
    CompletedSuccessfully,
    Failed,
    Skipped
  };

  class TrackerHasName {
    std::string mName;

  public:
    TrackerHasName( std::string const& name )
        : mName( name ) {
    }

    bool operator()( std::shared_ptr<ITracker> const& tracker ) {
      return tracker->name() == mName;
    }
  };

  typedef std::vector<std::shared_ptr<ITracker>> Children;
  std::string mName;
  TrackerContext& mCtx;
  ITracker* mParent;
  Children mChildren;
  CycleState mCycleState;

private:
  void moveToParent() {
    assert( mParent );
    mCtx.setCurrentTracker( mParent );
  }

  void moveToThis() {
    mCtx.setCurrentTracker( this );
  }
};

class SectionTracker;
typedef std::pair<SectionTracker*, bool>
    SectionAcquired; // second param indicates if it was just opened

//-----------------------------------------------------------------------------
/// Track sections
class SectionTracker : public TrackerBase {
public:
  SectionTracker( std::string const& name, TrackerContext& ctx,
                  ITracker* parent )
      : TrackerBase( name, ctx, parent ) {
  }

  static SectionAcquired acquire( TrackerContext& ctx,
                                  std::string const& name ) {
    SectionAcquired res;
    res.first = nullptr;
    res.second = false;

    ITracker& currentTracker = ctx.currentTracker();
    if( ITracker* childTracker = currentTracker.findChild( name ) ) {
      res.first = dynamic_cast<SectionTracker*>( childTracker );
      assert( res.first );
    } else {
      res.first = new SectionTracker( name, ctx, &currentTracker );
      currentTracker.addChild( std::shared_ptr<ITracker>( res.first ) );
    }

    if( !ctx.completedCycle() && !res.first->isComplete() ) {
      if( res.first->mCycleState != ExecutingChildren
          && res.first->mCycleState != NeedsAnotherRun ) {
        res.second = true;
      }
      res.first->open();
    }

    return res;
  }
};

class IndexTracker;
typedef std::pair<IndexTracker*, bool>
    IndexAcquired; // second param indicates if it was just opened

//-----------------------------------------------------------------------------
class IndexTracker : public TrackerBase {
public:
  IndexTracker( std::string const& name, TrackerContext& ctx, ITracker* parent,
                int size )
      : TrackerBase( name, ctx, parent )
      , mSize( size )
      , mIndex( -1 ) {
  }

  static IndexAcquired acquire( TrackerContext& ctx, std::string const& name,
                                int size ) {
    IndexAcquired res;
    res.first = nullptr;
    res.second = false;

    ITracker& currentTracker = ctx.currentTracker();
    if( ITracker* childTracker = currentTracker.findChild( name ) ) {
      res.first = dynamic_cast<IndexTracker*>( childTracker );
      assert( res.first );
    } else {
      res.first = new IndexTracker( name, ctx, &currentTracker, size );
      currentTracker.addChild( std::shared_ptr<ITracker>( res.first ) );
    }

    if( !ctx.completedCycle() && !res.first->isComplete() ) {
      if( res.first->mCycleState != ExecutingChildren
          && res.first->mCycleState != NeedsAnotherRun ) {
        res.first->moveNext();
        res.second = true;
      }
      res.first->open();
    }

    return res;
  }

  int index() const {
    return mIndex;
  }

  void moveNext() {
    mIndex++;
    mChildren.clear();
  }

  virtual void close() override {
    TrackerBase::close();
    if( mCycleState == CompletedSuccessfully && mIndex < mSize - 1 )
      mCycleState = Executing;
  }

protected:
  int mSize;
  int mIndex;
};

inline ITracker& TrackerContext::startRun() {
  mRootTracker = std::shared_ptr<ITracker>(
      new SectionTracker( "{root}", *this, nullptr ) );
  mCurrentTracker = nullptr;
  mRunState = Executing;
  return *mRootTracker;
}

} // namespace Catch
