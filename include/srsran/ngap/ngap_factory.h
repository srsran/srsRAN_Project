/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ngap.h"
#include "ngap_configuration.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {

namespace srs_cu_cp {

class n2_connection_client;

/// Creates an instance of an NGAP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<ngap_interface> create_ngap(const ngap_configuration&          ngap_cfg_,
                                            ngap_cu_cp_notifier&               cu_cp_ue_creation_notifier_,
                                            ngap_cu_cp_du_repository_notifier& cu_cp_du_repository_notifier_,
                                            n2_connection_client&              n2_gateway_handler_,
                                            timer_manager&                     timers_,
                                            task_executor&                     ctrl_exec_);

} // namespace srs_cu_cp

} // namespace srsran
