/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "e2.h"
#include "srsran/support/timers2.h"
#include <memory>

namespace srsran {

/// Creates an instance of an E2 interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<e2_interface> create_e2(timer_factory timers_, e2_message_notifier& e2_pdu_notifier_);

} // namespace srsran
