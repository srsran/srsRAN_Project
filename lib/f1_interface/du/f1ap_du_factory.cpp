/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1_interface/du/f1ap_du_factory.h"
#include "common/f1ap_packet_procedures_impl.h"
#include "f1ap_du_impl.h"
#include "srsgnb/du_manager/du_manager.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;
using namespace srs_du;

std::unique_ptr<f1_interface> srsgnb::srs_du::create_f1ap(timer_manager& timers, f1c_message_notifier& message_notifier)
{
  auto f1ap_du = std::make_unique<f1ap_du_impl>(timers, message_notifier);
  return f1ap_du;
}
