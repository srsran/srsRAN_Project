/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CU_CP_DU_MANAGER_H
#define SRSGNB_CU_CP_DU_MANAGER_H

//#include "cu_cp_du_context.h"
#include "cu_cp_manager_config.h"
#include "cu_cp_manager_interfaces.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {

namespace cu_cp {

class cu_cp_du_manager : public cu_cp_du_manager_ctrl_configurer
{
public:
  explicit cu_cp_du_manager(cu_cp_manager_config_t& cfg_);

  cu_cp_du_context* add_du(cu_cp_du_context du) override;
  void              remove_du(cu_cp_du_index_t du_index) override;
  cu_cp_du_context* find_du(cu_cp_du_index_t de_index) override;

private:
  cu_cp_manager_config_t& cfg;
  srslog::basic_logger&   logger;

  slot_array<cu_cp_du_context, MAX_NOF_CU_CP_DUS> du_db;

  // task event loops indexed by du_index
  slot_array<async_task_sequencer, MAX_NOF_CU_CP_DUS> du_ctrl_loop;
};

} // namespace cu_cp

} // namespace srsgnb

#endif // SRSGNB_CU_CP_DU_MANAGER_H
