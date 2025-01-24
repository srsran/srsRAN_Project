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

#include "du_cell_manager.h"
#include "du_ue/du_ue_manager.h"
#include "srsran/f1ap/du/f1ap_du_positioning_handler.h"

namespace srsran::srs_du {

class du_positioning_manager_impl : public f1ap_du_positioning_handler
{
public:
  du_positioning_manager_impl(du_cell_manager& cell_mng_, du_ue_manager& ue_mng_);

  async_task<du_positioning_info_response> request_positioning_info(const du_positioning_info_request& req) override;

  async_task<du_positioning_meas_response>
  request_positioning_measurement(const du_positioning_meas_request& req) override;

private:
  du_cell_manager& cell_mng;
  du_ue_manager&   ue_mng;
};

} // namespace srsran::srs_du
