/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/e2/e2_factory.h"
#include "e2_impl.h"

using namespace srsran;

std::unique_ptr<e2_interface> srsran::create_e2(srsran::timer_manager& timers_, e2_message_notifier& e2_pdu_notifier_)
{
  auto e2 = std::make_unique<e2_impl>(timers_, e2_pdu_notifier_);
  return e2;
}
