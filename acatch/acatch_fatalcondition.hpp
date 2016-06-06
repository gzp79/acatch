/*
 *  Based on the work of Phil, Copyright 2010 Two Blue Cubes Ltd. All rights
 * reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#include "acatch_config.hpp"

namespace ACatch {

struct ACATCH_API FatalConditionHandler {
public:
  FatalConditionHandler();
  ~FatalConditionHandler();

  void reset();

protected:
  bool mIsSet;

  static void handleSignal( int aSig );
};

} // namespace ACatch
