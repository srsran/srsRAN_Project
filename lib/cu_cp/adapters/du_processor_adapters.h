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

#include "../cu_cp_controller/cu_cp_controller.h"
#include "../cu_cp_impl_interface.h"
#include "../du_processor/du_processor.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/support/srsran_assert.h"
#include <cstddef>

namespace srsran {
namespace srs_cu_cp {

/// Adapter between DU processor and CU-CP
class du_processor_cu_cp_adapter : public du_processor_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_du_event_handler&                cu_cp_mng_,
                     cu_cp_ue_removal_handler&              ue_removal_handler_,
                     cu_cp_ue_context_manipulation_handler& ue_context_handler_)
  {
    cu_cp_handler      = &cu_cp_mng_;
    ue_removal_handler = &ue_removal_handler_;
    ue_context_handler = &ue_context_handler_;
  }

  void on_du_processor_created(du_index_t                       du_index,
                               f1ap_ue_context_removal_handler& f1ap_handler,
                               f1ap_statistics_handler&         f1ap_statistic_handler,
                               rrc_ue_handler&                  rrc_handler,
                               rrc_du_statistics_handler&       rrc_statistic_handler) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_du_processor_creation(
        du_index, f1ap_handler, f1ap_statistic_handler, rrc_handler, rrc_statistic_handler);
  }

  void on_rrc_ue_created(ue_index_t ue_index, rrc_ue_interface& rrc_ue) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    cu_cp_handler->handle_rrc_ue_creation(ue_index, rrc_ue);
  }

  byte_buffer on_target_cell_sib1_required(du_index_t du_index, nr_cell_global_id_t cgi) override
  {
    srsran_assert(cu_cp_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_handler->handle_target_cell_sib1_required(du_index, cgi);
  }

  async_task<void> on_ue_removal_required(ue_index_t ue_index) override
  {
    srsran_assert(ue_removal_handler != nullptr, "CU-CP UE Removal handler must not be nullptr");
    return ue_removal_handler->handle_ue_removal_request(ue_index);
  }

  async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) override
  {
    srsran_assert(ue_context_handler != nullptr, "UE context handler must not be nullptr");
    return ue_context_handler->handle_ue_context_release(request);
  }

  async_task<void> on_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev) override
  {
    return cu_cp_handler->handle_transaction_info_loss(ev);
  }

private:
  cu_cp_du_event_handler*                cu_cp_handler      = nullptr;
  cu_cp_ue_removal_handler*              ue_removal_handler = nullptr;
  cu_cp_ue_context_manipulation_handler* ue_context_handler = nullptr;
};

// Adapter between DU processor and F1AP for UE context
class du_processor_f1ap_ue_context_adapter : public du_processor_f1ap_ue_context_notifier
{
public:
  du_processor_f1ap_ue_context_adapter() = default;

  void connect_f1(f1ap_ue_context_manager& handler_) { handler = &handler_; }

  async_task<ue_index_t> on_ue_context_release_command(const f1ap_ue_context_release_command& msg) override
  {
    srsran_assert(handler != nullptr, "F1AP handler must not be nullptr");
    return handler->handle_ue_context_release_command(msg);
  }

  async_task<f1ap_ue_context_modification_response>
  on_ue_context_modification_request(const f1ap_ue_context_modification_request& request) override
  {
    srsran_assert(handler != nullptr, "F1AP handler must not be nullptr");
    return handler->handle_ue_context_modification_request(request);
  }

  bool on_intra_du_reestablishment(ue_index_t ue_index, ue_index_t old_ue_index) override
  {
    srsran_assert(handler != nullptr, "F1AP handler must not be nullptr");
    return handler->handle_ue_id_update(ue_index, old_ue_index);
  }

private:
  f1ap_ue_context_manager* handler = nullptr;
};

// Adapter between DU processor and RRC DU
class du_processor_rrc_du_adapter : public du_processor_rrc_du_ue_notifier
{
public:
  du_processor_rrc_du_adapter() = default;

  void connect_rrc_du(rrc_du_cell_manager& rrc_du_cell_handler_, rrc_du_ue_repository& rrc_du_handler_)
  {
    rrc_du_cell_handler = &rrc_du_cell_handler_;
    rrc_du_handler      = &rrc_du_handler_;
  }

