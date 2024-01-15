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

#include "srsran/cu_cp/du_processor.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class inter_ngran_node_n2_handover_routine
{
public:
  inter_ngran_node_n2_handover_routine(const cu_cp_inter_ngran_node_n2_handover_request& request_,
                                       du_processor_ngap_control_notifier&               ngap_ctrl_notifier_);

  void operator()(coro_context<async_task<cu_cp_inter_ngran_node_n2_handover_response>>& ctx);

private:
  du_processor_ngap_control_notifier& ngap_ctrl_notifier;

  ngap_handover_preparation_request  ho_prep_req;
  ngap_handover_preparation_response ho_prep_resp;

  const cu_cp_inter_ngran_node_n2_handover_request request;
  cu_cp_inter_ngran_node_n2_handover_response      response = {false};
};

} // namespace srs_cu_cp
} // namespace srsran
