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

/// Creates an instance of an F1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1_cu_interface> create_f1ap_cu(f1c_message_handler& f1c_handler);

} // namespace srsgnb

#endif // SRSGNB_F1AP_CU_FACTORY_H
