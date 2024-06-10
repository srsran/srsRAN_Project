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

#include "srsran/du/du_test_config.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// Creates an F1AP handler for the DU-high.
std::unique_ptr<f1ap_du> create_du_high_f1ap(f1c_connection_client&      f1c_client_handler,
                                             f1ap_du_configurator&       du_mng,
                                             task_executor&              ctrl_exec,
                                             du_high_ue_executor_mapper& ue_exec_mapper,
                                             f1ap_du_paging_notifier&    paging_notifier,
                                             const du_test_config&       test_cfg);

} // namespace srs_du
} // namespace srsran