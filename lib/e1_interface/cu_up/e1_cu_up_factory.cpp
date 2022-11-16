/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/e1_interface/cu_up/e1_cu_up_factory.h"
#include "e1_cu_up_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_cu_up;

std::unique_ptr<e1_interface> srsgnb::srs_cu_up::create_e1(e1_message_notifier&    e1_pdu_notifier_,
                                                           e1_ue_manager_notifier& e1_ue_manager_notifier_)
{
  auto e1_cu_up = std::make_unique<e1_cu_up_impl>(e1_pdu_notifier_, e1_ue_manager_notifier_);
  return e1_cu_up;
}
