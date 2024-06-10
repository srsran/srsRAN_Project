/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "apps/units/flexible_du/du_high/du_high_config.h"
#include "apps/units/flexible_du/du_low/du_low_config.h"
#include "apps/units/flexible_du/fapi/fapi_config.h"
#include "apps/units/flexible_du/split_7_2/ru_ofh_config.h"
#include "apps/units/flexible_du/split_8/ru_sdr_config.h"
#include <variant>

namespace srsran {

/// CPU affinities configuration for the cell.
struct ru_dummy_cpu_affinities_cell_unit_config {
  /// Radio Unit workers CPU affinity mask.
  os_sched_affinity_config ru_cpu_cfg = {sched_affinity_mask_types::ru, {}, sched_affinity_mask_policy::mask};
};

/// gNB app dummy Radio Unit configuration.
struct ru_dummy_unit_config {
  /// \brief DL processing delay in slots.
  ///
  /// It is the number of slots that the RU expects the downlink resource grid in advance for having enough time margin
  /// for processing.
  unsigned dl_processing_delay = 1;
  /// \brief CPU affinities per cell of the SDR Radio Unit.
  ///
  /// \note Add one cell by default.
  std::vector<ru_dummy_cpu_affinities_cell_unit_config> cell_affinities = {{}};
};

/// Dynamic DU unit configuration.
struct dynamic_du_unit_config {
  /// DU high configuration.
  du_high_parsed_config du_high_cfg;
  /// DU low configuration.
  du_low_unit_config du_low_cfg;
  /// FAPI configuration.
  fapi_unit_config fapi_cfg;
  /// Radio Unit configuration.
  std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config, ru_dummy_unit_config> ru_cfg = {ru_sdr_unit_config{}};
};

} // namespace srsran
