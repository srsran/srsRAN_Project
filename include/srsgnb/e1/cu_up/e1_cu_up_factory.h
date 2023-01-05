/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "e1_cu_up.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_up {

/// Creates an instance of an E1 interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1_interface> create_e1(e1_message_notifier& e1_pdu_notifier_, e1ap_cu_cp_notifier& e1_cu_up_notifier_);

} // namespace srs_cu_up
} // namespace srsgnb
