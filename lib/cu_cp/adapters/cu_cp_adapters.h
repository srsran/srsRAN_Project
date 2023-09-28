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
#include "srsran/e1ap/cu_cp/e1ap_cu_cp.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between CU-CP and NGAP, to initialize connection procedures
class cu_cp_ngap_adapter : public cu_cp_ngap_control_notifier
{
public:
  void connect_ngap(ngap_connection_manager&         ngap_conn_mng_,
                    ngap_control_message_handler&    ngap_ctrl_handler_,
                    ngap_ue_context_removal_handler& ngap_ue_handler_)
  {
    ngap_conn_mng     = &ngap_conn_mng_;
    ngap_ctrl_handler = &ngap_ctrl_handler_;
    ngap_ue_handler   = &ngap_ue_handler_;
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

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(ngap_ue_handler != nullptr, "NGAP handler must not be nullptr");
    return ngap_ue_handler->remove_ue_context(ue_index);
  }

private:
  ngap_connection_manager*         ngap_conn_mng     = nullptr;
  ngap_control_message_handler*    ngap_ctrl_handler = nullptr;
  ngap_ue_context_removal_handler* ngap_ue_handler   = nullptr;
};

/// Adapter between CU-CP and E1AP to request UE removal
class cu_cp_e1ap_adapter : public cu_cp_e1ap_ue_removal_notifier
{
public:
  cu_cp_e1ap_adapter() = default;

  void connect_e1ap(e1ap_bearer_context_removal_handler& e1ap_bearer_handler_)
  {
    e1ap_bearer_handler = &e1ap_bearer_handler_;
  }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(e1ap_bearer_handler != nullptr, "E1AP handler must not be nullptr");
    e1ap_bearer_handler->remove_bearer_context(ue_index);
  }

private:
  e1ap_bearer_context_removal_handler* e1ap_bearer_handler = nullptr;
};

/// Adapter between CU-CP and F1AP to request UE removal
class cu_cp_f1ap_adapter : public cu_cp_f1ap_ue_removal_notifier
{
public:
  cu_cp_f1ap_adapter() = default;

  void connect_f1ap(f1ap_ue_context_removal_handler& f1ap_ue_handler_) { f1ap_ue_handler = &f1ap_ue_handler_; }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(f1ap_ue_handler != nullptr, "F1AP handler must not be nullptr");
    f1ap_ue_handler->remove_ue_context(ue_index);
  }

private:
  f1ap_ue_context_removal_handler* f1ap_ue_handler = nullptr;
};

/// Adapter between CU-CP and RRC DU to request UE removal
class cu_cp_rrc_du_adapter : public cu_cp_rrc_ue_removal_notifier
{
public:
  cu_cp_rrc_du_adapter() = default;

  void connect_rrc_du(rrc_ue_removal_handler& rrc_du_handler_) { rrc_du_handler = &rrc_du_handler_; }

  void remove_ue(ue_index_t ue_index) override
  {
    srsran_assert(rrc_du_handler != nullptr, "RRC DU handler must not be nullptr");
    rrc_du_handler->remove_ue(ue_index);
  }

private:
  rrc_ue_removal_handler* rrc_du_handler = nullptr;
};

/// Adapter between CU-CP and RRC UE, to transfer UE context e.g. for RRC Reestablishments
class cu_cp_rrc_ue_adapter : public cu_cp_rrc_ue_context_transfer_notifier
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
