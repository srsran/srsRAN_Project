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

#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/cu_cp/ue_manager.h"
#include <unordered_map>

namespace srsgnb {

namespace srs_cu_cp {

class ue_manager : public du_processor_ue_manager, public ngc_ue_manager
{
public:
  explicit ue_manager();

  // du_processor_ue_manager
  ue_context* add_ue(rnti_t rnti) override;
  void        remove_ue(ue_index_t ue_index) override;
  ue_context* find_ue(ue_index_t ue_index) override;
  ue_index_t  find_ue_index(rnti_t rnti) override;
  size_t      get_nof_ues() override;

  // ngc_ue_manager
  ngc_ue& add_ngap_ue(ue_index_t                         ue_index,
                      ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                      ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                      ngc_du_processor_control_notifier& du_processor_ctrl_notifier_) override;
  void    remove_ngap_ue(ue_index_t ue_index) override;

  ngc_ue* find_ngap_ue(ue_index_t ue_index) override;

  size_t get_nof_ngap_ues() override { return ngap_ues.size(); }

  void set_amf_ue_id(ue_index_t ue_index, amf_ue_id_t amf_ue_id) override;

  ue_index_t get_ue_index(ran_ue_id_t ran_ue_id) override;
  ue_index_t get_ue_index(amf_ue_id_t amf_ue_id) override;

  bool contains(ue_index_t ue_index) override { return ngap_ues.find(ue_index) != ngap_ues.end(); }

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index();

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

  slotted_array<ue_context, MAX_NOF_UES_PER_DU> ue_db;
  std::map<rnti_t, ue_index_t>                  rnti_to_ue_index;

  std::unordered_map<ue_index_t, ngc_ue>      ngap_ues;              // ngap_ues indexed by ue_index
  std::unordered_map<ran_ue_id_t, ue_index_t> ran_ue_id_to_ue_index; // ue_indexs indexed by ran_ue_ids
  std::unordered_map<amf_ue_id_t, ue_index_t> amf_ue_id_to_ue_index; // ue_indexs indexed by amf_ue_ids
};

} // namespace srs_cu_cp

} // namespace srsgnb
