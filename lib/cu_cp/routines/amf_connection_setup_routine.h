/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cu_cp_impl_interface.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the setup of the connection between the CU-CP and AMF, handling in particular the NG Setup procedure.
class amf_connection_setup_routine
{
public:
  amf_connection_setup_routine(ngap_connection_manager& ngap_conn_mng_);

  void operator()(coro_context<async_task<bool>>& ctx);

private:
  async_task<ngap_ng_setup_result> send_ng_setup_request();

  ngap_connection_manager& ngap_conn_mng;

  ngap_ng_setup_result result_msg = {};
};

} // namespace srs_cu_cp
} // namespace srsran
