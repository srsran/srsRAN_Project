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

#include "../cu_cp_controller/cu_cp_ue_admission_controller.h"
#include "../cu_cp_impl_interface.h"
#include "../ue_manager/cu_cp_ue_impl_interface.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/ue_task_scheduler.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between RRC UE and F1AP to pass RRC PDUs
class rrc_ue_f1ap_pdu_adapter : public rrc_pdu_f1ap_notifier
{
public:
  explicit rrc_ue_f1ap_pdu_adapter(f1ap_rrc_message_handler& f1ap_handler_, ue_index_t ue_index_) :
    f1ap_handler(f1ap_handler_), ue_index(ue_index_)
  {
  }

  void on_new_rrc_pdu(const srb_id_t srb_id, const byte_buffer& pdu) override
  {
    f1ap_dl_rrc_message f1ap_msg = {};
    f1ap_msg.ue_index            = ue_index;
    f1ap_msg.srb_id              = srb_id;
    f1ap_msg.rrc_container       = pdu.copy();
    f1ap_handler.handle_dl_rrc_message_transfer(f1ap_msg);
  }

private:
  f1ap_rrc_message_handler& f1ap_handler;
  const ue_index_t          ue_index;
};

// Adapter between RRC UE and NGAP
class rrc_ue_ngap_adapter : public rrc_ue_nas_notifier, public rrc_ue_control_notifier
{
public:
  void connect_ngap(ngap_nas_message_handler&     ngap_nas_msg_handler_,
                    ngap_control_message_handler& ngap_ctrl_msg_handler_)
  {
    ngap_nas_msg_handler  = &ngap_nas_msg_handler_;
    ngap_ctrl_msg_handler = &ngap_ctrl_msg_handler_;
  }

  void on_initial_ue_message(const cu_cp_initial_ue_message& msg) override
  {
    srsran_assert(ngap_nas_msg_handler != nullptr, "NGAP handler must not be nullptr");
    ngap_nas_msg_handler->handle_initial_ue_message(msg);
  }

  void on_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg) override
  {
    srsran_assert(ngap_nas_msg_handler != nullptr, "NGAP handler must not be nullptr");
    ngap_nas_msg_handler->handle_ul_nas_transport_message(msg);
  }

  void on_inter_cu_ho_rrc_recfg_complete_received(const ue_index_t           ue_index,
                                                  const nr_cell_global_id_t& cgi,
                                                  const unsigned             tac) override
  {
    srsran_assert(ngap_ctrl_msg_handler != nullptr, "NGAP handler must not be nullptr");

    ngap_ctrl_msg_handler->handle_inter_cu_ho_rrc_recfg_complete(ue_index, cgi, tac);
  }

private:
  ngap_nas_message_handler*     ngap_nas_msg_handler  = nullptr;
  ngap_control_message_handler* ngap_ctrl_msg_handler = nullptr;
};

/// Adapter between RRC UE and CU-CP UE
class rrc_ue_cu_cp_ue_adapter : public rrc_ue_cu_cp_ue_notifier
{
public:
  rrc_ue_cu_cp_ue_adapter() = default;

  void connect_ue(cu_cp_ue_impl_interface& ue_) { ue = &ue_; }

  /// \brief Schedule an async task for the UE.
  bool schedule_async_task(async_task<void> task) override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_task_sched().schedule_async_task(std::move(task));
  }

  timer_factory get_timer_factory() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_task_sched().get_timer_factory();
  }

  task_executor& get_executor() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_task_sched().get_executor();
  }

  /// \brief Get the AS configuration for the RRC domain
  security::sec_as_config get_rrc_as_config() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().get_rrc_as_config();
  }

  /// \brief Get the AS configuration for the RRC domain with 128-bit keys
  security::sec_128_as_config get_rrc_128_as_config() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().get_rrc_128_as_config();
  }

  /// \brief Enable security
  void enable_security() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().enable_security();
  }

  /// \brief Get the current security context
  security::security_context get_security_context() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().get_security_context();
  }

  /// \brief Get the selected security algorithms
  security::sec_selected_algos get_security_algos() override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().get_security_algos();
  }

  /// \brief Update the security context
  /// \param[in] sec_ctxt The new security context
  void update_security_context(security::security_context sec_ctxt) override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().update_security_context(sec_ctxt);
  }

  /// \brief Perform horizontal key derivation
  void perform_horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn) override
  {
    srsran_assert(ue != nullptr, "CU-CP UE must not be nullptr");
    return ue->get_security_manager().perform_horizontal_key_derivation(target_pci, target_ssb_arfcn);
  }

