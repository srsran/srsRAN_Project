/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/e1/cu_cp/e1_cu_cp_factory.h"
#include "e1_cu_cp_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_cu_cp;

std::unique_ptr<e1_interface> srsgnb::srs_cu_cp::create_e1(srsgnb::timer_manager&       timers_,
                                                           e1_message_notifier&         e1_pdu_notifier_,
                                                           e1_cu_up_processor_notifier& e1_cu_up_processor_notifier_)
{
  auto e1_cu_cp = std::make_unique<e1_cu_cp_impl>(timers_, e1_pdu_notifier_, e1_cu_up_processor_notifier_);
  return e1_cu_cp;
}
