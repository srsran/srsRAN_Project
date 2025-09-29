/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between DU processor and CU-CP
class du_processor_cu_cp_adapter : public du_processor_cu_cp_notifier
{
public:
  void connect_cu_cp(cu_cp_du_event_handler&                cu_cp_mng_,
                     cu_cp_measurement_config_handler&      meas_config_handler_,
                     cu_cp_ue_removal_handler&              ue_removal_handler_,
                     cu_cp_ue_context_manipulation_handler& ue_context_handler_)
  {
    cu_cp_handler       = &cu_cp_mng_;
    meas_config_handler = &meas_config_handler_;
    ue_removal_handler  = &ue_removal_handler_;
    ue_context_handler  = &ue_context_handler_;
  }

  bool on_cell_config_update_request(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg) override
  {
    srsran_assert(meas_config_handler != nullptr, "Measurement config handler must not be nullptr");
    return meas_config_handler->handle_cell_config_update_request(nci, serv_cell_cfg);
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
  cu_cp_du_event_handler*                cu_cp_handler       = nullptr;
  cu_cp_measurement_config_handler*      meas_config_handler = nullptr;
  cu_cp_ue_removal_handler*              ue_removal_handler  = nullptr;
  cu_cp_ue_context_manipulation_handler* ue_context_handler  = nullptr;
};

class du_processor_cu_cp_connection_adapter final : public du_connection_notifier
{
public:
  void connect_node_connection_handler(cu_cp_controller& cu_ctrl_) { cu_ctrl = &cu_ctrl_; }

  bool on_du_setup_request(du_index_t du_index, const std::set<plmn_identity>& plmn_ids) override
  {
    srsran_assert(cu_ctrl != nullptr, "CU-CP controller must not be nullptr");
    return cu_ctrl->handle_du_setup_request(du_index, plmn_ids);
  }

private:
  cu_cp_controller* cu_ctrl = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
