/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

namespace ACatch {

/// Test section information.
struct ACATCH_API SectionInfo {
  SectionInfo( const std::string& aName )
      : name( aName ) {
    ACATCH_INTERNAL_ASSERT( !name.empty() );
  }

  std::string name;
};

/// Test section. Manage the section enter/leave cycle in the test.
class ACATCH_API Section {
public:
  Section( const SectionInfo& aInfo );
  ~Section();

  Section( const Section& ) = delete;
  Section( const Section&& ) = delete;
  Section& operator=( const Section& ) = delete;

  // This indicates whether the section should be executed or not
  explicit operator bool() const {
    return mSectionIncluded;
  }

private:
  SectionInfo mInfo;
  bool mSectionIncluded;
};

} // namespace ACatch
