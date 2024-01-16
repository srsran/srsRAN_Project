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

#include "cu_up_processor_factory.h"
#include "cu_up_processor_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<cu_up_processor_interface>
srsran::srs_cu_cp::create_cu_up_processor(const cu_up_processor_config_t cu_up_processor_config_,
                                          e1ap_message_notifier&         e1ap_notifier_,
                                          e1ap_cu_cp_notifier&           e1ap_cu_cp_notif_,
                                          cu_up_task_scheduler&          task_sched_,
                                          task_executor&                 ctrl_exec_)
{
  auto cu_up_processor = std::make_unique<cu_up_processor_impl>(
      cu_up_processor_config_, e1ap_notifier_, e1ap_cu_cp_notif_, task_sched_, ctrl_exec_);
  return cu_up_processor;
}
