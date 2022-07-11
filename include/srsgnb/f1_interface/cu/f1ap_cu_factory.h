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

#include "f1ap_cu.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

/// Creates an instance of an F1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1_interface> create_f1ap(f1c_message_notifier&              f1c_pdu_notifier_,
                                          f1c_du_processor_message_notifier& du_processor_notifier_,
                                          f1c_rrc_message_notifier&          rrc_message_notifier_,
                                          f1c_du_management_notifier&        f1c_du_management_notifier_);

} // namespace srs_cu_cp
} // namespace srsgnb
