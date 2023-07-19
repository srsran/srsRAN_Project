/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
