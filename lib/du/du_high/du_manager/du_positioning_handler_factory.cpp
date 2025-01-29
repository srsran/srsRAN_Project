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

#ifndef SRSRAN_HAS_ENTERPRISE

#include "du_positioning_handler_factory.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

namespace {

class dummy_du_positioning_handler : public f1ap_du_positioning_handler
{
public:
  dummy_du_positioning_handler(srslog::basic_logger& logger_) : logger(logger_) {}

  du_trp_info_response request_trp_info() override
  {
    logger.error("TRP Information Exchange Procedure Failed. Cause: Feature only supported in Enterprise version");
    return {};
  }

  async_task<du_positioning_info_response> request_positioning_info(const du_positioning_info_request& req) override
  {
    logger.error("Positioning Exchange Procedure Failed. Cause: Feature only supported in Enterprise version");
    return launch_no_op_task(du_positioning_info_response{});
  }

  async_task<du_positioning_meas_response>
  request_positioning_measurement(const du_positioning_meas_request& req) override
  {
    logger.error("Positioning Measurement Procedure Failed. Cause: Feature only supported in Enterprise version");
    return launch_no_op_task(du_positioning_meas_response{});
  }

private:
  srslog::basic_logger& logger;
};

} // namespace

std::unique_ptr<f1ap_du_positioning_handler> srs_du::create_du_positioning_handler(const du_manager_params& du_params,
                                                                                   du_cell_manager&         cell_mng,
                                                                                   du_ue_manager&           ue_mng,
                                                                                   srslog::basic_logger&    logger)
{
  return std::make_unique<dummy_du_positioning_handler>(logger);
}

#endif
