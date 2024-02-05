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

#include "srsran/ngap/ngap_factory.h"
#include "ngap_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_cu_cp;

std::unique_ptr<ngap_interface>
srsran::srs_cu_cp::create_ngap(ngap_configuration&                ngap_cfg_,
                               ngap_cu_cp_ue_creation_notifier&   cu_cp_ue_creation_notifier_,
                               ngap_cu_cp_du_repository_notifier& cu_cp_du_repository_notifier_,
                               ngap_ue_task_scheduler&            task_sched_,
                               ngap_ue_manager&                   ue_manager_,
                               ngap_message_notifier&             ngap_notifier_,
                               task_executor&                     ctrl_exec_)
{
  auto ngap = std::make_unique<ngap_impl>(ngap_cfg_,
                                          cu_cp_ue_creation_notifier_,
                                          cu_cp_du_repository_notifier_,
                                          task_sched_,
                                          ue_manager_,
                                          ngap_notifier_,
                                          ctrl_exec_);
  return ngap;
}
