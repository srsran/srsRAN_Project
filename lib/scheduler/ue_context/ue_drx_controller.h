/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

  /// Update UE DRX configuration.
  void reconfigure(const std::optional<drx_config>& new_drx_cfg);

  /// Update DRX controller state.
  void slot_indication(slot_point dl_slot);

  /// Determines whether the PDCCH can be allocated for a given slot.
  bool is_pdcch_enabled() const;

  /// Update DRX active time based on new PDCCH allocations.
  template <bool IsDl>
  void on_new_pdcch_alloc(slot_point pdcch_slot, slot_point pxsch_slot)
  {
    if (not drx_cfg.has_value()) {
      return;
    }

    if (not is_active_time()) {
      return;
    }

    // "1> if the MAC entity is in Active Time
    if (inactivity_dur != 0) {
      // "  2> if the PDCCH indicates a new transmission (DL or UL) [...] start or restart drx-InactivityTimer in the
      //       first symbol after the end of the PDCCH reception."
      slot_point inactivity_timer_end_slot = pdcch_slot + inactivity_dur;

      if (not active_time_end.valid() or active_time_end < inactivity_timer_end_slot) {
        active_time_end = inactivity_timer_end_slot;
      }
    }

    // For DL, the drx-HARQ-RTT-TimerUL is only started by the UE when the HARQ process is not successfully decoded, so
    // we don't know if that timer will be set at this point. See \ref ue_drx_controller::on_pucch_harq_nack.
    if (not IsDl) {
      // "  2> if the PDCCH indicates a UL transmission:
      // "    3> start the drx-HARQ-RTT-TimerUL for the corresponding HARQ process in the first symbol after the end of
      //         the first transmission (within a bundle) of the corresponding PUSCH transmission;
      // [Implementation defined] drx-HARQ-RTT-TimerUL is always 0.
      if (drx_cfg->retx_timer_ul != 0) {
        // "1> if a drx-HARQ-RTT-TimerUL expires:
        // "  2> start the drx-RetransmissionTimerUL for the corresponding HARQ process in the first symbol after the
        //       expiry of drx-HARQ-RTT-TimerUL.
        slot_point retx_timer_end_slot = pxsch_slot + drx_cfg->retx_timer_ul;

        if (not active_time_end.valid() or active_time_end < retx_timer_end_slot) {
          active_time_end = retx_timer_end_slot;
        }
      }
    }
  }

  /// Update DRX active time based on a PUCCH HARQ NACK.
  void on_dl_harq_nack(slot_point uci_slot);

  /// Update DRX active time based on ContentionResolutionTimer.
  void on_con_res_start();

private:
  /// Whether the UE is within DRX active time.
  bool is_active_time() const;

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
