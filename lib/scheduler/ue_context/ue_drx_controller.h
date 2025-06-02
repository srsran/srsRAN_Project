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

#include "srsran/ran/drx_config.h"
#include "srsran/ran/slot_point.h"
#include "srsran/srslog/logger.h"
#include <optional>

namespace srsran {

class cell_configuration;
class ue_cell_configuration;
class ul_logical_channel_manager;

/// Class that determines UE DRX active time.
class ue_drx_controller
{
public:
  ue_drx_controller(subcarrier_spacing                scs_common,
                    std::chrono::milliseconds         conres_timer,
                    const std::optional<drx_config>&  drx_cfg,
                    const ul_logical_channel_manager& ul_lc_mng,
                    std::optional<slot_point>         ul_ccch_slot_rx,
                    srslog::basic_logger&             logger);

  /// Update UE DRX configuration.
  void reconfigure(const std::optional<drx_config>& new_drx_cfg);

  /// Update DRX controller state.
  void slot_indication(slot_point dl_slot);

  /// Determines whether the PDCCH can be allocated for a given slot.
  bool is_pdcch_enabled() const;

  /// Update DRX active time based on new DL PDCCH allocations.
  void on_new_dl_pdcch_alloc(slot_point pdcch_slot);

  /// Update DRX active time based on new UL PDCCH allocations.
  void on_new_ul_pdcch_alloc(slot_point pdcch_slot, slot_point pusch_slot);

  /// Update DRX active time based on a PUCCH HARQ NACK.
  void on_dl_harq_nack(slot_point uci_slot);

  /// Update DRX active time based on ContentionResolutionTimer.
  void on_con_res_start();

private:
  /// Whether the UE is within DRX active time.
  bool is_active_time() const;
  void update_inactivity_timer(slot_point pdcch_slot);

  const subcarrier_spacing          scs_common;
  std::chrono::milliseconds         conres_timer;
  const ul_logical_channel_manager& ul_lc_mng;
  std::optional<slot_point>         ul_ccch_slot_rx;
  srslog::basic_logger&             logger;

  // Current UE DRX config.
  std::optional<drx_config> drx_cfg;

  // Converted config parameters from milliseconds to slots.
  unsigned           active_window_period;
  interval<unsigned> active_window;
  unsigned           inactivity_dur;

  // End slot for the active window. When invalid, the UE is not in active window.
  slot_point active_time_end;
};

} // namespace srsran
