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
    const unsigned slot_mod                  = dl_slot.to_uint() % active_window_period;
    bool is_slot_in_on_duration_timer_window = slot_mod >= active_window.start() and slot_mod < active_window.stop();
    if (not is_slot_in_on_duration_timer_window and active_window.stop() >= active_window_period) {
      // We must also check the interval [0, window_end % window_period).
      is_slot_in_on_duration_timer_window = slot_mod < active_window.stop() % active_window_period;
    }

    if (is_slot_in_on_duration_timer_window) {
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
void ue_drx_controller::update_inactivity_timer(slot_point pdcch_slot)
{
  if (inactivity_dur != 0) {
    // "  2> if the PDCCH indicates a new transmission (DL or UL) [...] start or restart drx-InactivityTimer in the
    //       first symbol after the end of the PDCCH reception."
    slot_point inactivity_timer_end_slot = pdcch_slot + inactivity_dur;

    if (not active_time_end.valid() or active_time_end < inactivity_timer_end_slot) {
      active_time_end = inactivity_timer_end_slot;
    }
  }
}

void ue_drx_controller::on_new_dl_pdcch_alloc(slot_point pdcch_slot)
{
  if (not drx_cfg.has_value()) {
    return;
  }

  // "1> if the MAC entity is in Active Time
  if (not is_active_time()) {
    return;
  }

  update_inactivity_timer(pdcch_slot);
}

void ue_drx_controller::on_new_ul_pdcch_alloc(slot_point pdcch_slot, slot_point pusch_slot)
{
  if (not drx_cfg.has_value()) {
    return;
  }

  // "1> if the MAC entity is in Active Time
  if (not is_active_time()) {
    return;
  }

  update_inactivity_timer(pdcch_slot);

  // "  2> if the PDCCH indicates a UL transmission:
  // "    3> start the drx-HARQ-RTT-TimerUL for the corresponding HARQ process in the first symbol after the end of
  //         the first transmission (within a bundle) of the corresponding PUSCH transmission;
  // [Implementation defined] drx-HARQ-RTT-TimerUL is always 0.
  if (drx_cfg->retx_timer_ul != 0) {
    // "1> if a drx-HARQ-RTT-TimerUL expires:
    // "  2> start the drx-RetransmissionTimerUL for the corresponding HARQ process in the first symbol after the
    //       expiry of drx-HARQ-RTT-TimerUL.
    slot_point retx_timer_end_slot = pusch_slot + drx_cfg->retx_timer_ul;

    if (not active_time_end.valid() or active_time_end < retx_timer_end_slot) {
      active_time_end = retx_timer_end_slot;
    }
  }
}

void ue_drx_controller::on_dl_harq_nack(slot_point uci_slot)
{
  if (not drx_cfg.has_value()) {
    return;
  }

  // "  2> if the PDCCH indicates a DL transmission:
  // "    3> start the drx-HARQ-RTT-TimerDL for the corresponding HARQ process in the first symbol after the end of the
  // "       corresponding transmission carrying the DL HARQ feedback;
  // [Implementation defined] drx-HARQ-TimerDL is always 0.
  if (drx_cfg->retx_timer_dl != 0) {
    // "1> if a drx-HARQ-RTT-TimerDL expires:
    // "  2> if the data of the corresponding HARQ process was not successfully decoded:
    // "    3> start the drx-RetransmissionTimerDL for the corresponding HARQ process in the first symbol after the
    // "       expiry of drx-HARQ-RTT-TimerDL.
    slot_point new_end = uci_slot + drx_cfg->retx_timer_dl;
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
