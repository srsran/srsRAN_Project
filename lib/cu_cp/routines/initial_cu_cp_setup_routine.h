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

#pragma once

#include "../cu_cp_impl_interface.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class initial_cu_cp_setup_routine
{
public:
  initial_cu_cp_setup_routine(const ngap_configuration&       ngap_cfg_,
                              cu_cp_ngap_control_notifier&    ngap_ctrl_notifier_,
                              ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  ngap_ng_setup_request            fill_ng_setup_request();
  async_task<ngap_ng_setup_result> send_ng_setup_request();
  void                             handle_ng_setup_result();

  const ngap_configuration&       ngap_cfg;
  cu_cp_ngap_control_notifier&    ngap_ctrl_notifier;
  ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier;

  ngap_ng_setup_result result_msg = {};
};

} // namespace srs_cu_cp
} // namespace srsran
