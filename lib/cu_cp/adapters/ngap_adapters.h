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
#include "../du_processor/du_processor.h"
#include "srsran/cu_cp/ue_manager.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between NGAP and CU-CP
class ngap_cu_cp_adapter : public ngap_cu_cp_du_repository_notifier, public ngap_cu_cp_notifier
{
public:
  explicit ngap_cu_cp_adapter() = default;

  void connect_cu_cp(du_repository_ngap_handler& du_repository_handler_, cu_cp_ngap_handler& cu_cp_handler_)
  {
    du_repository_handler = &du_repository_handler_;
    cu_cp_handler         = &cu_cp_handler_;
  }

  ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t cgi) override
  {
    srsran_assert(du_repository_handler != nullptr, "CU-CP Paging handler must not be nullptr");
    return du_repository_handler->handle_ue_index_allocation_request(cgi);
  }

  void on_paging_message(cu_cp_paging_message& msg) override
  {
    srsran_assert(du_repository_handler != nullptr, "CU-CP Paging handler must not be nullptr");
    du_repository_handler->handle_paging_message(msg);
  }

  async_task<ngap_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_ngap_handover_request(request);
  }

  ngap_ue_notifier* on_new_ngap_ue(ue_index_t ue_index) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_ngap_ue(ue_index);
  }

  bool on_ue_task_schedule_required(ue_index_t ue_index, async_task<void> task) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->schedule_ue_task(ue_index, std::move(task));
  }

  async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_pdu_session_resource_setup_request(request);
  }

  async_task<cu_cp_pdu_session_resource_modify_response>
  on_new_pdu_session_resource_modify_request(cu_cp_pdu_session_resource_modify_request& request) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_pdu_session_resource_modify_request(request);
  }

  async_task<cu_cp_pdu_session_resource_release_response>
  on_new_pdu_session_resource_release_command(cu_cp_pdu_session_resource_release_command& command) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_pdu_session_resource_release_command(command);
  }

  async_task<cu_cp_ue_context_release_complete>
  on_new_ue_context_release_command(const cu_cp_ue_context_release_command& command) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_ue_context_release_command(command);
  }

  async_task<bool> on_new_handover_command(ue_index_t ue_index, byte_buffer command) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_handover_command(ue_index, std::move(command));
  }

  void on_n2_disconnection() override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_n2_disconnection();
  }

private:
  du_repository_ngap_handler* du_repository_handler = nullptr;
  cu_cp_ngap_handler*         cu_cp_handler         = nullptr;
};

/// Adapter between NGAP and CU-CP UE
class ngap_cu_cp_ue_adapter : public ngap_ue_notifier
{
public:
  ngap_cu_cp_ue_adapter() = default;

  void connect_ngap_ue(ngap_ue& ue_) { ue = &ue_; }

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() override
  {
    srsran_assert(ue != nullptr, "NGAP UE must not be nullptr");
    return ue->get_ue_index();
  }

  /// \brief Get a map of all PDU sessions of the UE as const reference.
  [[nodiscard]] const std::map<pdu_session_id_t, up_pdu_session_context>& get_pdu_sessions_map() const override
  {
    srsran_assert(ue != nullptr, "NGAP UE must not be nullptr");
    return ue->get_up_resource_manager().get_pdu_sessions_map();
  }

  /// \brief Schedule an async task for the UE.
  bool schedule_async_task(async_task<void> task) override
  {
    srsran_assert(ue != nullptr, "NGAP UE must not be nullptr");
    return ue->get_task_sched().schedule_async_task(std::move(task));
  }

  /// \brief Get the RRC UE PDU notifier of the UE.
  ngap_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() override
  {
    srsran_assert(ue != nullptr, "NGAP UE must not be nullptr");
    return ue->get_rrc_ue_pdu_notifier();
  }

  /// \brief Get the RRC UE control notifier of the UE.
  ngap_rrc_ue_control_notifier& get_rrc_ue_control_notifier() override
  {
    srsran_assert(ue != nullptr, "NGAP UE must not be nullptr");
    return ue->get_rrc_ue_control_notifier();
  }

private:
  ngap_ue* ue = nullptr;
};

/// Adapter between NGAP and RRC UE
class ngap_rrc_ue_adapter : public ngap_rrc_ue_pdu_notifier, public ngap_rrc_ue_control_notifier
{
public:
  ngap_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_dl_nas_message_handler*           rrc_ue_msg_handler_,
                      rrc_ue_init_security_context_handler* rrc_ue_security_handler_,
                      rrc_ue_handover_preparation_handler*  rrc_ue_ho_prep_handler_)
  {
    rrc_ue_msg_handler      = rrc_ue_msg_handler_;
    rrc_ue_security_handler = rrc_ue_security_handler_;
    rrc_ue_ho_prep_handler  = rrc_ue_ho_prep_handler_;
  }

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    srsran_assert(rrc_ue_msg_handler != nullptr, "RRC UE message handler must not be nullptr");
    rrc_ue_msg_handler->handle_dl_nas_transport_message(std::move(nas_pdu));
  }

  async_task<bool> on_new_security_context(const security::security_context& sec_context) override
  {
    srsran_assert(rrc_ue_security_handler != nullptr, "RRC UE security handler must not be nullptr");
    return rrc_ue_security_handler->handle_init_security_context(sec_context);
  }

  bool on_security_enabled() override
  {
    srsran_assert(rrc_ue_security_handler != nullptr, "RRC UE security handler must not be nullptr");
    return rrc_ue_security_handler->get_security_enabled();
  }

  byte_buffer on_handover_preparation_message_required() override
  {
    srsran_assert(rrc_ue_ho_prep_handler != nullptr, "RRC UE up manager must not be nullptr");
    return rrc_ue_ho_prep_handler->get_packed_handover_preparation_message();
  }

private:
  rrc_dl_nas_message_handler*           rrc_ue_msg_handler      = nullptr;
  rrc_ue_init_security_context_handler* rrc_ue_security_handler = nullptr;
  rrc_ue_handover_preparation_handler*  rrc_ue_ho_prep_handler  = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
