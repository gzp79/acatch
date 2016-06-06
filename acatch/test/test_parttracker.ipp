/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

// to avoid registration name conflicts due to includes
#line 20000

namespace ACatchTest {

ACATCH_TEST_CASE( "acatch.section_tracking" ) {
  using namespace ACatch;

  TrackerContext ctx;
  ctx.startRun();
  ctx.startCycle();

  SectionAcquired testCase = SectionTracker::acquire( ctx, "Testcase" );
  ACATCH_REQUIRE( EXPECT, testCase.second == true );
  ACATCH_REQUIRE( EXPECT, testCase.first->isOpen() );

  SectionAcquired s1 = SectionTracker::acquire( ctx, "S1" );
  ACATCH_REQUIRE( EXPECT, s1.second == true );
  ACATCH_REQUIRE( EXPECT, s1.first->isOpen() );

  ACATCH_SECTION( "success S1" ) {
    // succeed the first section
    s1.first->close();
    ACATCH_REQUIRE( EXPECT, s1.first->isSuccessfullyCompleted() );
    ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() == false );

    testCase.first->close();
    ACATCH_REQUIRE( EXPECT, ctx.completedCycle() );
    ACATCH_REQUIRE( EXPECT, testCase.first->isSuccessfullyCompleted() );
  }

  ACATCH_SECTION( "fail S1" ) {
    // fail first section
    s1.first->fail();
    ACATCH_REQUIRE( EXPECT, s1.first->isComplete() );
    ACATCH_REQUIRE( EXPECT, s1.first->isSuccessfullyCompleted() == false );
    ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() == false );

    testCase.first->close();
    ACATCH_REQUIRE( EXPECT, ctx.completedCycle() );
    ACATCH_REQUIRE( EXPECT, testCase.first->isSuccessfullyCompleted() == false );

