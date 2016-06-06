/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch/acatch_core.hpp"

#include <csignal>

namespace ACatch {

namespace {

struct SignalDefs {
  int id;
  const char* name;
};

SignalDefs signalDefs[] = {
  { SIGINT, "SIGINT - Terminal interrupt signal" },
  { SIGILL, "SIGILL - Illegal instruction signal" },
  { SIGFPE, "SIGFPE - Floating point error signal" },
  { SIGSEGV, "SIGSEGV - Segmentation violation signal" },
  { SIGTERM, "SIGTERM - Termination request signal" },
  { SIGABRT, "SIGABRT - Abort (abnormal termination) signal" }
};

} // namespace

void FatalConditionHandler::handleSignal( int aSig ) {
  for( std::size_t i = 0; i < sizeof( signalDefs ) / sizeof( SignalDefs );
       ++i ) {
    if( aSig == signalDefs[ i ].id ) {
      fatal( signalDefs[ i ].name );
    }
  }
  fatal( "<unknown signal>" );
}

FatalConditionHandler::FatalConditionHandler()
    : mIsSet( true ) {
  for( std::size_t i = 0; i < sizeof( signalDefs ) / sizeof( SignalDefs );
       ++i ) {
    signal( signalDefs[ i ].id, handleSignal );
  }
}

FatalConditionHandler::~FatalConditionHandler() {
  reset();
}

void FatalConditionHandler::reset() {
  if( mIsSet ) {
    for( std::size_t i = 0; i < sizeof( signalDefs ) / sizeof( SignalDefs );
         ++i ) {
      signal( signalDefs[ i ].id, SIG_DFL );
    }
    mIsSet = false;
  }
}

} // namespace ACatch

