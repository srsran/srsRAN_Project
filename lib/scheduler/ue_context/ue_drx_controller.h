/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  /// Update DRX controller state.
  void slot_indication(slot_point dl_slot);

  /// Determines whether the PDCCH can be allocated for a given slot.
  bool is_pdcch_enabled(slot_point dl_slot) const;

  /// Update DRX active time based on new PDCCH allocations.
  void on_new_pdcch_alloc(slot_point dl_slot);

  /// Update DRX active time based on ContentionResolutionTimer.
  void on_con_res_start();

private:
  /// Whether the UE is within DRX active time.
  bool is_active_time(slot_point dl_slot) const;

  const subcarrier_spacing          scs_common;
  std::chrono::milliseconds         conres_timer;
  const std::optional<drx_config>   drx_cfg;
  const ul_logical_channel_manager& ul_lc_mng;
  std::optional<slot_point>         ul_ccch_slot_rx;
  srslog::basic_logger&             logger;

  // Converted config parameters from milliseconds to slots.
  unsigned           active_window_period;
  interval<unsigned> active_window;
  unsigned           inactivity_dur;

  // End slot for the active window. When invalid, the UE is not in active window.
  slot_point active_time_end;
};

} // namespace srsran