private:
  cu_cp_ue_impl_interface* ue = nullptr;
};

/// Adapter between RRC UE and CU-CP
class rrc_ue_cu_cp_adapter : public rrc_ue_context_update_notifier, public rrc_ue_measurement_notifier
{
public:
  rrc_ue_cu_cp_adapter(ue_index_t ue_index_) : ue_index(ue_index_) {}

  void connect_cu_cp(cu_cp_rrc_ue_interface&        cu_cp_rrc_ue_,
                     cu_cp_ue_removal_handler&      ue_removal_handler_,
                     cu_cp_ue_admission_controller& ctrl_,
                     up_resource_manager&           up_mng_,
                     cu_cp_measurement_handler&     meas_handler_)
  {
    cu_cp_rrc_ue_handler = &cu_cp_rrc_ue_;
    ue_removal_handler   = &ue_removal_handler_;
    controller           = &ctrl_;
    up_mng               = &up_mng_;
    meas_handler         = &meas_handler_;
  }

  bool on_ue_setup_request() override
  {
    srsran_assert(controller != nullptr, "CU-CP controller must not be nullptr");
    return controller->request_ue_setup();
  }

  rrc_ue_reestablishment_context_response on_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti) override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_rrc_reestablishment_request(old_pci, old_c_rnti, ue_index);
  }

  async_task<bool> on_rrc_reestablishment_context_modification_required() override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_rrc_reestablishment_context_modification_required(ue_index);
  }

  void on_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_rrc_reestablishment_failure(request);
  }

  void on_rrc_reestablishment_complete(ue_index_t old_ue_index) override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_rrc_reestablishment_complete(old_ue_index);
  }

  async_task<bool> on_ue_transfer_required(ue_index_t old_ue_index) override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_ue_context_transfer(ue_index, old_ue_index);
  }

  async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) override
  {
    srsran_assert(cu_cp_rrc_ue_handler != nullptr, "CU-CP handler must not be nullptr");
    return cu_cp_rrc_ue_handler->handle_ue_context_release(request);
  }

  void on_up_context_setup_required(up_context ctxt) override
  {
    srsran_assert(up_mng != nullptr, "UP resource manager must not be nullptr");
    up_mng->set_up_context(ctxt);
  }

  up_context on_up_context_required() override
  {
    srsran_assert(up_mng != nullptr, "UP resource manager must not be nullptr");
    return up_mng->get_up_context();
  }

  async_task<void> on_ue_removal_required() override
  {
    srsran_assert(ue_removal_handler != nullptr, "CU-CP UE removal handler must not be nullptr");
    return ue_removal_handler->handle_ue_removal_request(ue_index);
  }

  std::optional<rrc_meas_cfg>
  on_measurement_config_request(nr_cell_identity nci, std::optional<rrc_meas_cfg> current_meas_config = {}) override
  {
    srsran_assert(meas_handler != nullptr, "Measurement handler must not be nullptr");
    return meas_handler->handle_measurement_config_request(ue_index, nci, current_meas_config);
  }

  void on_measurement_report(const rrc_meas_results& meas_results) override
  {
    srsran_assert(meas_handler != nullptr, "Measurement handler must not be nullptr");
    meas_handler->handle_measurement_report(ue_index, meas_results);
  }

private:
  cu_cp_rrc_ue_interface*        cu_cp_rrc_ue_handler = nullptr;
  cu_cp_ue_removal_handler*      ue_removal_handler   = nullptr;
  up_resource_manager*           up_mng               = nullptr;
  cu_cp_ue_admission_controller* controller           = nullptr;
  cu_cp_measurement_handler*     meas_handler         = nullptr;
  ue_index_t                     ue_index;
};

} // namespace srs_cu_cp
} // namespace srsran
