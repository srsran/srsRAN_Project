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

#include "adapters/du_processor_adapters.h"
#include "srsran/cu_cp/ue_manager.h"
#include <unordered_map>

namespace srsran {

namespace srs_cu_cp {

class cu_cp_ue : public du_ue, public ngap_ue
{
public:
  cu_cp_ue(const ue_index_t ue_index_, const rnti_t c_rnti_) : ue_index(ue_index_), c_rnti(c_rnti_) {}

  // generic_ue

  /// \brief Get the UE index of the UE.
  ue_index_t get_ue_index() override { return ue_index; }

  // du_ue

  /// \brief Get the SRBs of the UE.
  std::map<srb_id_t, cu_srb_context>& get_srbs() override { return srbs; }

  /// \brief Get the task scheduler of the UE.
  rrc_ue_task_scheduler& get_task_sched() override { return *task_sched; }

  /// \brief Get the RRC UE control message notifier of the UE.
  du_processor_rrc_ue_control_message_notifier& get_rrc_ue_notifier() override { return *rrc_ue_notifier; }

  /// \brief Get the C-RNTI of the UE.
  rnti_t get_c_rnti() override { return c_rnti; }

  /// \brief Get the PCell index of the UE.
  du_cell_index_t get_pcell_index() override { return pcell_index; }

  /// \brief Set the PCell index of the UE.
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
  amf_ue_id_t get_amf_ue_id() override { return amf_ue_id; }

  /// \brief Get the RAN UE ID of the UE.
  ran_ue_id_t get_ran_ue_id() override { return ran_ue_id; }

  /// \brief Get the aggregate maximum bit rate DL of the UE.
  uint64_t get_aggregate_maximum_bit_rate_dl() override { return aggregate_maximum_bit_rate_dl; }

  /// \brief Set the aggregate maximum bit rate DL of the UE.
  /// \param[in] aggregate_maximum_bit_rate_dl Aggregate maximum bit rate DL.
  void set_aggregate_maximum_bit_rate_dl(uint64_t aggregate_maximum_bit_rate_dl_) override
  {
    aggregate_maximum_bit_rate_dl = aggregate_maximum_bit_rate_dl_;
  }

  bool du_ue_created   = false;
  bool ngap_ue_created = false;

  /// \brief Set the RAN UE ID of the UE.
  /// \param[in] ran_ue_id_ RAN UE ID of the UE.
  void set_ran_ue_id(ran_ue_id_t ran_ue_id_) { ran_ue_id = ran_ue_id_; }

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
  void set_amf_ue_id(amf_ue_id_t amf_ue_id_) { amf_ue_id = amf_ue_id_; }

private:
  // common context
  ue_index_t ue_index = ue_index_t::invalid;

  // du ue context
  du_cell_index_t pcell_index = du_cell_index_t::invalid;
  rnti_t          c_rnti      = INVALID_RNTI;

  std::map<srb_id_t, cu_srb_context>            srbs;
  rrc_ue_task_scheduler*                        task_sched      = nullptr;
  du_processor_rrc_ue_control_message_notifier* rrc_ue_notifier = nullptr;

  // ngap ue context
  amf_ue_id_t amf_ue_id                     = amf_ue_id_t::invalid;
  ran_ue_id_t ran_ue_id                     = ran_ue_id_t::invalid;
  uint64_t    aggregate_maximum_bit_rate_dl = 0;

  ngap_rrc_ue_pdu_notifier*           rrc_ue_pdu_notifier        = nullptr;
  ngap_rrc_ue_control_notifier*       rrc_ue_ctrl_notifier       = nullptr;
  ngap_du_processor_control_notifier* du_processor_ctrl_notifier = nullptr;
};

class ue_manager : public du_processor_ue_manager, public ngap_ue_manager
{
public:
  explicit ue_manager();

  // du_processor_ue_manager

  /// \brief Allocate new UE context for the given RNTI. A UE index is allocated internally. If a new UE can't be
  /// allocated or if a UE with the same RNTI already exists, nulltpr is returned.
  /// \param[in] du_index Index of the DU.
  /// \param[in] rnti RNTI of the UE to be added.
  /// \return Pointer to the newly added DU UE if successful, nullptr otherwise.
  du_ue* add_ue(du_index_t du_index, rnti_t rnti) override;

  /// \brief Remove the DU UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  void remove_du_ue(ue_index_t ue_index) override;

  /// \brief Find the DU UE with the given UE index.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the DU UE if found, nullptr otherwise.
  du_ue* find_du_ue(ue_index_t ue_index) override;

  /// \brief Find the index the DU UE with the given RNTI.
  /// \param[in] rnti RNTI of the UE to be found.
  /// \return Index of the DU UE if found, invalid index otherwise.
  ue_index_t get_ue_index(rnti_t rnti) override;

  /// \brief Get the number of UEs connected to DUs.
  /// \return Number of UEs.
  size_t get_nof_du_ues() override { return rnti_to_ue_index.size(); }

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

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index(du_index_t du_index);

  /// \brief Get the next available RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t get_next_ran_ue_id();

  /// \brief Find the RAN UE ID by a given UE NGAP ID.
  /// \param[in] ue_index The UE NGAP ID used to find the RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t find_ran_ue_id(ue_index_t ue_index);

  void clear_ue()
  {
    // TODO
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("UE-MNG");

  std::unordered_map<ue_index_t, cu_cp_ue> ues; // ues indexed by ue_index

  // ue index lookups
  std::unordered_map<rnti_t, ue_index_t>      rnti_to_ue_index;      // ue_indexes indexed by rntis
  std::unordered_map<ran_ue_id_t, ue_index_t> ran_ue_id_to_ue_index; // ue_indexes indexed by ran_ue_ids
  std::unordered_map<amf_ue_id_t, ue_index_t> amf_ue_id_to_ue_index; // ue_indexes indexed by amf_ue_ids
};

} // namespace srs_cu_cp
} // namespace srsran
