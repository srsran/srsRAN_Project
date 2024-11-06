/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_drx_controller.h"
#include "../config/ue_configuration.h"
#include "ul_logical_channel_manager.h"

using namespace srsran;

ue_drx_controller::ue_drx_controller(const cell_configuration&         cell_cfg_common_,
                                     const std::optional<drx_config>&  drx_cfg_,
                                     const ul_logical_channel_manager& ul_lc_mng_) :
  cell_cfg_common(cell_cfg_common_), drx_cfg(drx_cfg_), ul_lc_mng(ul_lc_mng_)
{
  if (drx_cfg.has_value()) {
    const subcarrier_spacing scs              = cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params.scs;
    const unsigned           nof_slots_per_sf = get_nof_slots_per_subframe(scs);

    active_window_period = drx_cfg.value().long_cycle.count() * nof_slots_per_sf;
    unsigned win_start   = drx_cfg.value().long_start_offset.count() * nof_slots_per_sf;
    active_window        = {win_start, win_start + drx_cfg.value().on_duration_timer.count() * nof_slots_per_sf};
    inactivity_dur       = drx_cfg.value().inactivity_timer.count() * nof_slots_per_sf;
  }
}

void ue_drx_controller::slot_indication(slot_point dl_slot)
{
  if (dl_slot >= active_time_end) {
    active_time_end = {};
  }

  const unsigned slot_mod = dl_slot.to_uint() % active_window_period;
  if (slot_mod >= active_window.start() and slot_mod < active_window.stop()) {
    // "the Active Time includes the time while [...] drx-onDuration [...] is running."
    slot_point new_end = dl_slot + (active_window.stop() - slot_mod);
    if (not active_time_end.valid() or new_end > active_time_end) {
      active_time_end = new_end;
    }
  }
}

bool ue_drx_controller::is_pdcch_enabled(slot_point dl_slot) const
{
  if (not drx_cfg.has_value()) {
    return true;
  }
  return is_active_time(dl_slot);
}

bool ue_drx_controller::is_active_time(slot_point dl_slot) const
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

  if (is_active_time(pdcch_slot)) {
    // "1> if the MAC entity is in Active Time
    // "  2> if the PDCCH indicates a new transmission (DL or UL) [...] start or restart drx-InactivityTimer in the
    // first symbol after the end of the PDCCH reception."
    active_time_end = pdcch_slot + inactivity_dur;
  }
}

void ue_drx_controller::on_con_res_start(slot_point msg3_slot_rx)
{
  if (not drx_cfg.has_value()) {
    return;
  }

  const subcarrier_spacing scs = cell_cfg_common.dl_cfg_common.init_dl_bwp.generic_params.scs;
  const unsigned conres_slots  = cell_cfg_common.ul_cfg_common.init_ul_bwp.rach_cfg_common->ra_con_res_timer.count() *
                                get_nof_slots_per_subframe(scs);

  slot_point new_time_end = msg3_slot_rx + conres_slots;

  if (not active_time_end.valid() or active_time_end < new_time_end) {
    // "the Active Time includes the time while [...] ra-ContentionResolutionTimer [...] is running."
    active_time_end = new_time_end;
  }
}
