/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/f1_interface/f1ap_cu_factory.h"
#include "f1ap_cu_impl.h"
#include "f1ap_packet_procedures_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

std::unique_ptr<f1_cu_interface> srsgnb::create_f1ap_cu(f1ap_cu_message_notifier& event_notifier)
{
  auto f1ap_cu = std::make_unique<f1ap_cu_impl>(event_notifier);
  return f1ap_cu;
}
