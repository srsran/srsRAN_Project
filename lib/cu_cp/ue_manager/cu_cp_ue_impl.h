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

#include "../adapters/cu_cp_adapters.h"
#include "../adapters/ngap_adapters.h"
#include "../adapters/rrc_ue_adapters.h"
#include "../cell_meas_manager/measurement_context.h"
#include "../up_resource_manager/up_resource_manager_impl.h"
#include "cu_cp_ue_impl_interface.h"
#include "ue_task_scheduler_impl.h"
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
  cu_cp_ue(const ue_index_t               ue_index_,
           const up_resource_manager_cfg& up_cfg,
           ue_task_scheduler_impl         task_sched_,
           const pci_t                    pci_    = INVALID_PCI,
           const rnti_t                   c_rnti_ = rnti_t::INVALID_RNTI);

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
  du_index_t get_du_index() { return ue_ctxt.du_idx; }

  /// \brief Get the PCell index of the UE.
  du_cell_index_t get_pcell_index() { return pcell_index; }

  /// \brief Get the UP resource manager of the UE.
  up_resource_manager& get_up_resource_manager() override { return up_mng; }

  /// \brief Get the task scheduler of the UE.
  ue_task_scheduler& get_task_sched() override { return task_sched; }

  cu_cp_ue_context&                     get_ue_context() { return ue_ctxt; }
  [[nodiscard]] const cu_cp_ue_context& get_ue_context() const { return ue_ctxt; }

  /// \brief Get the security context of the UE.
  security::security_context& get_security_context() override { return sec_context; }

  /// \brief Get the measurement context of the UE.
  cell_meas_manager_ue_context& get_meas_context() { return meas_context; }

  /// \brief Update a UE with PCI and/or C-RNTI.
  void update_du_ue(gnb_du_id_t du_id_  = gnb_du_id_t::invalid,
                    pci_t       pci_    = INVALID_PCI,
                    rnti_t      c_rnti_ = rnti_t::INVALID_RNTI);

  /// \brief Set/update the measurement context of the UE.
  void update_meas_context(cell_meas_manager_ue_context meas_ctxt);

  /// \brief Check if the DU UE context is created.
  /// \return True if the DU UE context is created, false otherwise.
  [[nodiscard]] bool du_ue_created() const { return ue_ctxt.du_idx != du_index_t::invalid; }

  /// \brief Set the DU and PCell index of the UE.
  /// \param[in] pcell_index PCell index of the UE.
  void set_pcell_index(du_cell_index_t pcell_index_);

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

  ngap_cu_cp_ue_notifier& get_ngap_cu_cp_ue_notifier() { return ue_adapter; }

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
  ue_index_t                 ue_index = ue_index_t::invalid;
  ue_task_scheduler_impl     task_sched;
  security::security_context sec_context;
  up_resource_manager        up_mng;

  // du ue context
  cu_cp_ue_context ue_ctxt;
  du_cell_index_t  pcell_index = du_cell_index_t::invalid;
  pci_t            pci         = INVALID_PCI;

  du_processor_rrc_ue_control_message_notifier* rrc_ue_notifier     = nullptr;
  du_processor_rrc_ue_srb_control_notifier*     rrc_ue_srb_notifier = nullptr;

  // ngap ue context
  ngap_cu_cp_ue_adapter ue_adapter;
  ngap_rrc_ue_adapter   ngap_rrc_ue_ev_notifier;

  // cu-cp ue context
  cu_cp_rrc_ue_adapter         cu_cp_rrc_ue_ev_notifier;
  rrc_ue_cu_cp_adapter         rrc_ue_cu_cp_ev_notifier;
  cell_meas_manager_ue_context meas_context;
};

} // namespace srs_cu_cp
} // namespace srsran
