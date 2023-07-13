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

#include "../cu_cp_impl_interface.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class source_inter_gnb_handover_handover_procedure
{
public:
  source_inter_gnb_handover_handover_procedure(cu_cp_ngap_control_notifier&    ngap_ctrl_notifier_,
                                               ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  cu_cp_ngap_control_notifier&    ngap_ctrl_notifier;
  ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier;
};

} // namespace srs_cu_cp
} // namespace srsran
