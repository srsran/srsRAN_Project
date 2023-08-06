/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/cu_cp/ue_manager.h"
#include <unordered_map>

namespace srsran {

namespace srs_cu_cp {

struct ngap_ue_context_t {
  amf_ue_id_t amf_ue_id                     = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id                     = ran_ue_id_t::invalid;
  uint64_t    aggregate_maximum_bit_rate_dl = 0;
};

class cu_cp_ue : public du_ue, public ngap_ue
{
public:
  cu_cp_ue(const ue_index_t              ue_index_,
           const up_resource_manager_cfg up_cfg,
           const pci_t                   pci_    = INVALID_PCI,
           const rnti_t                  c_rnti_ = rnti_t::INVALID_RNTI) :
    ue_index(ue_index_), up_mng(create_up_resource_manager(up_cfg))
  {
    if (pci_ != INVALID_PCI) {
      pci = pci_;
    }

    if (c_rnti_ != rnti_t::INVALID_RNTI) {
      c_rnti = c_rnti_;
    }

    du_index = get_du_index_from_ue_index(ue_index);
  }

  // generic_ue

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() override { return ue_index; }

  // du_ue

  /// \brief Get the UP resource manager of the UE.
  up_resource_manager& get_up_resource_manager() override { return *up_mng; }

  /// \brief Get the SRBs of the UE.
  std::map<srb_id_t, cu_srb_context>& get_srbs() override { return srbs; }

  /// \brief Get the task scheduler of the UE.
  rrc_ue_task_scheduler& get_task_sched() override { return *task_sched; }

  /// \brief Get the RRC UE control message notifier of the UE.
  du_processor_rrc_ue_control_message_notifier& get_rrc_ue_notifier() override { return *rrc_ue_notifier; }

  /// \brief Get the PCI of the UE.
  pci_t get_pci() override { return pci; };

  /// \brief Get the C-RNTI of the UE.
  rnti_t get_c_rnti() override { return c_rnti; }

  /// \brief Get the DU index of the UE.
  du_index_t get_du_index() override { return du_index; }

  /// \brief Get the PCell index of the UE.
  du_cell_index_t get_pcell_index() override { return pcell_index; }

  /// \brief Update a UE with PCI and/or C-RNTI.
  void update_du_ue(pci_t pci_ = INVALID_PCI, rnti_t c_rnti_ = INVALID_RNTI) override
  {
    if (pci_ != INVALID_PCI) {
      pci = pci_;
    }

    if (c_rnti_ != INVALID_RNTI) {
      c_rnti = c_rnti_;
    }
  }

  /// \brief Set the DU and PCell index of the UE.
  /// \param[in] pcell_index PCell index of the UE.
  void set_pcell_index(du_cell_index_t pcell_index_) override { pcell_index = pcell_index_; }

  /// \brief Set the task scheduler of the UE.
  /// \param[in] task_sched_ Task scheduler of the UE.
  void set_task_sched(rrc_ue_task_scheduler& task_sched_) override { task_sched = &task_sched_; }

  /// \brief Set the RRC UE control message notifier of the UE.
  /// \param[in] rrc_ue_notifier_ RRC UE control message notifier of the UE.
  void set_rrc_ue_notifier(du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_) override
  {
    rrc_ue_notifier = &rrc_ue_notifier_;
  }

  // ngap_ue

  /// \brief Get the RRC UE PDU notifier of the UE.
  ngap_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() override { return *rrc_ue_pdu_notifier; }

  /// \brief Get the RRC UE control notifier of the UE.
  ngap_rrc_ue_control_notifier& get_rrc_ue_control_notifier() override { return *rrc_ue_ctrl_notifier; }

  /// \brief Get the DU processor control notifier of the UE.
  ngap_du_processor_control_notifier& get_du_processor_control_notifier() override
  {
    return *du_processor_ctrl_notifier;
  }

  /// \brief Get the AMF UE ID of the UE.
  amf_ue_id_t get_amf_ue_id() override { return ngap_ue_context.amf_ue_id; }

  /// \brief Get the RAN UE ID of the UE.
  ran_ue_id_t get_ran_ue_id() override { return ngap_ue_context.ran_ue_id; }

  /// \brief Get the aggregate maximum bit rate DL of the UE.
  uint64_t get_aggregate_maximum_bit_rate_dl() override { return ngap_ue_context.aggregate_maximum_bit_rate_dl; }

