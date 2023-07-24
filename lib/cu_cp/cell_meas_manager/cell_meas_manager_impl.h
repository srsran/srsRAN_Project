/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/cu_cp/mobility_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Basic cell manager implementation
class cell_meas_manager_impl final : public cell_meas_manager
{
public:
  cell_meas_manager_impl(const cell_meas_manager_cfg& cfg, cell_meas_mobility_manager_notifier& mobility_mng_notfier_);
  ~cell_meas_manager_impl() = default;

  optional<rrc_meas_cfg>     get_measurement_config(nr_cell_id_t nci) override;
  optional<cell_meas_config> get_cell_config(nr_cell_id_t nci) override;
  void                       update_cell_config(nr_cell_id_t                    nci,
                                                const serving_cell_meas_config& serv_cell_cfg_,
                                                std::vector<nr_cell_id_t>       ncells_ = {}) override;
  void report_measurement(const ue_index_t ue_index, const rrc_meas_results& meas_results) override;

private:
  cell_meas_manager_cfg                cfg;
  cell_meas_mobility_manager_notifier& mobility_mng_notifier;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
