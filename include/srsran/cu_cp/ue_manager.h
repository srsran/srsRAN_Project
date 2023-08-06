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

#include "srsran/cu_cp/du_processor.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/rrc/rrc_ue.h"

namespace srsran {
namespace srs_cu_cp {

/// Common UE interface.
class ue_base
{
public:
  virtual ~ue_base() = default;

  /// \brief Get the UE index of the UE.
  virtual ue_index_t get_ue_index() = 0;
};

/// Interface for DU UE.
class du_ue : public ue_base
{
public:
  virtual ~du_ue() = default;

  /// \brief Get the UP resource manager of the UE.
  virtual up_resource_manager& get_up_resource_manager() = 0;

  /// \brief Get the SRBs of the UE.
  virtual std::map<srb_id_t, cu_srb_context>& get_srbs() = 0;

  /// \brief Get the task scheduler of the UE.
  virtual rrc_ue_task_scheduler& get_task_sched() = 0;

  /// \brief Get the RRC UE control message notifier of the UE.
  virtual du_processor_rrc_ue_control_message_notifier& get_rrc_ue_notifier() = 0;

  /// \brief Get the PCI of the UE.
  virtual pci_t get_pci() = 0;

  /// \brief Get the C-RNTI of the UE.
  virtual rnti_t get_c_rnti() = 0;

  /// \brief Get the DU index of the UE.
  virtual du_index_t get_du_index() = 0;

  /// \brief Get the PCell index of the UE.
  virtual du_cell_index_t get_pcell_index() = 0;

  /// \brief Update a UE with PCI and/or C-RNTI.
  virtual void update_du_ue(pci_t pci_, rnti_t c_rnti_) = 0;

  /// \brief Set the PCell infox of the UE.
  /// \param[in] pcell_index PCell index of the UE.
  virtual void set_pcell_index(du_cell_index_t pcell_index) = 0;

  /// \brief Set the task scheduler of the UE.
  /// \param[in] task_sched_ Task scheduler of the UE.
  virtual void set_task_sched(rrc_ue_task_scheduler& task_sched_) = 0;

  /// \brief Set the RRC UE control message notifier of the UE.
  /// \param[in] rrc_ue_notifier_ RRC UE control message notifier of the UE.
  virtual void set_rrc_ue_notifier(du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_) = 0;
};

/// UE configuration passed to CU-CP
struct ue_configuration {
  std::chrono::seconds inactivity_timer;
};

/// Common UE manager interface.
class common_ue_manager
{
public:
  virtual ~common_ue_manager() = default;

  /// \brief Get the CU-CP UE configuration stored in the UE manager.
  /// \return The CU-CP UE configuration.
  virtual ue_configuration get_ue_config() = 0;

  /// \brief Get the UE index of a UE by a given pci and rnti.
  /// \param[in] pci The PCI of the cell the UE is/was connected to.
  /// \param[in] c_rnti The RNTI of the UE.
  virtual ue_index_t get_ue_index(pci_t pci, rnti_t c_rnti) = 0;
};

/// DU Processor UE manager interface.
class du_processor_ue_manager : public common_ue_manager
{
public:
  virtual ~du_processor_ue_manager() = default;

  /// \brief Allocate and return the UE index of a new UE.
  virtual ue_index_t allocate_new_ue_index(du_index_t du_index) = 0;

  /// \brief Find the UE with the given UE index. Note that this will not check if a DU context exists.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the UE if found, nullptr otherwise.
  virtual du_ue* find_ue(ue_index_t ue_index) = 0;

  /// \brief Add PCI and C-RNTI to a UE for the given UE index. If the UE can't be found or if a UE with the UE index
  /// was already setup, nulltpr is returned.
  /// \param[in] ue_index Index of the UE to add the notifiers to.
  /// \param[in] pci PCI of the cell that the UE is connected to.
  /// \param[in] rnti RNTI of the UE to be added.
  /// \return Pointer to the newly added DU UE if successful, nullptr otherwise.
  virtual du_ue* add_ue(ue_index_t ue_index, pci_t pci, rnti_t rnti) = 0;

  /// \brief Remove the DU UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  virtual void remove_du_ue(ue_index_t ue_index) = 0;

  /// \brief Find the DU UE with the given UE index.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the DU UE if found, nullptr otherwise.
  virtual du_ue* find_du_ue(ue_index_t ue_index) = 0;

