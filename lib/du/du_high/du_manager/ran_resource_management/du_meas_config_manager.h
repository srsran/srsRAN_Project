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

#include "du_ue_resource_config.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/srslog/logger.h"

namespace srsran::srs_du {

/// Class that handles the measConfig of the UE.
///
/// In particular, this class determines the measGaps that the need to be set for the UE based on the measConfig
/// provided by the CU-CP for the UE.
class du_meas_config_manager
{
public:
  du_meas_config_manager(span<const du_cell_config> cell_cfg_list);

  /// Update UE config based on UE measConfig given by the CU-CP.
  void update(du_ue_resource_config& ue_cfg, const byte_buffer& meas_cfg);

private:
  span<const du_cell_config> cell_cfg_list;
  srslog::basic_logger&      logger;
};

} // namespace srsran::srs_du
