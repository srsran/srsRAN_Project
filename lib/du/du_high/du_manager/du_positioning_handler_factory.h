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

#include "srsran/f1ap/du/f1ap_du_positioning_handler.h"
#include "srsran/srslog/logger.h"

namespace srsran::srs_du {

class du_cell_manager;
class du_ue_manager;
struct du_manager_params;

std::unique_ptr<f1ap_du_positioning_handler> create_du_positioning_handler(const du_manager_params& du_params,
                                                                           du_cell_manager&         cell_mng,
                                                                           du_ue_manager&           ue_mng,
                                                                           srslog::basic_logger&    logger);

} // namespace srsran::srs_du
