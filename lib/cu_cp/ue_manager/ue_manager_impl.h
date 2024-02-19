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
#include "ue_metrics_handler.h"
#include "ue_task_scheduler.h"
#include "srsran/cu_cp/ue_manager.h"
#include <unordered_map>

namespace srsran {

namespace srs_cu_cp {

struct ngap_ue_t {
  ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier;
  ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier;
  ngap_du_processor_control_notifier& du_processor_ctrl_notifier;

  ngap_ue_t(ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
            ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
            ngap_du_processor_control_notifier& du_processor_ctrl_notifier_) :
    rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_),
    rrc_ue_ctrl_notifier(rrc_ue_ctrl_notifier_),
    du_processor_ctrl_notifier(du_processor_ctrl_notifier_)
  {
  }
};

class cu_cp_ue final : public du_ue, public ngap_ue, public rrc_ue_task_scheduler
{
public:
  cu_cp_ue(const ue_index_t              ue_index_,
           const up_resource_manager_cfg up_cfg,
           ue_task_scheduler             task_sched_,
           const pci_t                   pci_    = INVALID_PCI,
           const rnti_t                  c_rnti_ = rnti_t::INVALID_RNTI) :
    ue_index(ue_index_), task_sched(std::move(task_sched_)), up_mng(create_up_resource_manager(up_cfg))
  {
    if (pci_ != INVALID_PCI) {
      pci = pci_;
    }

    if (c_rnti_ != rnti_t::INVALID_RNTI) {
      c_rnti = c_rnti_;
    }

    du_index = get_du_index_from_ue_index(ue_index);
  }

  /// \brief Cancel all pending UE tasks.
  void stop();

  // generic_ue

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() override { return ue_index; }

  /// \brief Get the UP resource manager of the UE.
  up_resource_manager& get_up_resource_manager() override { return *up_mng; }

  // rrc ue task scheduler.
  void          schedule_async_task(async_task<void> task) override { task_sched.schedule_async_task(std::move(task)); }
  unique_timer  make_unique_timer() override { return task_sched.create_timer(); }
  timer_factory get_timer_factory() override { return task_sched.get_timer_factory(); }

  // du_ue

  /// \brief Get the task scheduler of the UE.
  rrc_ue_task_scheduler& get_task_sched() override { return *this; }

  /// \brief Get the RRC UE control message notifier of the UE.
  du_processor_rrc_ue_control_message_notifier& get_rrc_ue_notifier() override
  {
    srsran_assert(rrc_ue_notifier != nullptr, "ue={}: RRC UE notifier was not set", ue_index);
    return *rrc_ue_notifier;
  }

  /// \brief Get the RRC UE SRB control notifier of the UE.
  du_processor_rrc_ue_srb_control_notifier& get_rrc_ue_srb_notifier() override
  {
    srsran_assert(rrc_ue_srb_notifier != nullptr, "ue={}: RRC UE SRB notifier was not set", ue_index);
    return *rrc_ue_srb_notifier;
  }

