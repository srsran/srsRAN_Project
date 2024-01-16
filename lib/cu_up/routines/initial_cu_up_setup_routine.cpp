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

#include "initial_cu_up_setup_routine.h"
#include "srsran/e1ap/common/e1_setup_messages.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace srs_cu_up;

initial_cu_up_setup_routine::initial_cu_up_setup_routine(const cu_up_configuration& cfg_) :
  cfg(cfg_), logger(srslog::fetch_basic_logger("CU-UP"))
{
}

void initial_cu_up_setup_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("cu-up={}: \"{}\" initialized.", cfg.cu_up_id, name());

  // Connect to CU-CP.
  if (not cfg.e1ap.e1ap_conn_mng->connect_to_cu_cp()) {
    report_fatal_error("Failed to connect to CU-CP");
  }

  // Initiate E1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_cu_up_e1_setup_request());

  // Handle E1 setup result.
  handle_cu_up_e1_setup_response(response_msg);

  CORO_RETURN();
}

async_task<cu_up_e1_setup_response> initial_cu_up_setup_routine::start_cu_up_e1_setup_request()
{
  // Prepare request to send to E1.
  cu_up_e1_setup_request request_msg = {};

  request_msg.gnb_cu_up_id   = cfg.cu_up_id;
  request_msg.gnb_cu_up_name = cfg.cu_up_name;

  // We only support 5G
  request_msg.cn_support = cu_up_cn_support_t::c_5gc;

  supported_plmns_item_t plmn_item;

  plmn_item.plmn_id = cfg.plmn;

  request_msg.supported_plmns.push_back(plmn_item);

  // Initiate E1 Setup Request.
  return cfg.e1ap.e1ap_conn_mng->handle_cu_up_e1_setup_request(request_msg);
}

void initial_cu_up_setup_routine::handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& resp)
{
  // TODO
  if (not resp.success) {
    report_fatal_error("CU-UP E1 Setup failed");
  }

  logger.debug("cu-up={}: \"{}\" finalized.", cfg.cu_up_id, name());
}
