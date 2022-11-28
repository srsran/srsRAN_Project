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

class ue_manager : public du_processor_ue_manager
{
public:
  explicit ue_manager();

  ue_context* add_ue(rnti_t rnti) override;
  void        remove_ue(ue_index_t ue_index) override;
  ue_context* find_ue(ue_index_t ue_index) override;
  ue_index_t  find_ue_index(rnti_t rnti) override;
  size_t      get_nof_ues() override;

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index();

  void clear_ue()
  {
    // TODO
  }

  srslog::basic_logger& logger = srslog::fetch_basic_logger("UE-MNG");

  slotted_array<ue_context, MAX_NOF_UES> ue_db;
  std::map<rnti_t, ue_index_t>           rnti_to_ue_index;
};

} // namespace srs_cu_cp

} // namespace srsgnb