  /// \brief Get the NGAP UE Context.
  ngap_ue_context_t get_ngap_ue_context() { return ngap_ue_context; }

  /// \brief Set the aggregate maximum bit rate DL of the UE.
  /// \param[in] aggregate_maximum_bit_rate_dl Aggregate maximum bit rate DL.
  void set_aggregate_maximum_bit_rate_dl(uint64_t aggregate_maximum_bit_rate_dl_) override
  {
    ngap_ue_context.aggregate_maximum_bit_rate_dl = aggregate_maximum_bit_rate_dl_;
  }

  bool du_ue_created   = false;
  bool ngap_ue_created = false;

  /// \brief Set the RAN UE ID of the UE.
  /// \param[in] ran_ue_id_ RAN UE ID of the UE.
  void set_ran_ue_id(ran_ue_id_t ran_ue_id_) { ngap_ue_context.ran_ue_id = ran_ue_id_; }

  /// \brief Set the RRC UE PDU notifier of the UE.
  /// \param[in] rrc_ue_pdu_notifier_ RRC UE PDU notifier for the UE.
  void set_rrc_ue_pdu_notifier(ngap_rrc_ue_pdu_notifier& rrc_ue_pdu_notifier_)
  {
    rrc_ue_pdu_notifier = &rrc_ue_pdu_notifier_;
  }

  /// \brief Set the RRC UE control notifier of the UE.
  /// \param[in] rrc_ue_ctrl_notifier_ RRC UE control notifier for the UE.
  void set_rrc_ue_ctrl_notifier(ngap_rrc_ue_control_notifier& rrc_ue_ctrl_notifier_)
  {
    rrc_ue_ctrl_notifier = &rrc_ue_ctrl_notifier_;
  }

  /// \brief Set the DU processor control notifier of the UE.
  /// \param[in] du_processor_ctrl_notifier_ DU processor control notifier for the UE.
  void set_du_processor_ctrl_notifier(ngap_du_processor_control_notifier& du_processor_ctrl_notifier_)
  {
    du_processor_ctrl_notifier = &du_processor_ctrl_notifier_;
  }

  /// \brief Set the AMF UE ID in the UE.
  /// \param[in] amf_ue_id The AMF UE ID to set.
  void set_amf_ue_id(amf_ue_id_t amf_ue_id_) { ngap_ue_context.amf_ue_id = amf_ue_id_; }

  /// \brief Set the NGAP UE Context.
  /// \param[in] ue_context The NGAP UE Context.
  void set_ngap_ue_context(ngap_ue_context_t ue_context) { ngap_ue_context = ue_context; }

private:
  // common context
  ue_index_t ue_index = ue_index_t::invalid;

  // du ue context
  std::unique_ptr<up_resource_manager> up_mng;

  du_index_t      du_index    = du_index_t::invalid;
  du_cell_index_t pcell_index = du_cell_index_t::invalid;
  pci_t           pci         = INVALID_PCI;
  rnti_t          c_rnti      = rnti_t::INVALID_RNTI;

  std::map<srb_id_t, cu_srb_context>            srbs;
  rrc_ue_task_scheduler*                        task_sched      = nullptr;
  du_processor_rrc_ue_control_message_notifier* rrc_ue_notifier = nullptr;

  // ngap ue context
  ngap_ue_context_t ngap_ue_context;

  ngap_rrc_ue_pdu_notifier*           rrc_ue_pdu_notifier        = nullptr;
  ngap_rrc_ue_control_notifier*       rrc_ue_ctrl_notifier       = nullptr;
  ngap_du_processor_control_notifier* du_processor_ctrl_notifier = nullptr;
};

class ue_manager : public du_processor_ue_manager, public ngap_ue_manager
{
public:
  explicit ue_manager(const ue_configuration& ue_config_, const up_resource_manager_cfg& up_config_);
  ~ue_manager() = default;

  // common

  /// \brief Get the UE index of the UE.
  /// \param[in] pci The PCI of the cell the UE is/was connected to.
  /// \param[in] c_rnti The RNTI of the UE.
  ue_index_t get_ue_index(pci_t pci, rnti_t c_rnti) override;

  /// \brief Get the CU-CP UE configuration stored in the UE manager.
  /// \return The CU-CP UE configuration.
  ue_configuration get_ue_config() override { return ue_config; }

