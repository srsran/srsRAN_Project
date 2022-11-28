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

#include "e1_cu_cp.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

/// Creates an instance of an E1 interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e1_interface> create_e1(srsgnb::timer_manager&       timers_,
                                        e1_message_notifier&         e1_pdu_notifier_,
                                        e1_cu_up_processor_notifier& e1_cu_up_processor_notifier_);

} // namespace srs_cu_cp
} // namespace srsgnb
