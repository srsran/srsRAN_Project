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
  ngc_ue& add_ue(cu_cp_ue_id_t                cu_cp_ue_id,
                 ngc_rrc_ue_pdu_notifier&     rrc_ue_pdu_notifier,
                 ngc_rrc_ue_control_notifier& rrc_ue_ctrl_notifier) override;
  void    remove_ue(cu_cp_ue_id_t cu_cp_ue_id) override;

  ngc_ue* find_ue(std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint) override;
  ngc_ue* find_ue(cu_cp_ue_id_t cu_cp_ue_id) override { return find_ue(cu_cp_ue_id_to_uint(cu_cp_ue_id)); }

  size_t get_nof_ngc_ues() override { return ngc_ues.size(); }

  cu_cp_ue_id_t get_cu_cp_ue_id(std::underlying_type_t<ran_ue_id_t> ran_ue_id_uint) override;
  cu_cp_ue_id_t get_cu_cp_ue_id(ran_ue_id_t ran_ue_id) override
  {
    return get_cu_cp_ue_id(ran_ue_id_to_uint(ran_ue_id));
  }

  bool contains(std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint) override
  {
    return ngc_ues.contains(cu_cp_ue_id_uint);
  }
  bool contains(cu_cp_ue_id_t cu_cp_ue_id) override { return contains(cu_cp_ue_id_to_uint(cu_cp_ue_id)); }

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index();

  /// \brief Get the next available RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t get_next_ran_ue_id();

  /// \brief Find the RAN UE ID by a given UE NGAP ID.
  /// \param[in] cu_cp_ue_id The UE NGAP ID used to find the RAN UE ID.
  /// \return The RAN UE ID.
  ran_ue_id_t find_ran_ue_id(cu_cp_ue_id_t cu_cp_ue_id);

  void clear_ue()
  {
    // TODO
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("UE-MNG");

  slotted_array<ue_context, MAX_NOF_UES> ue_db;
  std::map<rnti_t, ue_index_t>           rnti_to_ue_index;

  slotted_array<ngc_ue, MAX_NOF_CU_UES>        ngc_ues;
  slotted_array<cu_cp_ue_id_t, MAX_NOF_CU_UES> ran_ue_id_to_cu_cp_ue_id; // cu_cp_ue_ids indexed by ran_ue_ids
};

} // namespace srs_cu_cp

} // namespace srsgnb
