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

#include "cu_cp_manager_config.h"
#include "srsgnb/adt/slot_array.h"
#include "ue_manager_interfaces.h"

namespace srsgnb {

namespace srs_cu_cp {

class ue_manager : public ue_manager_ctrl_configurer
{
public:
  explicit ue_manager(cu_cp_manager_config_t& cfg_);

  const slot_array<ue_context, MAX_NOF_UES>& get_ues() const { return ue_db; }

  ue_context* add_ue(rnti_t rnti) override;
  void        remove_ue(ue_index_t ue_index) override;
  ue_context* find_ue(ue_index_t ue_index) override;
  ue_context* find_rnti(rnti_t rnti) override;
  size_t      get_nof_ues() override;

private:
  /// \brief Get the next available UE index.
  /// \return The UE index.
  ue_index_t get_next_ue_index();

  void clear_ue()
  {
    // TODO
  }

  void create_srb0(ue_context& ue_ctx);

  cu_cp_manager_config_t& cfg;
  srslog::basic_logger&   logger;

  slot_array<ue_context, MAX_NOF_UES> ue_db;
  std::array<int, MAX_NOF_UES>        rnti_to_ue_index;
};

} // namespace srs_cu_cp

} // namespace srsgnb