  /// \brief Get the number of UEs connected to a specific DU.
  /// \return Number of UEs.
  virtual size_t get_nof_du_ues(du_index_t du_index) = 0;
};

/// Interface for NGAP UE.
class ngap_ue : public ue_base
{
public:
  virtual ~ngap_ue() = default;

  /// \brief Get the RRC UE PDU notifier of the UE.
  virtual ngap_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() = 0;

  /// \brief Get the RRC UE control notifier of the UE.
  virtual ngap_rrc_ue_control_notifier& get_rrc_ue_control_notifier() = 0;

  /// \brief Get the DU processor control notifier of the UE.
  virtual ngap_du_processor_control_notifier& get_du_processor_control_notifier() = 0;

  /// \brief Get the AMF UE ID of the UE.
  virtual amf_ue_id_t get_amf_ue_id() = 0;

  /// \brief Get the RAN UE ID of the UE.
  virtual ran_ue_id_t get_ran_ue_id() = 0;

  /// \brief Get the aggregate maximum bit rate DL of the UE.
  virtual uint64_t get_aggregate_maximum_bit_rate_dl() = 0;

  /// \brief Set the aggregate maximum bit rate DL of the UE.
  /// \param[in] aggregate_maximum_bit_rate_dl Aggregate maximum bit rate DL.
  virtual void set_aggregate_maximum_bit_rate_dl(uint64_t aggregate_maximum_bit_rate_dl) = 0;
};

/// NGAP UE manager interface.
class ngap_ue_manager : public common_ue_manager
{
public:
  virtual ~ngap_ue_manager() = default;

  /// \brief Add notifier to a UE for the given UE index. A RAN UE ID is allocated internally. If a new UE can't be
  /// found or if a UE with the UE index was already setup, nulltpr is returned.
  /// \param[in] ue_index Index of the UE to add the notifiers to.
  /// \param[in] rrc_ue_pdu_notifier RRC UE PDU notifier for the UE.
  /// \param[in] rrc_ue_ctrl_notifier RRC UE control notifier for the UE.
  /// \param[in] du_processor_ctrl_notifier DU processor control notifier for the UE.
  /// \return Pointer to the NGAP UE if found, nullptr otherwise.
  virtual ngap_ue* add_ue(ue_index_t                          ue_index,
                          ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                          ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                          ngap_du_processor_control_notifier& du_processor_ctrl_notifier) = 0;

  /// \brief Remove the NGAP UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  virtual void remove_ngap_ue(ue_index_t ue_index) = 0;

  /// \brief Find the NGAP UE with the given UE index.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the NGAP UE if found, nullptr otherwise.
  virtual ngap_ue* find_ngap_ue(ue_index_t ue_index) = 0;

  /// \brief Get the number of UEs connected to the AMF.
  /// \return Number of UEs.
  virtual size_t get_nof_ngap_ues() = 0;

  // Hide -Woverloaded-virtual warning by indicating to the compiler that we want all get_ue_index functions
  using common_ue_manager::get_ue_index;

  /// \brief Get the UE index of the UE for the given RAN UE ID.
  /// \param[in] ran_ue_id RAN UE ID of the UE.
  /// \return Index of the UE if found, invalid index otherwise.
  virtual ue_index_t get_ue_index(ran_ue_id_t ran_ue_id) = 0;

  /// \brief Get the UE index of the UE for the given AMF UE ID.
  /// \param[in] amf_ue_id AMF UE ID of the UE.
  /// \return Index of the UE if found, invalid index otherwise.
  virtual ue_index_t get_ue_index(amf_ue_id_t amf_ue_id) = 0;

  /// \brief Set the AMF UE ID of the UE.
  /// \param[in] ue_index Index of the UE.
  /// \param[in] amf_ue_id The AMF UE ID for the UE.
  virtual void set_amf_ue_id(ue_index_t ue_index, amf_ue_id_t amf_ue_id) = 0;

  /// \brief Transfer the NGAP UE context to a new UE e.g. in case of a reestablishment.
  /// \param[in] new_ue_index The index of the new UE.
  /// \param[in] old_ue_index The index of the old UE.
  virtual void transfer_ngap_ue_context(ue_index_t new_ue_index, ue_index_t old_ue_index) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
