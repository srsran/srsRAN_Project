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

#include "inter_ngran_node_n2_handover_routine.h"

using namespace srsran;
using namespace srs_cu_cp;

inter_ngran_node_n2_handover_routine::inter_ngran_node_n2_handover_routine(
    const cu_cp_inter_ngran_node_n2_handover_request& request_,
    du_processor_ngap_control_notifier&               ngap_ctrl_notifier_) :
  ngap_ctrl_notifier(ngap_ctrl_notifier_), request(request_)
{
}

void inter_ngran_node_n2_handover_routine::operator()(
    coro_context<async_task<cu_cp_inter_ngran_node_n2_handover_response>>& ctx)
{
  CORO_BEGIN(ctx);
  ho_prep_req.ue_index = request.ue_index;
  ho_prep_req.gnb_id   = request.gnb_id;
  ho_prep_req.nci      = request.nci;
  CORO_AWAIT_VALUE(ho_prep_resp, ngap_ctrl_notifier.on_ngap_handover_preparation_request(ho_prep_req));
  CORO_RETURN(response);
}
