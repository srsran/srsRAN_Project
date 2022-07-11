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

#include "cu_cp_manager_context.h"
#include "du_processor.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_manager.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/executors/task_executor.h"
#include <memory>
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

class cu_cp_manager_impl final : public cu_cp_manager_interface
{
public:
  explicit cu_cp_manager_impl(const cu_cp_manager_config_t& cfg);

  void add_du() override;
  void remove_du(du_index_t du_index) override;

  du_processor* find_du(du_index_t du_index) override;
  du_processor* find_du(uint64_t packed_nr_cell_id) override;

  size_t               get_nof_dus() const override;
  size_t               get_nof_ues() const override;
  f1c_message_handler* get_f1c_message_handler(const du_index_t du_index) override;

  void on_new_connection() override;
  void handle_du_remove_request(const du_index_t du_index) override;

private:
  /// \brief Get the next available index from the DU processor database.
  /// \return The DU index.
  du_index_t get_next_du_index();

  // DU manager configuration that will be visible to all running procedures
  cu_cp_manager_config_t cfg;

  // logger
  srslog::basic_logger& logger;

  // CU-CP context.
  cu_cp_manager_context ctx;

  // Components
  slot_array<std::unique_ptr<du_processor>, MAX_NOF_DUS> du_db;

  std::unordered_map<uint64_t, du_processor*> du_dict; // Hash-table to find DU by cell_id

  // task event loops indexed by du_index
  slot_array<async_task_sequencer, MAX_NOF_DUS> du_ctrl_loop;

  // Handler for DU tasks.
  async_task_sequencer main_ctrl_loop;
};

} // namespace srs_cu_cp

} // namespace srsgnb
