/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
