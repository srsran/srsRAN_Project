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

#include "cu_up_setup_routine.h"
#include "srsran/e1ap/common/e1_setup_messages.h"
#include "srsran/srslog/srslog.h"
#include <utility>

using namespace srsran;
using namespace srs_cu_up;

cu_up_setup_routine::cu_up_setup_routine(gnb_cu_up_id_t           cu_up_id_,
                                         std::string              cu_up_name_,
                                         std::string              plmn_,
                                         e1ap_connection_manager& e1ap_conn_mng_) :
  cu_up_id(cu_up_id_),
  cu_up_name(std::move(cu_up_name_)),
  plmn(std::move(plmn_)),
  e1ap_conn_mng(e1ap_conn_mng_),
  logger(srslog::fetch_basic_logger("CU-UP"))
{
}

void cu_up_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("cu-up={}: \"{}\" initialized.", cu_up_id, name());

  // Connect to CU-CP.
  if (not e1ap_conn_mng.connect_to_cu_cp()) {
    CORO_EARLY_RETURN(false);
  }

  // Initiate E1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_cu_up_e1_setup_request());

  // Handle E1 setup result.
  handle_cu_up_e1_setup_response(response_msg);

  CORO_RETURN(true);
}

async_task<cu_up_e1_setup_response> cu_up_setup_routine::start_cu_up_e1_setup_request()
{
  // Prepare request to send to E1.
  cu_up_e1_setup_request request_msg = {};

  request_msg.gnb_cu_up_id   = gnb_cu_up_id_to_uint(cu_up_id);
  request_msg.gnb_cu_up_name = cu_up_name;

  // We only support 5G
  request_msg.cn_support = cu_up_cn_support_t::c_5gc;

  supported_plmns_item_t plmn_item;

  plmn_item.plmn_id = plmn;

  request_msg.supported_plmns.push_back(plmn_item);

  // Initiate E1 Setup Request.
  return e1ap_conn_mng.handle_cu_up_e1_setup_request(request_msg);
}

void cu_up_setup_routine::handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& resp)
{
  // TODO
  if (not resp.success) {
    report_fatal_error("CU-UP E1 Setup failed");
  }

  logger.debug("cu-up={}: \"{}\" finalized.", cu_up_id, name());
}
