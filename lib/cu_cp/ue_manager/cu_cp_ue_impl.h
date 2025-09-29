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

#include "../adapters/ngap_adapters.h"
#include "../adapters/nrppa_adapters.h"
#include "../adapters/rrc_ue_adapters.h"
#include "../cell_meas_manager/measurement_context.h"
#include "../ue_security_manager/ue_security_manager_impl.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "cu_cp_ue_impl_interface.h"
#include "ue_task_scheduler_impl.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/plmn_identity.h"
#include <optional>
#include <unordered_map>

namespace srsran {

namespace srs_cu_cp {

/// \brief Context of a CU-CP UE.
struct cu_cp_ue_context {
  du_index_t    du_idx   = du_index_t::invalid;
  plmn_identity plmn     = plmn_identity::test_value();
  gnb_du_id_t   du_id    = gnb_du_id_t::invalid;
  ue_index_t    ue_index = ue_index_t::invalid;
  rnti_t        crnti    = rnti_t::INVALID_RNTI;
  /// \brief Flag to disable new UE reconfigurations. This can be used, for instance, to reconfigure UE contexts
  /// that are in the process of handover.
  bool reconfiguration_disabled = false;
};

struct cu_cp_ue_handover_context {
  ue_index_t target_ue_index = ue_index_t::invalid;
  uint8_t    rrc_reconfig_transaction_id;
};

class cu_cp_ue : public cu_cp_ue_impl_interface
{
public:
  cu_cp_ue(ue_index_t                     ue_index_,
           du_index_t                     du_index_,
           timer_manager&                 timers_,
           task_executor&                 task_exec_,
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
  ue_index_t get_ue_index() const override { return ue_index; }

  /// \brief Get the PCI of the UE.
  [[nodiscard]] pci_t get_pci() const { return pci; }

  /// \brief Get the C-RNTI of the UE.
  [[nodiscard]] rnti_t get_c_rnti() const { return ue_ctxt.crnti; }

  [[nodiscard]] gnb_du_id_t get_du_id() const { return ue_ctxt.du_id; }

  /// \brief Get the DU index of the UE.
  [[nodiscard]] du_index_t get_du_index() const override { return ue_ctxt.du_idx; }

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

  /// \brief Set the RRC UE of the UE.
  void set_rrc_ue(rrc_ue_interface& rrc_ue_);

  /// \brief Get the RRC UE notifier of the UE.
  ngap_rrc_ue_notifier& get_ngap_rrc_ue_notifier() override;

  /// \brief Get the NGAP CU-CP UE notifier of the UE.
  ngap_cu_cp_ue_notifier& get_ngap_cu_cp_ue_notifier() { return ngap_cu_cp_ue_ev_notifier; }

  /// \brief Get the NRPPA CU-CP UE notifier of the UE.
  nrppa_cu_cp_ue_notifier& get_nrppa_cu_cp_ue_notifier() { return nrppa_cu_cp_ue_ev_notifier; }

  /// \brief Get the RRC UE CU-CP UE notifier of the UE.
  rrc_ue_cu_cp_ue_notifier& get_rrc_ue_cu_cp_ue_notifier() { return rrc_ue_cu_cp_ue_ev_notifier; }

  rrc_ue_context_update_notifier& get_rrc_ue_context_update_notifier() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the RRC UE measurement notifier of the UE.
  rrc_ue_measurement_notifier& get_rrc_ue_measurement_notifier() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the NGAP to RRC UE adapter of the UE.
  ngap_rrc_ue_adapter& get_ngap_rrc_ue_adapter() { return ngap_rrc_ue_ev_notifier; }

  /// \brief Get the RRC UE to NGAP adapter of the UE.
  rrc_ue_ngap_adapter& get_rrc_ue_ngap_adapter() { return rrc_ue_ngap_ev_notifier; }

  /// \brief Get the RRC to CU-CP adapter of the UE.
  rrc_ue_cu_cp_adapter& get_rrc_ue_cu_cp_adapter() { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the RRC UE of the UE.
  rrc_ue_interface* get_rrc_ue() const { return rrc_ue; }

  /// \brief Get the measurement results of the UE.
  std::optional<cell_measurement_positioning_info>& get_measurement_results() override
  {
    return meas_context.meas_results;
  }

  unique_timer& get_handover_ue_release_timer() { return handover_ue_release_timer; }

  std::optional<cu_cp_ue_handover_context>& get_ho_context() { return ho_context; }

private:
  // Common context.
  ue_index_t             ue_index = ue_index_t::invalid;
  ue_task_scheduler_impl task_sched;
  up_resource_manager    up_mng;
  ue_security_manager    sec_mng;

  // DU UE context.
  cu_cp_ue_context ue_ctxt;
  du_cell_index_t  pcell_index = du_cell_index_t::invalid;
  pci_t            pci         = INVALID_PCI;

  rrc_ue_cu_cp_ue_adapter rrc_ue_cu_cp_ue_ev_notifier;

  // RRC UE context.
  rrc_ue_interface*   rrc_ue = nullptr;
  rrc_ue_ngap_adapter rrc_ue_ngap_ev_notifier;

  // NGAP UE context.
  ngap_cu_cp_ue_adapter ngap_cu_cp_ue_ev_notifier;
  ngap_rrc_ue_adapter   ngap_rrc_ue_ev_notifier;

  // NRPPA UE context.
  nrppa_cu_cp_ue_adapter nrppa_cu_cp_ue_ev_notifier;

  // CU-CP UE context.
  rrc_ue_cu_cp_adapter                     rrc_ue_cu_cp_ev_notifier;
  cell_meas_manager_ue_context             meas_context;
  unique_timer                             handover_ue_release_timer = {};
  std::optional<cu_cp_ue_handover_context> ho_context;
};

} // namespace srs_cu_cp
} // namespace srsran
