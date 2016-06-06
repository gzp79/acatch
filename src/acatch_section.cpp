/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

namespace ACatch {

Section::Section( const SectionInfo& aInfo )
    : mInfo( aInfo )
    , mSectionIncluded( theACatch().sectionStarted( mInfo ) ) {
  // m_timer.start();
}

Section::~Section() {
  if( mSectionIncluded ) {
    if( std::uncaught_exception() )
      theACatch().sectionEndedEarly( mInfo );
    else
      theACatch().sectionEnded( mInfo );
  }
}

} // namespace ACatch
