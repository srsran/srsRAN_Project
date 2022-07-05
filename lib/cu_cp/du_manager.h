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

#include "cu_cp_manager_config.h"
#include "cu_cp_manager_interfaces.h"
#include "du_processor.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/support/async/async_task_loop.h"
#include <unordered_map>

namespace srsgnb {

namespace srs_cu_cp {

class du_manager : public du_manager_ctrl_configurer
{
public:
  explicit du_manager(cu_cp_manager_config_t& cfg_);

  du_processor* add_du(std::unique_ptr<du_processor> du) override;
  void          remove_du(du_index_t du_index) override;
  du_index_t    get_next_du_index() override;
  du_processor* find_du(du_index_t du_index) override;
  du_processor* find_du(uint64_t packed_nr_cell_id) override;
  size_t        get_nof_dus() const override;

  void handle_initial_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg) override;

private:
  cu_cp_manager_config_t& cfg;
  srslog::basic_logger&   logger;

  slot_array<std::unique_ptr<du_processor>, MAX_NOF_DUS> du_db;
  std::atomic<uint16_t>                                  next_du_index{0};

  std::unordered_map<uint64_t, du_processor*> du_dict; // Hash-table to find DU by cell_id

  // task event loops indexed by du_index
  slot_array<async_task_sequencer, MAX_NOF_DUS> du_ctrl_loop;
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_DU_MANAGER_H
