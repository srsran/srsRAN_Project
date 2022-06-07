/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UE_MANAGER_H
#define SRSGNB_UE_MANAGER_H

#include "cu_cp_manager_config.h"
#include "cu_cp_manager_interfaces.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "ue_context.h"

namespace srsgnb {

namespace srs_cu_cp {

class ue_manager : public ue_manager_ctrl_configurer
{
public:
  explicit ue_manager(cu_cp_manager_config_t& cfg_);

  const slot_array<ue_context, MAX_NOF_UES>& get_ues() const { return ue_db; }

  ue_context* add_ue(ue_context u) override;
  void        remove_ue(ue_index_t ue_index) override;
  ue_context* find_ue(ue_index_t ue_index) override;

private:
  cu_cp_manager_config_t& cfg;
  srslog::basic_logger&   logger;

  slot_array<ue_context, MAX_NOF_UES> ue_db;

  // task event loops indexed by ue_index
  slot_array<async_task_sequencer, MAX_NOF_UES> ue_ctrl_loop;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_UE_MANAGER_H