  rrc_ue_context_update_notifier& get_rrc_ue_context_update_notifier() override { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the RRC UE measurement notifier of the UE.
  rrc_ue_measurement_notifier& get_rrc_ue_measurement_notifier() override { return rrc_ue_cu_cp_ev_notifier; }

  /// \brief Get the PCI of the UE.
  pci_t get_pci() const override { return pci; };

  /// \brief Get the C-RNTI of the UE.
  rnti_t get_c_rnti() const override { return c_rnti; }

  gnb_du_id_t get_du_id() const { return du_id; }

  /// \brief Get the DU index of the UE.
  du_index_t get_du_index() override { return du_index; }

  /// \brief Get the PCell index of the UE.
  du_cell_index_t get_pcell_index() override { return pcell_index; }

  /// \brief Update a UE with PCI and/or C-RNTI.
  void update_du_ue(gnb_du_id_t du_id_  = gnb_du_id_t::invalid,
                    pci_t       pci_    = INVALID_PCI,
                    rnti_t      c_rnti_ = rnti_t::INVALID_RNTI) override
  {
    if (du_id_ != gnb_du_id_t::invalid) {
      du_id = du_id_;
    }

    if (pci_ != INVALID_PCI) {
      pci = pci_;
    }

    if (c_rnti_ != rnti_t::INVALID_RNTI) {
      c_rnti = c_rnti_;
    }
  }

  /// \brief Set the DU and PCell index of the UE.
  /// \param[in] pcell_index PCell index of the UE.
  void set_pcell_index(du_cell_index_t pcell_index_) override { pcell_index = pcell_index_; }

  /// \brief Set the RRC UE control message notifier of the UE.
  /// \param[in] rrc_ue_notifier_ RRC UE control message notifier of the UE.
  void set_rrc_ue_notifier(du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_) override
  {
    rrc_ue_notifier = &rrc_ue_notifier_;
  }

  /// \brief Set the RRC UE SRB notifier of the UE.
  /// \param[in] rrc_ue_srb_notifier_ RRC UE SRB control notifier of the UE.
  void set_rrc_ue_srb_notifier(du_processor_rrc_ue_srb_control_notifier& rrc_ue_srb_notifier_) override
  {
    rrc_ue_srb_notifier = &rrc_ue_srb_notifier_;
  }

  // ngap_ue

  /// \brief Get the RRC UE PDU notifier of the UE.
  ngap_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() override
  {
    srsran_assert(ngap_ue_context.has_value(), "ue={}: NGAP UE was not created", ue_index);
    return ngap_ue_context.value().rrc_ue_pdu_notifier;
  }

  /// \brief Get the RRC UE control notifier of the UE.
  ngap_rrc_ue_control_notifier& get_rrc_ue_control_notifier() override
  {
    srsran_assert(ngap_ue_context.has_value(), "ue={}: NGAP UE was not created", ue_index);
    return ngap_ue_context.value().rrc_ue_ctrl_notifier;
  }

  /// \brief Get the DU processor control notifier of the UE.
  ngap_du_processor_control_notifier& get_du_processor_control_notifier() override
  {
    srsran_assert(ngap_ue_context.has_value(), "ue={}: NGAP UE was not created", ue_index);
    return ngap_ue_context.value().du_processor_ctrl_notifier;
  }

  /// \brief Add the context for the NGAP UE.
  /// \param[in] rrc_ue_pdu_notifier The RRC UE PDU notifier for the UE.
  /// \param[in] rrc_ue_ctrl_notifier The RRC UE ctrl notifier for the UE.
  /// \param[in] du_processor_ctrl_notifier The DU processor ctrl notifier for the UE.
  void add_ngap_ue_context(ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                           ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                           ngap_du_processor_control_notifier& du_processor_ctrl_notifier)
  {
    ngap_ue_context.emplace(rrc_ue_pdu_notifier, rrc_ue_ctrl_notifier, du_processor_ctrl_notifier);
  }

  bool ngap_ue_created() { return ngap_ue_context.has_value(); }

  // cu-cp ue
  /// \brief Get the NGAP to RRC UE adapter of the UE.
  ngap_rrc_ue_adapter& get_ngap_rrc_ue_adapter() { return ngap_rrc_ue_ev_notifier; }

  /// \brief Get the CU-CP to RRC UE adapter of the UE.
  cu_cp_rrc_ue_adapter& get_cu_cp_rrc_ue_adapter() { return cu_cp_rrc_ue_ev_notifier; }

  /// \brief Get the RRC to CU-CP adapter of the UE.
  rrc_ue_cu_cp_adapter& get_rrc_ue_cu_cp_adapter() { return rrc_ue_cu_cp_ev_notifier; }

private:
  // common context
  ue_index_t                           ue_index = ue_index_t::invalid;
  ue_task_scheduler                    task_sched;
  std::unique_ptr<up_resource_manager> up_mng;

  // du ue context
  du_index_t      du_index    = du_index_t::invalid;
  gnb_du_id_t     du_id       = gnb_du_id_t::invalid;
  du_cell_index_t pcell_index = du_cell_index_t::invalid;
  pci_t           pci         = INVALID_PCI;
  rnti_t          c_rnti      = rnti_t::INVALID_RNTI;

  du_processor_rrc_ue_control_message_notifier* rrc_ue_notifier     = nullptr;
  du_processor_rrc_ue_srb_control_notifier*     rrc_ue_srb_notifier = nullptr;

  // ngap ue context
  optional<ngap_ue_t> ngap_ue_context;

  // cu-cp ue context
  ngap_rrc_ue_adapter  ngap_rrc_ue_ev_notifier;
  cu_cp_rrc_ue_adapter cu_cp_rrc_ue_ev_notifier;
  rrc_ue_cu_cp_adapter rrc_ue_cu_cp_ev_notifier;
};

class ue_manager : public du_processor_ue_manager, public ngap_ue_manager, public ue_metrics_handler
{
public:
  explicit ue_manager(const ue_configuration&        ue_config_,
                      const up_resource_manager_cfg& up_config_,
                      timer_manager&                 timers,
                      task_executor&                 cu_cp_exec);

  /// Stop UE activity.
  void stop();

  // common

  /// \brief Get the CU-CP UE configuration stored in the UE manager.
  /// \return The CU-CP UE configuration.
  ue_configuration get_ue_config() override { return ue_config; }

  /// \brief Get the UE index of the UE.
  /// \param[in] pci The PCI of the cell the UE is/was connected to.
  /// \param[in] c_rnti The RNTI of the UE.
  ue_index_t get_ue_index(pci_t pci, rnti_t c_rnti) override;

