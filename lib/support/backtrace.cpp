/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/backtrace.h"

#ifdef HAVE_BACKWARD
#include "Backward/backward.hpp"

void srsran::enable_backtrace()
{
  static backward::SignalHandling sh;
}
#else  // HAVE_BACKWARD
void srsran::enable_backtrace()
{
  // Ignore.
}
#endif // HAVE_BACKWARD