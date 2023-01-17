/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/e2/common/e2_factory.h"
#include "e2_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

std::unique_ptr<e2_interface> srsgnb::create_e2(srsgnb::timer_manager& timers_, e2_message_notifier& e2_pdu_notifier_)
{
  auto e2 = std::make_unique<e2_impl>(timers_, e2_pdu_notifier_);
  return e2;
}
