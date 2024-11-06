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
#include <optional>

namespace srsran {

class cell_configuration;
class ue_cell_configuration;
class ul_logical_channel_manager;

/// Class that determines UE DRX active time.
class ue_drx_controller
{
public:
  ue_drx_controller(const cell_configuration&         cell_cfg_common_,
                    const std::optional<drx_config>&  drx_cfg_,
                    const ul_logical_channel_manager& ul_lc_mng);

  /// Update DRX controller state.
  void slot_indication(slot_point dl_slot);

  /// Determines whether the PDCCH can be allocated for a given slot.
  bool is_pdcch_enabled(slot_point dl_slot) const;

  /// Update DRX active time based on new PDCCH allocations.
  void on_new_pdcch_alloc(slot_point dl_slot);

  /// Update DRX active time based on ContentionResolutionTimer.
  void on_con_res_start(slot_point msg3_slot_rx);

private:
  /// Whether the UE is within DRX active time.
  bool is_active_time(slot_point dl_slot) const;

  const cell_configuration&         cell_cfg_common;
  const std::optional<drx_config>&  drx_cfg;
  const ul_logical_channel_manager& ul_lc_mng;
  unsigned                          active_window_period;
  interval<unsigned>                active_window;
  unsigned                          inactivity_dur;

  // End slot for the active window. When invalid, the UE is not in active window.
  slot_point active_time_end;
};

} // namespace srsran