  bool on_new_served_cell_list(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) override
  {
    srsran_assert(rrc_du_cell_handler != nullptr, "RRC DU cell handler must not be nullptr");
    return rrc_du_cell_handler->handle_served_cell_list(served_cell_list);
  }

  byte_buffer on_rrc_reject_required() override
  {
    srsran_assert(rrc_du_handler != nullptr, "RRC DU UE handler must not be nullptr");
    return rrc_du_handler->get_rrc_reject();
  }

  rrc_ue_interface* on_ue_creation_request(const rrc_ue_creation_message& msg) override
  {
    srsran_assert(rrc_du_handler != nullptr, "RRC DU UE handler must not be nullptr");
    return rrc_du_handler->add_ue(msg);
  }

  void on_release_ues() override
  {
    srsran_assert(rrc_du_handler != nullptr, "RRC DU UE handler must not be nullptr");
    return rrc_du_handler->release_ues();
  }

private:
  rrc_du_cell_manager*  rrc_du_cell_handler = nullptr;
  rrc_du_ue_repository* rrc_du_handler      = nullptr;
};

// Adapter between DU processor and RRC UE
class du_processor_rrc_ue_adapter : public du_processor_rrc_ue_control_message_notifier,
                                    public du_processor_rrc_ue_srb_control_notifier
{
public:
  du_processor_rrc_ue_adapter() = default;

  void connect_rrc_ue(rrc_ue_control_message_handler& rrc_ue_handler_, rrc_ue_srb_handler& srb_handler_)
  {
    rrc_ue_handler = &rrc_ue_handler_;
    srb_handler    = &srb_handler_;
  }

  async_task<bool> on_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_rrc_ue_capability_transfer_request(msg);
  }

  async_task<bool> on_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_rrc_reconfiguration_request(msg);
  }

  byte_buffer get_packed_ue_capability_rat_container_list() override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_packed_ue_capability_rat_container_list();
  }

  rrc_ue_handover_reconfiguration_context
  get_rrc_ue_handover_reconfiguration_context(const rrc_reconfiguration_procedure_request& request) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_rrc_ue_handover_reconfiguration_context(request);
  }

  async_task<bool> on_handover_reconfiguration_complete_expected(uint8_t transaction_id) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_handover_reconfiguration_complete_expected(transaction_id);
  }

  rrc_ue_release_context get_rrc_ue_release_context(bool requires_rrc_msg) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_rrc_ue_release_context(requires_rrc_msg);
  }

  rrc_ue_transfer_context get_transfer_context() override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_transfer_context();
  }

  std::optional<rrc_meas_cfg> generate_meas_config(std::optional<rrc_meas_cfg> current_meas_config = {}) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->generate_meas_config(current_meas_config);
  }

  byte_buffer get_packed_handover_preparation_message() override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_packed_handover_preparation_message();
  }

  byte_buffer on_new_rrc_handover_command(byte_buffer cmd) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->handle_rrc_handover_command(std::move(cmd));
  }

  byte_buffer on_rrc_handover_command_required(const rrc_reconfiguration_procedure_request& request,
                                               unsigned                                     transaction_id) override
  {
    srsran_assert(rrc_ue_handler != nullptr, "RRC UE handler must not be nullptr");
    return rrc_ue_handler->get_rrc_handover_command(request, transaction_id);
  }

  void create_srb(const srb_creation_message& msg) override
  {
    srsran_assert(srb_handler != nullptr, "RRC UE SRB handler must not be nullptr");
    return srb_handler->create_srb(msg);
  }

  static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() override
  {
    srsran_assert(srb_handler != nullptr, "RRC UE SRB handler must not be nullptr");
    return srb_handler->get_srbs();
  }

private:
  rrc_ue_control_message_handler* rrc_ue_handler = nullptr;
  rrc_ue_srb_handler*             srb_handler    = nullptr;
};

class du_processor_cu_cp_connection_adapter final : public du_connection_notifier
{
public:
  void connect_node_connection_handler(cu_cp_controller& cu_ctrl_) { cu_ctrl = &cu_ctrl_; }

  bool on_du_setup_request(du_index_t du_index, const du_setup_request& req) override
  {
    srsran_assert(cu_ctrl != nullptr, "CU-CP controller must not be nullptr");
    return cu_ctrl->handle_du_setup_request(du_index, req);
  }

private:
  cu_cp_controller* cu_ctrl = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
