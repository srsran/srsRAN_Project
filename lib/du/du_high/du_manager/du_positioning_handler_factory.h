/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1ap/du/f1ap_du_positioning_handler.h"
#include "srsran/srslog/logger.h"

namespace srsran::srs_du {

class du_cell_manager;
class du_ue_manager;

std::unique_ptr<f1ap_du_positioning_handler>
create_du_positioning_handler(du_cell_manager& cell_mng, du_ue_manager& ue_mng, srslog::basic_logger& logger);

} // namespace srsran::srs_du
