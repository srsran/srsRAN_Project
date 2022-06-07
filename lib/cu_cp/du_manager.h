/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DU_MANAGER_H
#define SRSGNB_DU_MANAGER_H

//#include "du_context.h"
#include "cu_cp_manager_config.h"
#include "cu_cp_manager_interfaces.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {

namespace srs_cu_cp {

class du_manager : public du_manager_ctrl_configurer
{
public:
  explicit du_manager(cu_cp_manager_config_t& cfg_);

  du_context* add_du(du_context du) override;
  void        remove_du(du_index_t du_index) override;
  du_context* find_du(du_index_t de_index) override;

private:
  cu_cp_manager_config_t& cfg;
  srslog::basic_logger&   logger;

  slot_array<du_context, MAX_NOF_DUS> du_db;

  // task event loops indexed by du_index
  slot_array<async_task_sequencer, MAX_NOF_DUS> du_ctrl_loop;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_H
