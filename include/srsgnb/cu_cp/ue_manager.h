/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/rrc/rrc_ue.h"

namespace srsgnb {
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

  /// \brief Get the SRBs of the UE.
  virtual std::map<srb_id_t, cu_srb_context>& get_srbs() = 0;

  /// \brief Get the task scheduler of the UE.
  virtual rrc_ue_task_scheduler& get_task_sched() = 0;

  /// \brief Get the RRC UE control message notifier of the UE.
  virtual du_processor_rrc_ue_control_message_notifier& get_rrc_ue_notifier() = 0;

  /// \brief Get the C-RNTI of the UE.
  virtual rnti_t get_c_rnti() = 0;

  /// \brief Get the PCell index of the UE.
  virtual du_cell_index_t get_pcell_index() = 0;

  /// \brief Set the PCell index of the UE.
  /// \param[in] pcell_index PCell index of the UE.
  virtual void set_pcell_index(du_cell_index_t pcell_index) = 0;

  /// \brief Set the task scheduler of the UE.
  /// \param[in] task_sched_ Task scheduler of the UE.
  virtual void set_task_sched(rrc_ue_task_scheduler& task_sched_) = 0;

  /// \brief Set the RRC UE control message notifier of the UE.
  /// \param[in] rrc_ue_notifier_ RRC UE control message notifier of the UE.
  virtual void set_rrc_ue_notifier(du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_) = 0;
};

/// DU Processor UE manager interface.
class du_processor_ue_manager
{
public:
  virtual ~du_processor_ue_manager() = default;

  /// \brief Allocate new UE context for the given RNTI. A UE index is allocated internally. If a new UE can't be
  /// allocated or if a UE with the same RNTI already exists, nulltpr is returned.
  /// \param[in] du_index Index of the DU.
  /// \param[in] rnti RNTI of the UE to be added.
  /// \return Pointer to the newly added DU UE if successful, nullptr otherwise.
  virtual du_ue* add_ue(du_index_t du_index, rnti_t rnti) = 0;

  /// \brief Remove the DU UE context with the given UE index.
  /// \param[in] ue_index Index of the UE to be removed.
  virtual void remove_du_ue(ue_index_t ue_index) = 0;

  /// \brief Find the DU UE with the given UE index.
  /// \param[in] ue_index Index of the UE to be found.
  /// \return Pointer to the DU UE if found, nullptr otherwise.
  virtual du_ue* find_du_ue(ue_index_t ue_index) = 0;

  /// \brief Find the index the DU UE with the given RNTI.
  /// \param[in] rnti RNTI of the UE to be found.
  /// \return Index of the DU UE if found, invalid index otherwise.
  virtual ue_index_t get_ue_index(rnti_t rnti) = 0;

  /// \brief Get the number of UEs connected to the DU.
  /// \return Number of UEs.
  virtual size_t get_nof_du_ues() = 0;

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
  virtual ngc_rrc_ue_pdu_notifier& get_rrc_ue_pdu_notifier() = 0;

  /// \brief Get the RRC UE control notifier of the UE.
  virtual ngc_rrc_ue_control_notifier& get_rrc_ue_control_notifier() = 0;

  /// \brief Get the DU processor control notifier of the UE.
  virtual ngc_du_processor_control_notifier& get_du_processor_control_notifier() = 0;

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
class ngap_ue_manager
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
  virtual ngap_ue* add_ue(ue_index_t                         ue_index,
                          ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                          ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                          ngc_du_processor_control_notifier& du_processor_ctrl_notifier) = 0;

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
};

} // namespace srs_cu_cp
} // namespace srsgnb
