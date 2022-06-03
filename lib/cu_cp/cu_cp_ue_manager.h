/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_UE_MANAGER_H
#define SRSGNB_CU_CP_UE_MANAGER_H

#include "cu_cp_manager_config.h"
#include "cu_cp_manager_interfaces.h"
#include "cu_cp_ue_context.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {

namespace cu_cp {

class cu_cp_ue_manager : public cu_cp_ue_manager_ctrl_configurer
{
public:
  explicit cu_cp_ue_manager(cu_cp_manager_config_t& cfg_);

  const slot_array<cu_cp_ue_context, MAX_NOF_CU_CP_UES>& get_ues() const { return ue_db; }

  cu_cp_ue_context* add_ue(cu_cp_ue_context u) override;
  void              remove_ue(cu_cp_ue_index_t ue_index) override;
  cu_cp_ue_context* find_ue(cu_cp_ue_index_t ue_index) override;

private:
  cu_cp_manager_config_t& cfg;
  srslog::basic_logger&   logger;

  slot_array<cu_cp_ue_context, MAX_NOF_CU_CP_UES> ue_db;

  // task event loops indexed by ue_index
  slot_array<async_task_sequencer, MAX_NOF_CU_CP_UES> ue_ctrl_loop;
};

} // namespace cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_UE_MANAGER_H
