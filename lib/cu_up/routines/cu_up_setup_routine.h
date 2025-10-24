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

#include "srsran/cu_up/cu_up_config.h"
#include "srsran/e1ap/common/e1_setup_messages.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_up {

class cu_up_setup_routine
{
public:
  cu_up_setup_routine(gnb_cu_up_id_t           cu_up_id_,
                      std::string              cu_up_name_,
                      std::string              plmn_,
                      e1ap_connection_manager& e1ap_conn_mng_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "CU-UP setup routine"; }

private:
  async_task<cu_up_e1_setup_response> start_cu_up_e1_setup_request();
  void                                handle_cu_up_e1_setup_response(const cu_up_e1_setup_response& resp);

  gnb_cu_up_id_t           cu_up_id;
  std::string              cu_up_name;
  std::string              plmn;
  e1ap_connection_manager& e1ap_conn_mng;

  srslog::basic_logger& logger;

  cu_up_e1_setup_response response_msg = {};
};

} // namespace srs_cu_up
} // namespace srsran
