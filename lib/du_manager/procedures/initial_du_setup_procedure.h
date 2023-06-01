/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/f1ap/du/f1ap_du_connection_manager.h"

namespace srsran {
namespace srs_du {

class du_cell_manager;
struct du_manager_params;

class initial_du_setup_procedure
{
public:
  initial_du_setup_procedure(const du_manager_params& params_, du_cell_manager& cell_mng_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  async_task<f1_setup_response_message> start_f1_setup_request();
  void                                  handle_f1_setup_response(const f1_setup_response_message& resp);

  const du_manager_params& params;
  du_cell_manager&         cell_mng;
  srslog::basic_logger&    logger;

  f1_setup_response_message response_msg = {};
};

} // namespace srs_du
} // namespace srsran
