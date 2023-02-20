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

#include "srsran/cu_cp/cu_cp.h"
#include "srsran/ngap/ngap.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between CU-CP and NGAP, to initialize connection procedures
class cu_cp_ngap_adapter : public cu_cp_ngap_control_notifier
{
public:
  void connect_ngap(ngap_connection_manager& ngap_conn_mng_) { ngap_conn_mng = &ngap_conn_mng_; }

  async_task<ng_setup_response> on_ng_setup_request(const ng_setup_request& request) override
  {
    srsgnb_assert(ngap_conn_mng != nullptr, "NGAP handler must not be nullptr");
    return ngap_conn_mng->handle_ng_setup_request(request);
  }

private:
  ngap_connection_manager* ngap_conn_mng = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
