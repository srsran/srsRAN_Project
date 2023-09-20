/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/e1ap/cu_cp/e1ap_cu_cp_factory.h"
#include "e1ap_cu_cp_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<e1ap_interface>
srsran::srs_cu_cp::create_e1ap(e1ap_message_notifier&         e1ap_pdu_notifier_,
                               e1ap_cu_up_processor_notifier& e1ap_cu_up_processor_notifier_,
                               e1ap_cu_cp_notifier&           cu_cp_notifier_,
                               timer_manager&                 timers_,
                               task_executor&                 ctrl_exec_)
{
  auto e1ap_cu_cp = std::make_unique<e1ap_cu_cp_impl>(
      e1ap_pdu_notifier_, e1ap_cu_up_processor_notifier_, cu_cp_notifier_, timers_, ctrl_exec_);
  return e1ap_cu_cp;
}
