/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
