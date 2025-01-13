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

#include "ue_drx_controller.h"
#include "ul_logical_channel_manager.h"

using namespace srsran;

ue_drx_controller::ue_drx_controller(subcarrier_spacing                scs_common_,
                                     std::chrono::milliseconds         conres_timer_,
                                     const std::optional<drx_config>&  drx_cfg_,
                                     const ul_logical_channel_manager& ul_lc_mng_,
                                     std::optional<slot_point>         ul_ccch_slot_rx_,
                                     srslog::basic_logger&             logger_) :
  scs_common(scs_common_),
  conres_timer(conres_timer_),
  ul_lc_mng(ul_lc_mng_),
  ul_ccch_slot_rx(ul_ccch_slot_rx_),
  logger(logger_)
{
  reconfigure(drx_cfg_);
}

void ue_drx_controller::reconfigure(const std::optional<drx_config>& new_drx_cfg)
{
  if (drx_cfg == new_drx_cfg) {
    // Nothing happens.
    return;
  }
  if (not new_drx_cfg.has_value()) {
    // DRX is disabled.
    drx_cfg.reset();
    return;
  }

  drx_cfg = new_drx_cfg;

  const unsigned nof_slots_per_sf = get_nof_slots_per_subframe(scs_common);

  // Convert milliseconds to slots.
  active_window_period = drx_cfg.value().long_cycle.count() * nof_slots_per_sf;
  unsigned win_start   = drx_cfg.value().long_start_offset.count() * nof_slots_per_sf;
  active_window        = {win_start, win_start + drx_cfg.value().on_duration_timer.count() * nof_slots_per_sf};
  inactivity_dur       = drx_cfg.value().inactivity_timer.count() * nof_slots_per_sf;
}

void ue_drx_controller::slot_indication(slot_point dl_slot)
{
  if (not drx_cfg.has_value()) {
    return;
  }

  if (active_time_end.valid() and dl_slot >= active_time_end) {
    active_time_end = {};
  }

  if (not active_time_end.valid()) {
    const unsigned slot_mod = dl_slot.to_uint() % active_window_period;
    if (slot_mod >= active_window.start() and slot_mod < active_window.stop()) {
      // "the Active Time includes the time while [...] drx-onDuration [...] is running."
      slot_point new_end = dl_slot + (active_window.stop() - slot_mod);
      active_time_end    = new_end;
    }
  }
}

bool ue_drx_controller::is_pdcch_enabled() const
{
  if (not drx_cfg.has_value()) {
    return true;
  }
  return is_active_time();
}

bool ue_drx_controller::is_active_time() const
{
  if (active_time_end.valid()) {
    // active_time_end is set during drx-onDurationTimer or drx-InactivityTimer.
    return true;
  }

  if (ul_lc_mng.has_pending_sr()) {
    // "the Active Time includes the time while [...] a Scheduling Request is sent on PUCCH and is pending"
    return true;
  }

  return false;
}

void ue_drx_controller::on_new_pdcch_alloc(slot_point pdcch_slot)
{
  if (not drx_cfg.has_value()) {
    return;
  }

  if (inactivity_dur != 0 and is_active_time()) {
    // "1> if the MAC entity is in Active Time
    // "  2> if the PDCCH indicates a new transmission (DL or UL) [...] start or restart drx-InactivityTimer in the
    // first symbol after the end of the PDCCH reception."
    slot_point new_end = pdcch_slot + inactivity_dur;
    if (not active_time_end.valid() or active_time_end < new_end) {
      active_time_end = new_end;
    }
  }
}

void ue_drx_controller::on_con_res_start()
{
  if (not drx_cfg.has_value()) {
    return;
  }
  if (not ul_ccch_slot_rx.has_value() or not ul_ccch_slot_rx.value().valid()) {
    logger.error("Setting conRes timer but slot of UL-CCCH was not registered");
    return;
  }

  const unsigned conres_slots = conres_timer.count() * get_nof_slots_per_subframe(scs_common);

  slot_point new_time_end = ul_ccch_slot_rx.value() + conres_slots;

  if (not active_time_end.valid() or active_time_end < new_time_end) {
    // "the Active Time includes the time while [...] ra-ContentionResolutionTimer [...] is running."
    active_time_end = new_time_end;
  }
}