  // du_processor_ue_manager

  /// \brief Allocate and return the UE index of a new UE.
  ue_index_t allocate_new_ue_index(du_index_t du_index) override;

  /// \brief Find the UE with the given UE index. Note that this will not check if a DU context exists.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the UE if found, nullptr otherwise.
  du_ue* find_ue(ue_index_t ue_index) override;

  /// \brief Add PCI and C-RNTI to a UE for the given UE index. If the UE can't be found or if a UE with the UE
  /// index was already setup, nulltpr is returned. \param[in] ue_index Index of the UE to add the notifiers to.
  /// \param[in] pci PCI of the cell that the UE is connected to.
  /// \param[in] rnti RNTI of the UE to be added.
  /// \return Pointer to the newly added DU UE if successful, nullptr otherwise.
  du_ue* add_ue(ue_index_t ue_index, pci_t pci, rnti_t rnti) override;

  /// \brief Remove the DU UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  void remove_du_ue(ue_index_t ue_index) override;

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
      if (ues.find(new_ue_index) != ues.end() && ues.at(new_ue_index).du_ue_created) {
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
  ngap_ue* add_ue(ue_index_t                          ue_index,
                  ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                  ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                  ngap_du_processor_control_notifier& du_processor_ctrl_notifier_) override;

  /// \brief Remove the NGAP UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  void remove_ngap_ue(ue_index_t ue_index) override;

  /// \brief Find the NGAP UE with the given UE index.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the NGAP UE if found, nullptr otherwise.
  ngap_ue* find_ngap_ue(ue_index_t ue_index) override;

  /// \brief Get the number of UEs connected to the AMF.
  /// \return Number of UEs.
  size_t get_nof_ngap_ues() override { return ran_ue_id_to_ue_index.size(); }

  /// \brief Get the UE index of the UE for the given RAN UE ID.
  /// \param[in] ran_ue_id RAN UE ID of the UE.
  /// \return Index of the UE if found, invalid index otherwise.
  ue_index_t get_ue_index(ran_ue_id_t ran_ue_id) override;

  /// \brief Get the UE index of the UE for the given AMF UE ID.
  /// \param[in] amf_ue_id AMF UE ID of the UE.
  /// \return Index of the UE if found, invalid index otherwise.
  ue_index_t get_ue_index(amf_ue_id_t amf_ue_id) override;

  /// \brief Set the AMF UE ID of the UE.
  /// \param[in] ue_index Index of the UE.
  /// \param[in] amf_ue_id The AMF UE ID for the UE.
  void set_amf_ue_id(ue_index_t ue_index, amf_ue_id_t amf_ue_id) override;

  /// \brief Transfer the NGAP UE context to a new UE e.g. in case of a reestablishment.
  /// \param[in] new_ue_index The index of the new UE.
  /// \param[in] old_ue_index The index of the old UE.
  void transfer_ngap_ue_context(ue_index_t new_ue_index, ue_index_t old_ue_index) override;

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index(du_index_t du_index);

  /// \brief Get the next available RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t get_next_ran_ue_id();

  /// \brief Find the RAN UE ID by a given UE index.
  /// \param[in] ue_index The UE index used to find the RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t find_ran_ue_id(ue_index_t ue_index);

  /// \brief Find the AMF UE ID by a given UE index.
  /// \param[in] ue_index The UE index used to find the AMF UE ID.
  /// \return The AMF UE ID.
  amf_ue_id_t find_amf_ue_id(ue_index_t ue_index);

  void clear_ue()
  {
    // TODO
  }

  srslog::basic_logger&         logger = srslog::fetch_basic_logger("CU-UEMNG");
  const ue_configuration        ue_config;
  const up_resource_manager_cfg up_config;

  std::unordered_map<ue_index_t, cu_cp_ue> ues; // ues indexed by ue_index

  // ue index lookups
  std::map<std::tuple<pci_t, rnti_t>, ue_index_t> pci_rnti_to_ue_index;  // ue_indexes indexed by pci and rnti
  std::unordered_map<ran_ue_id_t, ue_index_t>     ran_ue_id_to_ue_index; // ue_indexes indexed by ran_ue_ids
  std::unordered_map<amf_ue_id_t, ue_index_t>     amf_ue_id_to_ue_index; // ue_indexes indexed by amf_ue_ids
};

} // namespace srs_cu_cp
} // namespace srsran
