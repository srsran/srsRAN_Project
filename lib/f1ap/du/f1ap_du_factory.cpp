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

#include "srsran/f1ap/du/f1ap_du_factory.h"
#include "f1ap_du_impl.h"
#include "srsran/du_manager/du_manager.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;
using namespace srs_du;

std::unique_ptr<f1ap_du> srsran::srs_du::create_f1ap(f1c_connection_client&      f1c_client_handler,
                                                     f1ap_du_configurator&       du_mng,
                                                     task_executor&              ctrl_exec,
                                                     du_high_ue_executor_mapper& ue_exec_mapper,
                                                     f1ap_du_paging_notifier&    paging_notifier)
{
  auto f1ap_du = std::make_unique<f1ap_du_impl>(f1c_client_handler, du_mng, ctrl_exec, ue_exec_mapper, paging_notifier);
  return f1ap_du;
}
