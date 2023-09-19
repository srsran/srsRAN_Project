/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ue_link_adaptation_controller.h"
#include "../support/mcs_calculator.h"

using namespace srsran;

ue_link_adaptation_controller::ue_link_adaptation_controller(const cell_configuration&       cell_cfg_,
                                                             const ue_channel_state_manager& ue_channel_state) :
  cell_cfg(cell_cfg_), ue_ch_st(ue_channel_state)
{
  if (cell_cfg.expert_cfg.ue.olla_cqi_inc > 0) {
    dl_olla.emplace(cell_cfg.expert_cfg.ue.olla_dl_target_bler,
                    cell_cfg.expert_cfg.ue.olla_cqi_inc,
                    cell_cfg.expert_cfg.ue.olla_max_cqi_offset);
  }
  if (cell_cfg.expert_cfg.ue.olla_ul_snr_inc > 0) {
    ul_olla.emplace(cell_cfg.expert_cfg.ue.olla_ul_target_bler,
                    cell_cfg.expert_cfg.ue.olla_ul_snr_inc,
                    cell_cfg.expert_cfg.ue.olla_max_ul_snr_offset);
  }
}

void ue_link_adaptation_controller::handle_dl_ack_info(mac_harq_ack_report_status ack_value,
                                                       sch_mcs_index              used_mcs,
                                                       pdsch_mcs_table            mcs_table)
{
  static constexpr unsigned MAX_CQI = 15;

  if (ack_value == mac_harq_ack_report_status::dtx or not dl_olla.has_value()) {
    return;
  }

  const sch_mcs_index                 max_mcs = map_cqi_to_mcs(MAX_CQI, mcs_table).value();
  const interval<sch_mcs_index, true> mcs_bounds{cell_cfg.expert_cfg.ue.dl_mcs.start(),
                                                 std::min(cell_cfg.expert_cfg.ue.dl_mcs.stop(), max_mcs)};

  dl_olla->update(ack_value == mac_harq_ack_report_status::ack, used_mcs, mcs_bounds);
}

void ue_link_adaptation_controller::handle_ul_crc_info(bool crc, sch_mcs_index used_mcs, pusch_mcs_table mcs_table)
{
  if (not ul_olla.has_value()) {
    return;
  }

  const interval<sch_mcs_index, true> mcs_bounds{
      cell_cfg.expert_cfg.ue.dl_mcs.start(), std::min(cell_cfg.expert_cfg.ue.dl_mcs.stop(), get_max_mcs_ul(mcs_table))};
  ul_olla->update(crc, used_mcs, mcs_bounds);
}

float ue_link_adaptation_controller::get_effective_cqi() const
{
  float eff_cqi = static_cast<float>(ue_ch_st.get_wideband_cqi().value());
  if (eff_cqi == 0.0F) {
    // CQI==0 is a special case, where the channel is not considered in a valid state.
    return eff_cqi;
  }

  if (dl_olla.has_value()) {
    // For the case of DL outer loop link adaptation enabled.
    eff_cqi += dl_olla->offset_db();
    // Ensure CQI remains within [1, 15].
    eff_cqi = std::min(std::max(1.0F, eff_cqi), static_cast<float>(cqi_value::max()));
  }
  return eff_cqi;
}

float ue_link_adaptation_controller::get_effective_snr() const
{
  return ue_ch_st.get_pusch_snr() + (ul_olla.has_value() ? ul_olla.value().offset_db() : 0.0f);
}

optional<sch_mcs_index> ue_link_adaptation_controller::calculate_dl_mcs(pdsch_mcs_table mcs_table) const
{
  if (cell_cfg.expert_cfg.ue.dl_mcs.length() == 0) {
    // Fixed MCS.
    return cell_cfg.expert_cfg.ue.dl_mcs.start();
  }

  // Derive MCS using the combination of CQI + outer loop link adaptation.
  const float eff_cqi = get_effective_cqi();
  if (eff_cqi == 0.0F) {
    // Special case, where reported CQI==0.
    return nullopt;
  }

  // There are fewer CQIs than MCS values, so we perform a linear interpolation.
  const float   cqi_lb = floorf(eff_cqi), cqi_ub = ceilf(eff_cqi);
  const float   coeff  = eff_cqi - cqi_lb;
  const float   mcs_lb = static_cast<float>(map_cqi_to_mcs(static_cast<unsigned>(cqi_lb), mcs_table).value().value());
  const float   mcs_ub = static_cast<float>(map_cqi_to_mcs(static_cast<unsigned>(cqi_ub), mcs_table).value().value());
  sch_mcs_index mcs{static_cast<uint8_t>(floorf(mcs_lb * (1 - coeff) + mcs_ub * coeff))};

  // Ensures that the MCS is within the configured range.
  mcs = std::min(std::max(mcs, cell_cfg.expert_cfg.ue.dl_mcs.start()), cell_cfg.expert_cfg.ue.dl_mcs.stop());
  return mcs;
}

sch_mcs_index ue_link_adaptation_controller::calculate_ul_mcs(pusch_mcs_table mcs_table) const
{
  if (cell_cfg.expert_cfg.ue.ul_mcs.length() == 0) {
    // Fixed MCS.
    return cell_cfg.expert_cfg.ue.ul_mcs.start();
  }

  // Derive MCS using the combination of estimated UL SNR + outer loop link adaptation.
  sch_mcs_index mcs = map_snr_to_mcs_ul(get_effective_snr(), mcs_table);
  mcs = std::min(std::max(mcs, cell_cfg.expert_cfg.ue.ul_mcs.start()), cell_cfg.expert_cfg.ue.ul_mcs.stop());

  return mcs;
}
