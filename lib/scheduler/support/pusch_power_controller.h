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

#include "../ue_context/ue_channel_state_manager.h"
#include "srsran/adt/circular_array.h"

namespace srsran {

class ue_cell_configuration;

class pusch_power_controller
{
public:
  pusch_power_controller(const ue_cell_configuration& ue_cell_cfg_, const ue_channel_state_manager& ch_state_manager);

  /// Save the PUSCH power control configuration.
  void reconfigure(const ue_cell_configuration& ue_cell_cfg);

  /// Update UE with the latest PHR for a given cell.
  void handle_phr(const cell_ph_report& phr, slot_point slot_rx);

  /// Save the PUSCH power control parameters after each PUSCH transmission.
  void update_pusch_pw_ctrl_state(slot_point slot, unsigned nof_prbs);

  /// Adapt the number of PUSCH PRBs to the PHR.
  unsigned adapt_pusch_prbs_to_phr(unsigned nof_prbs) const;

  /// Compute the TPC command for the PUSCH with the objective set to reach the target SINR.
  uint8_t compute_tpc_command(slot_point pusch_slot);

private:
  /// This variable defines how many PUSCH PRB allocations needs to be stored in the internal ring-buffer; it is the
  /// maximum expected delay (in slots) between the for which slot the PUSCH is scheduled and the slot at which the PHR
  /// is received; this delay depends on the PHY processing capabilities. For simplicity, we take round the number to a
  /// multiple of 10.
  static constexpr size_t MAX_PHR_IND_DELAY_SLOTS = 80;
  /// This variable defines a time window after a PUSCH transmission, in slots, in which TPC adjustments are forbidden.
  /// This is to prevent the PUSCH TPC to be adjusted too quickly, leading to oscillations in the SINR.
  //  [Implementation-defined] This value should be enough to guarantee that the CRC indication (reporting the PUSCH
  //  SINR) for the PUSCH with the latest power adjustment is received, before a new power adjustment is computed.
  static constexpr unsigned tpc_adjust_prohibit_time_ms = 40U;
  /// Minimum value for the closed-loop power control adjustment value. This is to avoid the power control going too
  /// low.
  static constexpr int min_f_cl_pw_control = -30;

  const rnti_t rnti;
  const bool   cl_pw_control_enabled;
  const bool   phr_bw_adaptation_enabled;
  const int    p0_nominal_pusch;
  // Configuration for the PUSCH power control.
  std::optional<pusch_config::pusch_power_control> pusch_pwr_ctrl;
  const ue_channel_state_manager&                  channel_state_manager;

  /// \brief Latest PHR received from the UE.
  struct ue_phr_report {
    struct phr_derived_data {
      /// This is the normalized value of the PH, which is the PH linear value multiplied by the number of PRBs and then
      /// converted to dB. The PH is the Power Headroom used for the PUSCH transmission within which the PHR was
      /// calculated.
      /// \remark The term "normalized" is not entirely correct, as we don't divide by the number of PRBs, but it is
      /// used for ease of terminology.
      float ph_normalized_pw_dB;
      /// The closed-loop power control adjustment value used for the PUSCH transmission that carried the PHR.
      int f_cl_pw_control;
      /// Estimate of the last path loss in dB, calculated based on the latest reported PHR.
      float latest_path_loss_db;
    };

    cell_ph_report                  phr;
    std::optional<phr_derived_data> phr_data_for_pw_ctrl;
  };

  /// PUSCH SINR target that would be achieved with closed-loop power control at the reference path-loss.
  const float pusch_sinr_target_dB;
  /// Reference path-loss, in dB, for which the UE would achieve the target SINR \c pusch_sinr_target_dB through
  /// closed-loop power control.
  const float    ref_path_loss_for_target_sinr;
  const unsigned tpc_adjust_prohibit_time_sl;

  /// Latest PHR received from the UE.
  std::optional<ue_phr_report> latest_phr;

  /// \brief Latest Power control adjustment value used for PUSCH for this UE.
  /// With reference to TS 38.213, Section 7.1.1, this is the value of \f$f_{b, f, c}(i, l)\f$ for the only BWP,
  /// for the only carrier and for this cell used for the latest transmitted PUSCH.
  struct pusch_pw_control {
    int        f_cl_pw_control;
    slot_point latest_tpc_slot;
  };
  std::optional<pusch_pw_control> latest_pusch_pw_control = std::nullopt;

  /// \brief Entry for PUSCH power-control and PHR related parameters.
  struct pusch_pw_ctrl_data {
    slot_point slot_rx;
    unsigned   nof_prbs;
    /// \brief Power control adjustment value at the last PUSCH slot before slot_rx.
    /// With reference to TS 38.213, Section 7.1.1, this is the value of \f$f_{b, f, c}(i, l)\f$ for the only BWP,
    /// for the only carrier and for this cell, at the last PUSCH opportunity before the slot_rx.
    int f_pw_control;
  };
  /// \brief Ring of PUSCH power allocation parameters indexed by slot.
  circular_array<pusch_pw_ctrl_data, get_allocator_ring_size_gt_min(MAX_PHR_IND_DELAY_SLOTS)> pusch_pw_ctrl_grid;

  srslog::basic_logger& logger;
};

} // namespace srsran