  /// \brief Remove the UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  void remove_ue(ue_index_t ue_index) override;

  /// \brief Get the number of UEs.
  /// \return Number of UEs.
  size_t get_nof_ues() const override { return ues.size(); }

  // du_processor_ue_manager

  /// \brief Allocate resources for the UE in the CU-CP.
  ///
  /// \return ue_index of the created UE or ue_index_t::invalid in case of failure.
  ue_index_t add_ue(du_index_t du_index) override;

  /// \brief Find the UE with the given UE index. Note that this will not check if a DU context exists.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the UE if found, nullptr otherwise.
  du_ue* find_ue(ue_index_t ue_index) override;

  du_ue* set_ue_du_context(ue_index_t ue_index, gnb_du_id_t du_id, pci_t pci, rnti_t rnti) override;

  /// \brief Find the UE with the given UE index, thats DU context is set up.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the DU UE if found, nullptr otherwise.
  du_ue* find_du_ue(ue_index_t ue_index) override;

  /// \brief Get the number of UEs connected to a specific DU.
  /// \return Number of UEs.
  size_t get_nof_du_ues(du_index_t du_index) override
  {
    unsigned ue_count = 0;
    // Search allocated UE indexes
    for (uint16_t i = 0; i < MAX_NOF_UES_PER_DU; i++) {
      ue_index_t new_ue_index = generate_ue_index(du_index, i);
      if (ues.find(new_ue_index) != ues.end()) {
        ue_count++;
      }
    }
    return ue_count;
  }

  // ngap_ue_manager

  /// \brief Add notifier to a UE for the given UE index. A RAN UE ID is allocated internally. If a new UE can't be
  /// found or if a UE with the UE index was already setup, nulltpr is returned.
  /// \param[in] ue_index Index of the UE to add the notifiers to.
  /// \param[in] rrc_ue_pdu_notifier RRC UE PDU notifier for the UE.
  /// \param[in] rrc_ue_ctrl_notifier RRC UE control notifier for the UE.
  /// \param[in] du_processor_ctrl_notifier DU processor control notifier for the UE.
  /// \return Pointer to the NGAP UE if found, nullptr otherwise.
  ngap_ue* set_ue_ng_context(ue_index_t                          ue_index,
                             ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                             ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                             ngap_du_processor_control_notifier& du_processor_ctrl_notifier_) override;

  /// \brief Find the NGAP UE with the given UE index.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the NGAP UE if found, nullptr otherwise.
  ngap_ue* find_ngap_ue(ue_index_t ue_index) override;

  /// \brief Get the number of UEs connected to the AMF.
  /// \return Number of UEs.
  size_t get_nof_ngap_ues() override
  {
    unsigned ue_count = 0;
    // Search allocated UE indexes
    for (auto& ue : ues) {
      if (ue.second.ngap_ue_created()) {
        ue_count++;
      }
    }
    return ue_count;
  }

  // cu-cp ue manager
  /// \brief Get the NGAP to RRC UE adapter of the UE.
  ngap_rrc_ue_adapter& get_ngap_rrc_ue_adapter(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ues.find(ue_index) != ues.end(), "UE with ue_index={} does not exist", ue_index);

    return ues.at(ue_index).get_ngap_rrc_ue_adapter();
  }

  /// \brief Get the CU-CP to RRC UE adapter of the UE.
  cu_cp_rrc_ue_adapter& get_cu_cp_rrc_ue_adapter(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ues.find(ue_index) != ues.end(), "UE with ue_index={} does not exist", ue_index);

    return ues.at(ue_index).get_cu_cp_rrc_ue_adapter();
  }

  rrc_ue_cu_cp_adapter& get_rrc_ue_cu_cp_adapter(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ues.find(ue_index) != ues.end(), "UE with ue_index={} does not exist", ue_index);

    return ues.at(ue_index).get_rrc_ue_cu_cp_adapter();
  }

  std::vector<metrics_report::ue_info> handle_ue_metrics_report_request() const override;

  ue_task_scheduler_manager& get_task_sched() { return ue_task_scheds; }

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index(du_index_t du_index);

  srslog::basic_logger&         logger = srslog::fetch_basic_logger("CU-UEMNG");
  const ue_configuration        ue_config;
  const up_resource_manager_cfg up_config;

  // Manager of UE task schedulers.
  ue_task_scheduler_manager ue_task_scheds;

  // Container of UE contexts handled by the CU-CP.
  std::unordered_map<ue_index_t, cu_cp_ue> ues;

  // ue index lookups
  std::map<std::tuple<pci_t, rnti_t>, ue_index_t> pci_rnti_to_ue_index; // ue_indexes indexed by pci and rnti
};

} // namespace srs_cu_cp
} // namespace srsran
