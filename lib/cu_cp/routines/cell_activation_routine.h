/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../ue_manager/ue_manager_impl.h"
#include "srsran/f1ap/cu_cp/f1ap_cu_configuration_update.h"
#include <unordered_set>

namespace srsran {
namespace srs_cu_cp {

/// \brief Handles the activation of the cell.
class cell_activation_routine
{
public:
  cell_activation_routine(const cu_cp_configuration&        cu_cp_cfg_,
                          const std::vector<plmn_identity>& plmns_,
                          du_processor_repository&          du_db_,
                          srslog::basic_logger&             logger_);
  ~cell_activation_routine() = default;

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "Cell Activation Routine"; }

  void get_plmns_to_activate(const du_cell_configuration& cell_cfg);

  bool generate_gnb_cu_configuration_update();

private:
  const cu_cp_configuration&       cu_cp_cfg;
  const std::vector<plmn_identity> plmns;
  du_processor_repository&         du_db;
  srslog::basic_logger&            logger;

  // (Sub-)Routine requests.
  f1ap_gnb_cu_configuration_update f1ap_cu_cfg_update;

  // (Sub-)Routine results.
  f1ap_gnb_cu_configuration_update_response f1ap_cu_cfg_update_response;

  std::unordered_set<plmn_identity> plmns_to_activate;
  std::vector<du_index_t>           du_indexes;
  std::vector<du_index_t>::iterator du_idx_it;
  du_processor*                     du_proc = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
