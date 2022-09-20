/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/ngap/ngc_factory.h"
#include "ngc_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<ngc_interface> srsgnb::srs_cu_cp::create_ngc(timer_manager&        timers,
                                                             ngc_message_notifier& message_notifier)
{
  auto ngc = std::make_unique<ngc_impl>(timers, message_notifier);
  return ngc;
}
