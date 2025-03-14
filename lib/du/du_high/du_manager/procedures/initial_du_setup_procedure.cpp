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

#include "initial_du_setup_procedure.h"
#include "../converters/f1ap_configuration_helpers.h"
#include "../converters/scheduler_configuration_helpers.h"
#include "../du_cell_manager.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/scheduler/config/scheduler_cell_config_validator.h"

using namespace srsran;
using namespace srs_du;

initial_du_setup_procedure::initial_du_setup_procedure(const du_manager_params& params_, du_cell_manager& cell_mng_) :
  params(params_), cell_mng(cell_mng_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void initial_du_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Connect to CU-CP.
  if (not params.f1ap.conn_mng.connect_to_cu_cp()) {
    report_error("Failed to connect DU to CU-CP");
  }

  // Configure cells.
  configure_du_cells();

  // Initiate F1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_f1_setup_request());

  // Handle F1 setup result and activate cells.
  CORO_AWAIT(handle_f1_setup_response(response_msg));

  CORO_RETURN();
}

void initial_du_setup_procedure::configure_du_cells()
{
  // Save cell configurations.
  for (const du_cell_config& cell : params.ran.cells) {
    cell_mng.add_cell(cell);
  }

  // Configure MAC Cells (without activating them).
  for (unsigned idx = 0, e = cell_mng.nof_cells(); idx != e; ++idx) {
    du_cell_index_t       cell_index = to_du_cell_index(idx);
    const du_cell_config& du_cfg     = cell_mng.get_cell_cfg(cell_index);

    std::vector<byte_buffer>  bcch_msgs = make_asn1_rrc_cell_bcch_dl_sch_msgs(du_cfg);
    std::vector<units::bytes> bcch_msg_payload_lens(bcch_msgs.size());
    for (unsigned i = 0; i < bcch_msgs.size(); ++i) {
      bcch_msg_payload_lens[i] = units::bytes(bcch_msgs[i].length());
    }
    auto                    sched_cfg = make_sched_cell_config_req(cell_index, du_cfg, bcch_msg_payload_lens);
    error_type<std::string> result =
        config_validators::validate_sched_cell_configuration_request_message(sched_cfg, params.mac.sched_cfg);
    if (not result.has_value()) {
      report_error("Invalid cell={} configuration. Cause: {}", fmt::underlying(cell_index), result.error());
    }

    // Forward config to MAC.
    params.mac.cell_mng.add_cell(make_mac_cell_config(cell_index, du_cfg, std::move(bcch_msgs), sched_cfg));
  }
}

async_task<f1_setup_result> initial_du_setup_procedure::start_f1_setup_request()
{
  // Prepare request to send to F1.
  f1_setup_request_message req = {};

  req.gnb_du_id   = params.ran.gnb_du_id;
  req.gnb_du_name = params.ran.gnb_du_name;
  req.rrc_version = params.ran.rrc_version;

  bool log_si_info = logger.info.enabled();
  req.served_cells.reserve(cell_mng.nof_cells());
  for (unsigned i = 0, e = cell_mng.nof_cells(); i != e; ++i) {
    du_cell_index_t       cell_index  = to_du_cell_index(i);
    const du_cell_config& du_cell_cfg = cell_mng.get_cell_cfg(cell_index);
    if (not du_cell_cfg.enabled) {
      // Served cell is disabled. Do not forward it to the CU-CP.
      continue;
    }
    auto& serv_cell = req.served_cells.emplace_back();

    // Fill serving cell info.
    serv_cell.cell_index = cell_index;
    serv_cell.cell_info  = make_f1ap_du_cell_info(du_cell_cfg);
    for (const auto& slice : du_cell_cfg.rrm_policy_members) {
      serv_cell.slices.push_back(slice.rrc_member.s_nssai);
    }

    // Pack RRC ASN.1 Serving Cell system info.
    std::string js_str;
    serv_cell.du_sys_info = make_f1ap_du_sys_info(du_cell_cfg, log_si_info ? &js_str : nullptr);

    // Log RRC ASN.1 SIB1 json.
    if (log_si_info) {
      logger.info(serv_cell.du_sys_info.packed_sib1.begin(),
                  serv_cell.du_sys_info.packed_sib1.end(),
                  "SIB1 cell={}: {}",
                  fmt::underlying(to_du_cell_index(i)),
                  js_str);
    }
  }

  // Initiate F1 Setup Request.
  return params.f1ap.conn_mng.handle_f1_setup_request(req);
}

async_task<void> initial_du_setup_procedure::handle_f1_setup_response(const f1_setup_result& resp)
{
  if (not resp.has_value()) {
    std::string cause;
    switch (resp.error().result) {
      case f1_setup_failure::result_code::f1_setup_failure:
        cause = "CU-CP responded with \"F1 Setup Failure\"";
        if (resp.error().f1_setup_failure_cause != "unspecified") {
          cause += fmt::format(" with F1AP cause \"{}\"", resp.error().f1_setup_failure_cause);
        }
        break;
      case f1_setup_failure::result_code::invalid_response:
        cause = "CU-CP response to F1 Setup Request is invalid";
        break;
      case f1_setup_failure::result_code::timeout:
        cause = "CU-CP did not respond to F1 Setup Request";
        break;
      case f1_setup_failure::result_code::proc_failure:
        cause = "DU failed to run F1 Setup Procedure";
      default:
        report_fatal_error("Invalid F1 Setup Response");
    }
    report_error("F1 Setup failed. Cause: {}", cause);
  }

  // Success case.

  // Activate respective cells.
  return launch_async(
      [this, cells_to_activ = resp.value().cells_to_activate, i = 0U](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        for (; i != cells_to_activ.size(); ++i) {
          // Start MAC cell.
          CORO_AWAIT(cell_mng.start(cell_mng.get_cell_index(cells_to_activ[i].cgi)));
        }

        CORO_RETURN();
      });
}
