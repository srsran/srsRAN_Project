/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between CU-CP and NGAP, to initialize connection procedures
class cu_cp_ngap_adapter : public cu_cp_ngap_control_notifier
{
public:
  void connect_ngap(ngc_connection_manager& ngc_conn_mng_) { ngc_conn_mng = &ngc_conn_mng_; }

  async_task<ng_setup_response> on_ng_setup_request(const ng_setup_request& request) override
  {
    srsgnb_assert(ngc_conn_mng != nullptr, "NGAP handler must not be nullptr");
    return ngc_conn_mng->handle_ng_setup_request(request);
  }

private:
  ngc_connection_manager* ngc_conn_mng = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb
