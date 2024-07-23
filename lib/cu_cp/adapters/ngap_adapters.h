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

#include "../cu_cp_impl_interface.h"
#include "../du_processor/du_processor.h"
#include "../paging/paging_message_handler.h"
#include "../ue_manager/cu_cp_ue_impl_interface.h"
#include "srsran/cu_cp/ue_task_scheduler.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between NGAP and CU-CP
class ngap_cu_cp_adapter : public ngap_cu_cp_du_repository_notifier, public ngap_cu_cp_notifier
{
public:
  explicit ngap_cu_cp_adapter() = default;

  void connect_cu_cp(cu_cp_ngap_handler& cu_cp_handler_, paging_message_handler& paging_handler_)
  {
    cu_cp_handler  = &cu_cp_handler_;
    paging_handler = &paging_handler_;
  }

  void on_paging_message(cu_cp_paging_message& msg) override
  {
    srsran_assert(paging_handler != nullptr, "CU-CP Paging handler must not be nullptr");
    paging_handler->handle_paging_message(msg);
  }

  async_task<ngap_handover_resource_allocation_response>
  on_ngap_handover_request(const ngap_handover_request& request) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_ngap_handover_request(request);
  }

  ngap_cu_cp_ue_notifier* on_new_ngap_ue(ue_index_t ue_index) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_ngap_ue(ue_index);
  }

  bool schedule_async_task(ue_index_t ue_index, async_task<void> task) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->schedule_ue_task(ue_index, std::move(task));
  }

  bool on_handover_request_received(ue_index_t ue_index, security::security_context sec_ctxt) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_handover_request(ue_index, sec_ctxt);
  }

  async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  on_new_initial_context_setup_request(ngap_init_context_setup_request& request) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_new_initial_context_setup_request(request);
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

  ue_index_t request_new_ue_index_allocation(nr_cell_global_id_t cgi) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    return cu_cp_handler->handle_ue_index_allocation_request(cgi);
  }

  void on_n2_disconnection() override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP NGAP handler must not be nullptr");
    cu_cp_handler->handle_n2_disconnection();
  }

private:
  cu_cp_ngap_handler*     cu_cp_handler  = nullptr;
  paging_message_handler* paging_handler = nullptr;
};

/// Adapter between NGAP and CU-CP UE
class ngap_cu_cp_ue_adapter : public ngap_cu_cp_ue_notifier
{
public:
  ngap_cu_cp_ue_adapter() = default;

  void connect_ue(cu_cp_ue_impl_interface& ue_) { ue = &ue_; }

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_ue_index();
  }

  /// \brief Schedule an async task for the UE.
  bool schedule_async_task(async_task<void> task) override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_task_sched().schedule_async_task(std::move(task));
  }

  /// \brief Get the RRC UE PDU notifier of the UE.
  ngap_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_rrc_ue_pdu_notifier();
  }

  /// \brief Get the RRC UE control notifier of the UE.
  ngap_rrc_ue_control_notifier& get_rrc_ue_control_notifier() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_rrc_ue_control_notifier();
  }

  bool init_security_context(security::security_context sec_ctxt) override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().init_security_context(sec_ctxt);
  }

  [[nodiscard]] bool is_security_enabled() const override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().is_security_enabled();
  }

private:
  cu_cp_ue_impl_interface* ue = nullptr;
};

/// Adapter between NGAP and RRC UE
class ngap_rrc_ue_adapter : public ngap_rrc_ue_pdu_notifier, public ngap_rrc_ue_control_notifier
{
public:
  ngap_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_dl_nas_message_handler&             rrc_ue_msg_handler_,
                      rrc_ue_radio_access_capability_handler& rrc_ue_radio_access_cap_handler_,
                      rrc_ue_handover_preparation_handler&    rrc_ue_ho_prep_handler_)
  {
    rrc_ue_msg_handler              = &rrc_ue_msg_handler_;
    rrc_ue_radio_access_cap_handler = &rrc_ue_radio_access_cap_handler_;
    rrc_ue_ho_prep_handler          = &rrc_ue_ho_prep_handler_;
  }

  void on_new_pdu(byte_buffer nas_pdu) override
  {
    srsran_assert(rrc_ue_msg_handler != nullptr, "RRC UE message handler must not be nullptr");
    rrc_ue_msg_handler->handle_dl_nas_transport_message(std::move(nas_pdu));
  }

  byte_buffer on_ue_radio_access_cap_info_required() override
  {
    srsran_assert(rrc_ue_radio_access_cap_handler != nullptr, "RRC UE Radio Access Cap handler must not be nullptr");
    return rrc_ue_radio_access_cap_handler->get_packed_ue_radio_access_cap_info();
  }

  byte_buffer on_handover_preparation_message_required() override
  {
    srsran_assert(rrc_ue_ho_prep_handler != nullptr, "RRC UE UP manager must not be nullptr");
    return rrc_ue_ho_prep_handler->get_packed_handover_preparation_message();
  }

private:
  rrc_dl_nas_message_handler*             rrc_ue_msg_handler              = nullptr;
  rrc_ue_radio_access_capability_handler* rrc_ue_radio_access_cap_handler = nullptr;
  rrc_ue_handover_preparation_handler*    rrc_ue_ho_prep_handler          = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