    ACATCH_SECTION( "check" ) {
      ctx.startCycle();
      SectionAcquired testCase2 = SectionTracker::acquire( ctx, "Testcase" );
      ACATCH_REQUIRE( EXPECT, testCase2.second == false );
      ACATCH_REQUIRE( EXPECT, testCase2.first->isOpen() );

      SectionAcquired s1b = SectionTracker::acquire( ctx, "S1" );
      ACATCH_REQUIRE( EXPECT, s1b.second == false );
      ACATCH_REQUIRE( EXPECT, s1b.first->isOpen() == false );

      testCase2.first->close();
      ACATCH_REQUIRE( EXPECT, ctx.completedCycle() );
      ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() );
      ACATCH_REQUIRE( EXPECT, testCase.first->isSuccessfullyCompleted() );
    }

    ACATCH_SECTION( "exec S2" ) {
      ctx.startCycle();
      SectionAcquired testCase2 = SectionTracker::acquire( ctx, "Testcase" );
      ACATCH_REQUIRE( EXPECT, testCase2.second == false );
      ACATCH_REQUIRE( EXPECT, testCase2.first->isOpen() );

      SectionAcquired s1b = SectionTracker::acquire( ctx, "S1" );
      ACATCH_REQUIRE( EXPECT, s1b.second == false );
      ACATCH_REQUIRE( EXPECT, s1b.first->isOpen() == false );

      SectionAcquired s2 = SectionTracker::acquire( ctx, "S2" );
      ACATCH_REQUIRE( EXPECT, s2.second == true );
      ACATCH_REQUIRE( EXPECT, s2.first->isOpen() );

      s2.first->close();
      ACATCH_REQUIRE( EXPECT, ctx.completedCycle() );

      testCase2.first->close();
      ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() );
      ACATCH_REQUIRE( EXPECT, testCase.first->isSuccessfullyCompleted() );
    }
  }

  ACATCH_SECTION( "success S1" ) {
    s1.first->close();

    SectionAcquired s2 = SectionTracker::acquire( ctx, "S2" );
    ACATCH_REQUIRE( EXPECT, s2.second == false ); // cycle has completed, don't care for the new sections
    ACATCH_REQUIRE( EXPECT, s2.first->isOpen() == false );

    testCase.first->close();
    ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() == false );

    ACATCH_SECTION( "start S1/S2" ) {
      ctx.startCycle();
      SectionAcquired testCase2 = SectionTracker::acquire( ctx, "Testcase" );
      ACATCH_REQUIRE( EXPECT, testCase2.second == false );
      ACATCH_REQUIRE( EXPECT, testCase2.first->isOpen() );

      SectionAcquired s1b = SectionTracker::acquire( ctx, "S1" );
      ACATCH_REQUIRE( EXPECT, s1b.second == false );
      ACATCH_REQUIRE( EXPECT, s1b.first->isOpen() == false );

      SectionAcquired s2b = SectionTracker::acquire( ctx, "S2" );
      ACATCH_REQUIRE( EXPECT,
                      s2b.second == true ); // section execution starts here
      ACATCH_REQUIRE( EXPECT, s2b.first->isOpen() );

      ACATCH_REQUIRE( EXPECT, ctx.completedCycle() == false );

      ACATCH_SECTION( "success S1/S2" ) {
        s2b.first->close();
        ACATCH_REQUIRE( EXPECT, ctx.completedCycle() );

        ACATCH_REQUIRE( EXPECT, s2b.first->isSuccessfullyCompleted() );
        ACATCH_REQUIRE( EXPECT, testCase2.first->isComplete() == false );

        testCase2.first->close();
        ACATCH_REQUIRE( EXPECT, testCase2.first->isSuccessfullyCompleted() );
      }
      ACATCH_SECTION( "fail S1/S2" ) {
        s2b.first->fail();
        ACATCH_REQUIRE( EXPECT, ctx.completedCycle() );

        ACATCH_REQUIRE( EXPECT, s2b.first->isComplete() );
        ACATCH_REQUIRE( EXPECT, s2b.first->isSuccessfullyCompleted() == false );

        testCase2.first->close();
        ACATCH_REQUIRE( EXPECT,
                        testCase2.first->isSuccessfullyCompleted() == false );

        // Need a final cycle to complete
        ctx.startCycle();
        SectionAcquired testCase3 = SectionTracker::acquire( ctx, "Testcase" );
        ACATCH_REQUIRE( EXPECT, testCase3.second == false );
        ACATCH_REQUIRE( EXPECT, testCase3.first->isOpen() );

        SectionAcquired s1c = SectionTracker::acquire( ctx, "S1" );
        ACATCH_REQUIRE( EXPECT, s1c.second == false );
        ACATCH_REQUIRE( EXPECT, s1c.first->isOpen() == false );

        SectionAcquired s2c = SectionTracker::acquire( ctx, "S2" );
        ACATCH_REQUIRE( EXPECT, s2c.second == false );
        ACATCH_REQUIRE( EXPECT, s2c.first->isOpen() == false );

        testCase3.first->close();
        ACATCH_REQUIRE( EXPECT, testCase3.first->isSuccessfullyCompleted() );
      }
    }
  }

  ACATCH_SECTION( "exec S1/S2" ) {
    SectionAcquired s2 = SectionTracker::acquire( ctx, "S2" );
    ACATCH_REQUIRE( EXPECT, s2.second == true );
    ACATCH_REQUIRE( EXPECT, s2.first->isOpen() );

    s2.first->close();
    ACATCH_REQUIRE( EXPECT, s2.first->isComplete() );
    ACATCH_REQUIRE( EXPECT, s1.first->isComplete() == false );

    s1.first->close();
    ACATCH_REQUIRE( EXPECT, s1.first->isComplete() );
    ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() == false );

    testCase.first->close();
    ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() );
  }

  ACATCH_SECTION( "start S1/G1" ) {
    IndexAcquired g1 = IndexTracker::acquire( ctx, "G1", 2 );
    ACATCH_REQUIRE( EXPECT, g1.second == true );
    ACATCH_REQUIRE( EXPECT, g1.first->isOpen() );
    ACATCH_REQUIRE( EXPECT, g1.first->index() == 0 );

    ACATCH_REQUIRE( EXPECT, g1.first->isComplete() == false );
    ACATCH_REQUIRE( EXPECT, s1.first->isComplete() == false );

    ACATCH_SECTION( "success S1/G1" ) {
      s1.first->close();
      ACATCH_REQUIRE( EXPECT, s1.first->isComplete() == false );
      testCase.first->close();
      ACATCH_REQUIRE( EXPECT, testCase.first->isSuccessfullyCompleted() == false );

      ACATCH_SECTION( "exec S1/G1" ) {
        ctx.startCycle();
        SectionAcquired testCase2 = SectionTracker::acquire( ctx, "Testcase" );
        ACATCH_REQUIRE( EXPECT, testCase2.second == false );
        ACATCH_REQUIRE( EXPECT, testCase2.first->isOpen() );

        SectionAcquired s1b = SectionTracker::acquire( ctx, "S1" );
        ACATCH_REQUIRE( EXPECT, s1b.second == false );
        ACATCH_REQUIRE( EXPECT, s1b.first->isOpen() );

        IndexAcquired g1b = IndexTracker::acquire( ctx, "G1", 2 );
        ACATCH_REQUIRE( EXPECT, g1b.second == true );
        ACATCH_REQUIRE( EXPECT, g1b.first->isOpen() );
        ACATCH_REQUIRE( EXPECT, g1b.first->index() == 1 );

        ACATCH_REQUIRE( EXPECT, s1.first->isComplete() == false );

        s1b.first->close();
        ACATCH_REQUIRE( EXPECT, s1b.first->isComplete() );
        ACATCH_REQUIRE( EXPECT, g1b.first->isComplete() );
        testCase2.first->close();
        ACATCH_REQUIRE( EXPECT, testCase2.first->isComplete() );
      }
    }

    ACATCH_SECTION( "exec S1/G1/S2" ) {
      SectionAcquired s2 = SectionTracker::acquire( ctx, "S2" );
      ACATCH_REQUIRE( EXPECT, s2.second == true );
      ACATCH_REQUIRE( EXPECT, s2.first->isOpen() );

      s2.first->close();
      ACATCH_REQUIRE( EXPECT, s2.first->isComplete() );

      s1.first->close();
      ACATCH_REQUIRE( EXPECT, s1.first->isComplete() == false );

      testCase.first->close();
      ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() == false );

      // execute again, generator create new index and new section (scope) for
      // S2
      ctx.startCycle();
      SectionAcquired testCase2 = SectionTracker::acquire( ctx, "Testcase" );
      ACATCH_REQUIRE( EXPECT, testCase2.second == false );
      ACATCH_REQUIRE( EXPECT, testCase2.first->isOpen() );

      SectionAcquired s1b = SectionTracker::acquire( ctx, "S1" );
      ACATCH_REQUIRE( EXPECT, s1b.second == false );
      ACATCH_REQUIRE( EXPECT, s1b.first->isOpen() );

      // generator - next value
      IndexAcquired g1b = IndexTracker::acquire( ctx, "G1", 2 );
      ACATCH_REQUIRE( EXPECT, g1b.second == true );
      ACATCH_REQUIRE( EXPECT, g1b.first->isOpen() );
      ACATCH_REQUIRE( EXPECT, g1b.first->index() == 1 );

      // inner section again
      SectionAcquired s2b = SectionTracker::acquire( ctx, "S2" );
      ACATCH_REQUIRE( EXPECT, s2b.second == true );
      ACATCH_REQUIRE( EXPECT, s2b.first->isOpen() );

      s2b.first->close();
      ACATCH_REQUIRE( EXPECT, s2b.first->isComplete() );

      s1b.first->close();
      ACATCH_REQUIRE( EXPECT, g1b.first->isComplete() );
      ACATCH_REQUIRE( EXPECT, s1b.first->isComplete() );

      testCase2.first->close();
      ACATCH_REQUIRE( EXPECT, testCase2.first->isComplete() );
    }

    ACATCH_SECTION( "exec(fail) S1/G1/S2" ) {
      // 1. Executing S2 for the first time will fail
      SectionAcquired s2 = SectionTracker::acquire( ctx, "S2" );
      ACATCH_REQUIRE( EXPECT, s2.second == true );
      ACATCH_REQUIRE( EXPECT, s2.first->isOpen() );

      s2.first->fail();
      ACATCH_REQUIRE( EXPECT, s2.first->isComplete() );
      ACATCH_REQUIRE( EXPECT, s2.first->isSuccessfullyCompleted() == false );

      s1.first->close();
      ACATCH_REQUIRE( EXPECT, s1.first->isComplete() == false );

      testCase.first->close();
      ACATCH_REQUIRE( EXPECT, testCase.first->isComplete() == false );

      // 2. Executing S2 after fail won't open new test section
      ctx.startCycle();
      SectionAcquired testCase2 = SectionTracker::acquire( ctx, "Testcase" );
      ACATCH_REQUIRE( EXPECT, testCase2.second == false );
      ACATCH_REQUIRE( EXPECT, testCase2.first->isOpen() );

      SectionAcquired s1b = SectionTracker::acquire( ctx, "S1" );
      ACATCH_REQUIRE( EXPECT, s1b.second == false );
      ACATCH_REQUIRE( EXPECT, s1b.first->isOpen() );

      // generator - still same value - S2 failed, G1 is not incremented
      IndexAcquired g1b = IndexTracker::acquire( ctx, "G1", 2 );
      ACATCH_REQUIRE( EXPECT, g1b.second == false );
      ACATCH_REQUIRE( EXPECT, g1b.first->isOpen() );
      ACATCH_REQUIRE( EXPECT, g1b.first->index() == 0 );

      // inner section again - this time won't open, as it failed before
      SectionAcquired s2b = SectionTracker::acquire( ctx, "S2" );
      ACATCH_REQUIRE( EXPECT, s2b.second == false );
      ACATCH_REQUIRE( EXPECT, s2b.first->isOpen() == false );

      s1b.first->close();
      ACATCH_REQUIRE( EXPECT, g1b.first->isComplete() == false );
      ACATCH_REQUIRE( EXPECT, s1b.first->isComplete() == false );

      testCase2.first->close();
      ACATCH_REQUIRE( EXPECT, testCase2.first->isComplete() == false );

      // 3. Another cycle moves to the next section and should complete
      ctx.startCycle();
      SectionAcquired testCase3 = SectionTracker::acquire( ctx, "Testcase" );
      ACATCH_REQUIRE( EXPECT, testCase3.second == false );
      ACATCH_REQUIRE( EXPECT, testCase3.first->isOpen() );

      SectionAcquired s1c = SectionTracker::acquire( ctx, "S1" );
      ACATCH_REQUIRE( EXPECT, s1c.second == false );
      ACATCH_REQUIRE( EXPECT, s1c.first->isOpen() );

      // generator - now next value
      IndexAcquired g1c = IndexTracker::acquire( ctx, "G1", 2 );
      ACATCH_REQUIRE( EXPECT, g1c.second == true );
      ACATCH_REQUIRE( EXPECT, g1c.first->isOpen() );
      ACATCH_REQUIRE( EXPECT, g1c.first->index() == 1 );

      // inner section - now should open again
      SectionAcquired s2c = SectionTracker::acquire( ctx, "S2" );
      ACATCH_REQUIRE( EXPECT, s2c.second == true );
      ACATCH_REQUIRE( EXPECT, s2c.first->isOpen() );

      s2c.first->close();
      ACATCH_REQUIRE( EXPECT, s2c.first->isComplete() );

      s1c.first->close();
      ACATCH_REQUIRE( EXPECT, g1c.first->isComplete() );
      ACATCH_REQUIRE( EXPECT, s1c.first->isComplete() );

      testCase3.first->close();
      ACATCH_REQUIRE( EXPECT, testCase3.first->isComplete() );
    }

    // !TBD"
    //   nested generator
    //   two sections within a generator
  }
}

} // namespace ACatchTest

