/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_CU_FACTORY_H
#define SRSGNB_F1AP_CU_FACTORY_H

#include "srsgnb/f1_interface/f1ap_cu.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

/// Creates an instance of an F1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1_interface> create_f1ap(f1c_message_notifier&            f1c_pdu_notifier_,
                                          f1c_initiating_message_notifier& f1c_init_message_notifier_);

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_F1AP_CU_FACTORY_H
