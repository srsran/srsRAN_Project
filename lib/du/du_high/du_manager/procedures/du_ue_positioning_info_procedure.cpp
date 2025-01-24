/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ue_positioning_info_procedure.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<du_positioning_meas_response>
srs_du::start_positioning_measurement_procedure(const du_positioning_meas_request& req)
{
  srslog::fetch_basic_logger("DU-MNG").error(
      "Positioning Measurement Procedure Failed. Cause: Feature only supported in Enterprise version.");
  return launch_no_op_task(du_positioning_meas_response{});
}

async_task<du_positioning_info_response>
srs_du::start_du_ue_positioning_info_procedure(const du_positioning_info_request& msg,
                                               du_cell_manager&                   du_cells_,
                                               du_ue_manager&                     ue_mng_)
{
  srslog::fetch_basic_logger("DU-MNG").error(
      "Positioning Exchange Procedure Failed. Cause: Feature only supported in Enterprise version.");
  return launch_no_op_task(du_positioning_info_response{});
}

#endif
