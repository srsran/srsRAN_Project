/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cell_meas_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Basic cell manager implementation
class cell_meas_manager_impl final : public cell_meas_manager
{
public:
  cell_meas_manager_impl(const cell_meas_manager_cfg& cfg);
  ~cell_meas_manager_impl() = default;

  rrc_meas_cfg get_measurement_config(nr_cell_id_t nci) override;
  bool         update_cell_config(nr_cell_id_t nci, const cell_meas_cfg& cfg) override;
  void         report_measurement(const rrc_meas_results& meas_results) override;

private:
  cell_meas_manager_cfg cfg;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
