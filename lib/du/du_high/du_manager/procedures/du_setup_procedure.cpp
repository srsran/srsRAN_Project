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

#include "du_setup_procedure.h"
#include "../converters/asn1_sys_info_packer.h"
#include "../converters/f1ap_configuration_helpers.h"
#include "../converters/scheduler_configuration_helpers.h"
#include "../du_cell_manager.h"
#include "../du_manager_context.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/scheduler/config/scheduler_cell_config_validator.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_no_op_task.h"
#include "srsran/support/async/async_timer.h"

using namespace srsran;
using namespace srs_du;

du_setup_procedure::du_setup_procedure(const du_proc_context_view& ctxt_, const du_start_request& request_) :
  ctxt(ctxt_),
  request(request_),
  proc_logger(ctxt.logger, "DU setup"),
  timer(ctxt.params.services.timers.create_unique_timer(ctxt.params.services.du_mng_exec))
{
}

void du_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  proc_logger.log_proc_started();

  // Establish TNL association with the CU-CP.
  for (; count != request.max_f1c_tnl_connection_retries and not ctxt.ctxt.stop_command_received and
         not ctxt.params.f1ap.conn_mng.connect_to_cu_cp();
       ++count) {
    ctxt.logger.warning("F1-C TNL association with CU-CP attempt {}/{} failed. Retrying in {} ms...",
                        count + 1,
                        request.max_f1c_tnl_connection_retries,
                        request.f1c_tnl_connection_retry_wait.count());
    CORO_AWAIT(async_wait_for(timer, request.f1c_tnl_connection_retry_wait));
  }
  if (count == request.max_f1c_tnl_connection_retries) {
    report_error("F1 Setup failed. Cause: F1-C TNL connection failed");
  }
  if (ctxt.ctxt.stop_command_received) {
    // DU is being shutdown.
    CORO_EARLY_RETURN();
  }

  // Configure cells.
  configure_du_cells();

  // Initiate F1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_f1_setup_request());

  // Handle F1 setup result and activate cells.
  CORO_AWAIT(handle_f1_setup_response(response_msg));

  proc_logger.log_proc_completed();

  CORO_RETURN();
}

void du_setup_procedure::configure_du_cells()
{
  if (not request.configure_cells) {
    // No need to reconfigure cells.
    return;
  }
  ctxt.cell_mng.remove_all_cells();

  // Save cell configurations.
  for (const du_cell_config& cell : ctxt.params.ran.cells) {
    ctxt.cell_mng.add_cell(cell);
  }

  // Configure MAC Cells (without activating them).
  for (unsigned idx = 0, e = ctxt.cell_mng.nof_cells(); idx != e; ++idx) {
    du_cell_index_t                 cell_index = to_du_cell_index(idx);
    const du_cell_config&           du_cfg     = ctxt.cell_mng.get_cell_cfg(cell_index);
    const mac_cell_sys_info_config& sys_info   = ctxt.cell_mng.get_sys_info(cell_index);

    auto                    sched_cfg = make_sched_cell_config_req(cell_index,
                                                du_cfg,
                                                units::bytes{static_cast<unsigned>(sys_info.sib1.length())},
                                                sys_info.si_sched_cfg.si_sched_cfg);
    error_type<std::string> result =
        config_validators::validate_sched_cell_configuration_request_message(sched_cfg, ctxt.params.mac.sched_cfg);
    if (not result.has_value()) {
      report_error("Invalid cell={} configuration. Cause: {}", fmt::underlying(cell_index), result.error());
    }

    // Forward config to MAC.
    ctxt.params.mac.mgr.get_cell_manager().add_cell(
        make_mac_cell_config(cell_index, du_cfg, sys_info.sib1, sys_info.si_messages, sched_cfg));
  }
}

async_task<f1_setup_result> du_setup_procedure::start_f1_setup_request()
{
  // Prepare request to send to F1.
  f1_setup_request_message req = {};

  req.gnb_du_id   = ctxt.params.ran.gnb_du_id;
  req.gnb_du_name = ctxt.params.ran.gnb_du_name;
  req.rrc_version = ctxt.params.ran.rrc_version;

  bool log_si_info = ctxt.logger.info.enabled();
  req.served_cells.reserve(ctxt.cell_mng.nof_cells());
  for (unsigned i = 0, e = ctxt.cell_mng.nof_cells(); i != e; ++i) {
    du_cell_index_t       cell_index  = to_du_cell_index(i);
    const du_cell_config& du_cell_cfg = ctxt.cell_mng.get_cell_cfg(cell_index);
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
      ctxt.logger.info(serv_cell.du_sys_info.packed_sib1.begin(),
                       serv_cell.du_sys_info.packed_sib1.end(),
                       "SIB1 cell={}: {}",
                       fmt::underlying(to_du_cell_index(i)),
                       js_str);
    }
  }

  // Initiate F1 Setup Request.
  return ctxt.params.f1ap.conn_mng.handle_f1_setup_request(req);
}

async_task<void> du_setup_procedure::handle_f1_setup_response(const f1_setup_result& resp)
{
  if (not resp.has_value()) {
    switch (resp.error().result) {
      case f1_setup_failure::result_code::f1_setup_failure:
        failure_cause = "CU-CP responded with \"F1 Setup Failure\"";
        if (resp.error().f1_setup_failure_cause != "unspecified") {
          failure_cause += fmt::format(" with F1AP cause \"{}\"", resp.error().f1_setup_failure_cause);
        }
        break;
      case f1_setup_failure::result_code::invalid_response:
        failure_cause = "CU-CP response to F1 Setup Request is invalid";
        break;
      case f1_setup_failure::result_code::timeout:
        failure_cause = "CU-CP did not respond to F1 Setup Request";
        break;
      case f1_setup_failure::result_code::proc_failure:
        failure_cause = "DU failed to run F1 Setup Procedure";
      default:
        report_fatal_error("Invalid F1 Setup Response");
    }

    if (count < request.max_f1c_tnl_connection_retries - 1) {
      ctxt.logger.warning("F1 Setup attempt {}/{} failed. Cause: {}. Retrying in {} ms...",
                          count + 1,
                          request.max_f1c_tnl_connection_retries,
                          failure_cause,
                          request.f1c_tnl_connection_retry_wait.count());
      return launch_async([this](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_AWAIT(async_wait_for(timer, request.f1c_tnl_connection_retry_wait));
        CORO_RETURN();
      });
    }
    ctxt.logger.error("F1 Setup attempt {}/{} failed. Cause: {}. No more retries left.",
                      count + 1,
                      request.max_f1c_tnl_connection_retries,
                      failure_cause);
    return launch_no_op_task();
  }

  // Success case.

  // Activate respective cells.
  return launch_async(
      [this, cells_to_activ = resp.value().cells_to_activate, i = 0U](coro_context<async_task<void>>& ctx) mutable {
        CORO_BEGIN(ctx);

        for (; i != cells_to_activ.size(); ++i) {
          // Start MAC cell.
          CORO_AWAIT(ctxt.cell_mng.start(ctxt.cell_mng.get_cell_index(cells_to_activ[i].cgi)));

          // Add cell to metrics.
          ctxt.metrics.handle_cell_start(ctxt.cell_mng.get_cell_index(cells_to_activ[i].cgi));
        }

        CORO_RETURN();
      });
}
