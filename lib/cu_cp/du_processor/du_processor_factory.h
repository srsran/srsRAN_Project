/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../ue_manager/ue_manager_impl.h"
#include "du_processor.h"
#include "du_processor_config.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>

namespace srsran {
namespace srs_cu_cp {

class common_task_scheduler;

/// Creates an instance of an DU processor interface
std::unique_ptr<du_processor> create_du_processor(du_processor_config_t        du_processor_config_,
                                                  du_processor_cu_cp_notifier& cu_cp_notifier_,
                                                  f1ap_message_notifier&       f1ap_pdu_notifier_,
                                                  common_task_scheduler&       common_task_sched_,
                                                  ue_manager&                  ue_mng_);

} // namespace srs_cu_cp
} // namespace srsran
