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

#include "../adapters/cu_cp_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../adapters/rrc_ue_adapters.h"
#include "../cell_meas_manager/measurement_context.h"
#include "../ue_security_manager/ue_security_manager_impl.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "cu_cp_ue_impl_interface.h"
#include "ue_task_scheduler_impl.h"
#include <optional>
#include <unordered_map>

namespace srsran {

namespace srs_cu_cp {

/// \brief Context of a CU-CP UE.
struct cu_cp_ue_context {
  du_index_t  du_idx   = du_index_t::invalid;
  gnb_du_id_t du_id    = gnb_du_id_t::invalid;
  ue_index_t  ue_index = ue_index_t::invalid;
  rnti_t      crnti    = rnti_t::INVALID_RNTI;
  /// \brief Flag to disable new UE reconfigurations. This can be used, for instance, to reconfigure UE contexts
  /// that are in the process of handover.
  bool reconfiguration_disabled = false;
};

class cu_cp_ue : public cu_cp_ue_impl_interface
{
public:
  cu_cp_ue(ue_index_t                     ue_index_,
           du_index_t                     du_index_,
           const up_resource_manager_cfg& up_cfg,
           const security_manager_config& sec_cfg,
           ue_task_scheduler_impl         task_sched_,
           std::optional<gnb_du_id_t>     du_id_       = std::nullopt,
           std::optional<pci_t>           pci_         = std::nullopt,
           std::optional<rnti_t>          c_rnti_      = std::nullopt,
           std::optional<du_cell_index_t> pcell_index_ = std::nullopt);

  /// \brief Cancel all pending UE tasks.
  void stop();

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() override { return ue_index; }

  /// \brief Get the PCI of the UE.
  [[nodiscard]] pci_t get_pci() const { return pci; };

  /// \brief Get the C-RNTI of the UE.
  [[nodiscard]] rnti_t get_c_rnti() const { return ue_ctxt.crnti; }

  [[nodiscard]] gnb_du_id_t get_du_id() const { return ue_ctxt.du_id; }

  /// \brief Get the DU index of the UE.
  [[nodiscard]] du_index_t get_du_index() const { return ue_ctxt.du_idx; }

  /// \brief Get the PCell index of the UE.
  du_cell_index_t get_pcell_index() { return pcell_index; }

  /// \brief Get the UP resource manager of the UE.
  up_resource_manager& get_up_resource_manager() override { return up_mng; }

  /// \brief Get the task scheduler of the UE.
  ue_task_scheduler& get_task_sched() override { return task_sched; }

  /// \brief Get the security manager of the UE.
  ue_security_manager& get_security_manager() override { return sec_mng; }

  cu_cp_ue_context&                     get_ue_context() { return ue_ctxt; }
  [[nodiscard]] const cu_cp_ue_context& get_ue_context() const { return ue_ctxt; }

  /// \brief Get the measurement context of the UE.
  cell_meas_manager_ue_context& get_meas_context() { return meas_context; }

  /// \brief Update a UE with PCI and/or C-RNTI.
  void update_du_ue(gnb_du_id_t     du_id_       = gnb_du_id_t::invalid,
                    pci_t           pci_         = INVALID_PCI,
                    rnti_t          c_rnti_      = rnti_t::INVALID_RNTI,
                    du_cell_index_t pcell_index_ = du_cell_index_t::invalid);

  /// \brief Set/update the measurement context of the UE.
  void update_meas_context(cell_meas_manager_ue_context meas_ctxt);

  /// \brief Check if the DU UE context is created.
  /// \return True if the DU UE context is created, false otherwise.
  [[nodiscard]] bool du_ue_created() const { return ue_ctxt.du_idx != du_index_t::invalid; }

  /// \brief Set the RRC UE control message notifier of the UE.
  /// \param[in] rrc_ue_notifier_ RRC UE control message notifier of the UE.
  void set_rrc_ue_notifier(du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_);

  /// \brief Set the RRC UE SRB notifier of the UE.
  /// \param[in] rrc_ue_srb_notifier_ RRC UE SRB control notifier of the UE.
  void set_rrc_ue_srb_notifier(du_processor_rrc_ue_srb_control_notifier& rrc_ue_srb_notifier_);

  /// \brief Get the RRC UE PDU notifier of the UE.
  ngap_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() override;

  /// \brief Get the RRC UE control notifier of the UE.
  ngap_rrc_ue_control_notifier& get_rrc_ue_control_notifier() override;

  /// \brief Get the NGAP CU-CP UE notifier of the UE.
  ngap_cu_cp_ue_notifier& get_ngap_cu_cp_ue_notifier() { return ngap_cu_cp_ue_ev_notifier; }

  /// \brief Get the RRC UE CU-CP UE notifier of the UE.
  rrc_ue_cu_cp_ue_notifier& get_rrc_ue_cu_cp_ue_notifier() { return rrc_ue_cu_cp_ue_ev_notifier; }

  /// \brief Get the RRC UE control message notifier of the UE.
  du_processor_rrc_ue_control_message_notifier& get_rrc_ue_notifier();

  /// \brief Get the RRC UE SRB control notifier of the UE.
  du_processor_rrc_ue_srb_control_notifier& get_rrc_ue_srb_notifier();

  rrc_ue_context_update_notifier& get_rrc_ue_context_update_notifier() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the RRC UE measurement notifier of the UE.
  rrc_ue_measurement_notifier& get_rrc_ue_measurement_notifier() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the NGAP to RRC UE adapter of the UE.
  ngap_rrc_ue_adapter& get_ngap_rrc_ue_adapter() { return ngap_rrc_ue_ev_notifier; }

  /// \brief Get the CU-CP to RRC UE adapter of the UE.
  cu_cp_rrc_ue_adapter& get_cu_cp_rrc_ue_adapter() { return cu_cp_rrc_ue_ev_notifier; }

  /// \brief Get the RRC to CU-CP adapter of the UE.
  rrc_ue_cu_cp_adapter& get_rrc_ue_cu_cp_adapter() { return rrc_ue_cu_cp_ev_notifier; }

private:
  // common context
  ue_index_t             ue_index = ue_index_t::invalid;
  ue_task_scheduler_impl task_sched;
  up_resource_manager    up_mng;
  ue_security_manager    sec_mng;

  // du ue context
  cu_cp_ue_context ue_ctxt;
  du_cell_index_t  pcell_index = du_cell_index_t::invalid;
  pci_t            pci         = INVALID_PCI;

  rrc_ue_cu_cp_ue_adapter                       rrc_ue_cu_cp_ue_ev_notifier;
  du_processor_rrc_ue_control_message_notifier* rrc_ue_notifier     = nullptr;
  du_processor_rrc_ue_srb_control_notifier*     rrc_ue_srb_notifier = nullptr;

  // ngap ue context
  ngap_cu_cp_ue_adapter ngap_cu_cp_ue_ev_notifier;
  ngap_rrc_ue_adapter   ngap_rrc_ue_ev_notifier;

  // cu-cp ue context
  cu_cp_rrc_ue_adapter         cu_cp_rrc_ue_ev_notifier;
  rrc_ue_cu_cp_adapter         rrc_ue_cu_cp_ev_notifier;
  cell_meas_manager_ue_context meas_context;
};

} // namespace srs_cu_cp
} // namespace srsran
