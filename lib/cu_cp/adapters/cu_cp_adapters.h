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

namespace srsran {
namespace srs_cu_cp {

/// Adapter between CU-CP and NGAP, to initialize connection procedures
class cu_cp_ngap_adapter : public cu_cp_ngap_control_notifier
{
public:
  void connect_ngap(ngap_connection_manager& ngap_conn_mng_, ngap_control_message_handler& ngap_ctrl_handler_)
  {
    ngap_conn_mng     = &ngap_conn_mng_;
    ngap_ctrl_handler = &ngap_ctrl_handler_;
  }

  async_task<ng_setup_response> on_ng_setup_request(const ng_setup_request& request) override
  {
    srsran_assert(ngap_conn_mng != nullptr, "NGAP handler must not be nullptr");
    return ngap_conn_mng->handle_ng_setup_request(request);
  }

  void on_ue_context_release_request(const cu_cp_ue_context_release_request& request) override
  {
    srsran_assert(ngap_ctrl_handler != nullptr, "NGAP handler must not be nullptr");
    return ngap_ctrl_handler->handle_ue_context_release_request(request);
  }

private:
  ngap_connection_manager*      ngap_conn_mng     = nullptr;
  ngap_control_message_handler* ngap_ctrl_handler = nullptr;
};

/// Adapter between CU-CP and RRC UE, to transfer UE context e.g. for RRC Reestablishments
class cu_cp_rrc_ue_adapter : public cu_cp_rrc_ue_context_trasfer_notifier
{
public:
  cu_cp_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_ue_context_handler& rrc_context_handler_) { rrc_context_handler = &rrc_context_handler_; }

  rrc_reestablishment_ue_context_t on_rrc_ue_context_transfer() override
  {
    srsran_assert(rrc_context_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_context_handler->get_context();
  }

private:
  rrc_ue_context_handler* rrc_context_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
