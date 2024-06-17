/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_ue_impl.h"

using namespace srsran;
using namespace srs_cu_cp;

cu_cp_ue::cu_cp_ue(const ue_index_t               ue_index_,
                   const up_resource_manager_cfg& up_cfg,
                   ue_task_scheduler_impl         task_sched_,
                   const pci_t                    pci_,
                   const rnti_t                   c_rnti_) :
  ue_index(ue_index_),
  task_sched(std::move(task_sched_)),
  up_mng(create_up_resource_manager(up_cfg)),
  rrc_ue_cu_cp_ev_notifier(ue_index)
{
  if (pci_ != INVALID_PCI) {
    pci = pci_;
  }

  if (c_rnti_ != rnti_t::INVALID_RNTI) {
    ue_ctxt.crnti = c_rnti_;
  }

  ue_ctxt.du_idx = get_du_index_from_ue_index(ue_index);
}

/// \brief Update a UE with PCI and/or C-RNTI.
void cu_cp_ue::update_du_ue(gnb_du_id_t du_id_, pci_t pci_, rnti_t c_rnti_)
{
  if (du_id_ != gnb_du_id_t::invalid) {
    ue_ctxt.du_id = du_id_;
  }

  if (pci_ != INVALID_PCI) {
    pci = pci_;
  }

  if (c_rnti_ != rnti_t::INVALID_RNTI) {
    ue_ctxt.crnti = c_rnti_;
  }
}

/// \brief Add the context for the NGAP UE.
/// \param[in] rrc_ue_pdu_notifier The RRC UE PDU notifier for the UE.
/// \param[in] rrc_ue_ctrl_notifier The RRC UE ctrl notifier for the UE.
/// \return Pointer to the NGAP UE notifier.
ngap_ue_notifier* cu_cp_ue::add_ngap_ue_context(ngap_rrc_ue_pdu_notifier&     rrc_ue_pdu_notifier,
                                                ngap_rrc_ue_control_notifier& rrc_ue_ctrl_notifier)
{
  ngap_ue_context.emplace(rrc_ue_pdu_notifier, rrc_ue_ctrl_notifier);
  ue_adapter.connect_ue(*this);
  return &ue_adapter;
}

/// \brief Set/update the measurement context of the UE.
void cu_cp_ue::update_meas_context(cell_meas_manager_ue_context meas_ctxt)
{
  meas_context = std::move(meas_ctxt);
}

/// \brief Set the DU and PCell index of the UE.
/// \param[in] pcell_index PCell index of the UE.
void cu_cp_ue::set_pcell_index(du_cell_index_t pcell_index_)
{
  pcell_index = pcell_index_;
}

/// \brief Set the RRC UE control message notifier of the UE.
/// \param[in] rrc_ue_notifier_ RRC UE control message notifier of the UE.
void cu_cp_ue::set_rrc_ue_notifier(du_processor_rrc_ue_control_message_notifier& rrc_ue_notifier_)
{
  rrc_ue_notifier = &rrc_ue_notifier_;
}

/// \brief Set the RRC UE SRB notifier of the UE.
/// \param[in] rrc_ue_srb_notifier_ RRC UE SRB control notifier of the UE.
void cu_cp_ue::set_rrc_ue_srb_notifier(du_processor_rrc_ue_srb_control_notifier& rrc_ue_srb_notifier_)
{
  rrc_ue_srb_notifier = &rrc_ue_srb_notifier_;
}

/// \brief Get the RRC UE PDU notifier of the UE.
ngap_rrc_ue_pdu_notifier& cu_cp_ue::get_rrc_ue_pdu_notifier()
{
  srsran_assert(ngap_ue_context.has_value(), "ue={}: NGAP UE was not created", ue_index);
  return ngap_ue_context.value().rrc_ue_pdu_notifier;
}

/// \brief Get the RRC UE control notifier of the UE.
ngap_rrc_ue_control_notifier& cu_cp_ue::get_rrc_ue_control_notifier()
{
  srsran_assert(ngap_ue_context.has_value(), "ue={}: NGAP UE was not created", ue_index);
  return ngap_ue_context.value().rrc_ue_ctrl_notifier;
}

/// \brief Get the RRC UE control message notifier of the UE.
du_processor_rrc_ue_control_message_notifier& cu_cp_ue::get_rrc_ue_notifier()
{
  srsran_assert(rrc_ue_notifier != nullptr, "ue={}: RRC UE notifier was not set", ue_index);
  return *rrc_ue_notifier;
}

/// \brief Get the RRC UE SRB control notifier of the UE.
du_processor_rrc_ue_srb_control_notifier& cu_cp_ue::get_rrc_ue_srb_notifier()
{
  srsran_assert(rrc_ue_srb_notifier != nullptr, "ue={}: RRC UE SRB notifier was not set", ue_index);
  return *rrc_ue_srb_notifier;
}
