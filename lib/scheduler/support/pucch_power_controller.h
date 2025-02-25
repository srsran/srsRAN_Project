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

#include "../../../include/srsran/ran/pucch/pucch_uci_bits.h"
#include "srsran/adt/circular_array.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/resource_grid_util.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/math/exponential_averager.h"
#include <optional>

namespace srsran {

class ue_cell_configuration;

class pucch_power_controller
{
public:
  pucch_power_controller(const ue_cell_configuration& ue_cell_cfg_);

  /// Save the PUCCH power control configuration.
  void reconfigure(const ue_cell_configuration& ue_cell_cfg);

  /// Update the PUCCH SINR, only for Format 0 or 1.
  void update_pucch_sinr_f0_f1(slot_point slor_rx, float sinr_db);

  /// Update the PUCCH SINR, only for Format 2, 3 or 4.
  void update_pucch_sinr_f2_f3_f4(slot_point slor_rx, float sinr_db, bool has_harq_bits, bool has_csi_bits);

  /// Save the PUCCH power control parameters at the slot when the PUCCH is expected to be transmitted.
  void update_pucch_pw_ctrl_state(slot_point      slot,
                                  pucch_format    format,
                                  unsigned        nof_prbs,
                                  unsigned        nof_symb,
                                  pucch_uci_bits& uci_bits,
                                  bool            intraslot_freq_hopping,
                                  bool            pi_2_bpsk       = false,
                                  bool            additional_dmrs = false);

  /// Compute the TPC command for the PUCCH with the objective set to reach the target SINR for Format 0/2/3. This
  /// function doesn't consider FOrmat 1 and 4 for closed-loop power control based on target SINR.
  uint8_t compute_tpc_command(slot_point pucch_slot);

private:
  /// This variable defines the length of the internal ring-buffer in slots, where we store the PUCCH power control
  /// entries; it is the maximum expected delay (in slots) between the for which slot the PUCCH is scheduled and the UCI
  /// indication; this delay depends on the PHY processing capabilities. For simplicity, we take round the number to a
  /// multiple of 10.
  static constexpr size_t MAX_UCI_IND_DELAY_SLOTS = 80;
  /// This variable defines a time window after a PUCCH transmission, in slots, in which TPC adjustments are forbidden.
  /// This is to prevent the PUCCH TPC to be adjusted too quickly, leading to oscillations in the SINR.
  //  [Implementation-defined] This value should be enough to guarantee that the UCI indication (reporting the PUSCH
  //  SINR) for the PUSCH with the latest power adjustment is received, before a new power adjustment is computed.
  static constexpr unsigned tpc_adjust_prohibit_time_ms = 40U;
  /// Interval of allowed closed-loop power control adjustment values. This is to avoid the power control going too
  /// low or high.
  static constexpr interval<int> g_cl_pw_control_bounds{-30, 12};
  /// Defines the alpha value for the exponential moving average of the PUCCH SINR.
  static constexpr float alpha_ema_sinr = 0.5f;
  /// This is maximum number of PUCCH grants per UE per slot that can be scheduled by the GNB.
  static constexpr size_t MAX_SCHED_PUCCH_GRANTS_PER_UE = 3;

  uint8_t get_tpc(float sinr_to_target_diff);

  const rnti_t rnti;
  const bool   cl_pw_control_enabled;
  const float  pucch_f0_sinr_target_dB;
  const float  pucch_f2_sinr_target_dB;
  const float  pucch_f3_sinr_target_dB;
  /// PUCCH power control configuration; this is required to enable PUCCH power control.
  std::optional<pucch_power_control> pucch_pwr_ctrl;

  const unsigned tpc_adjust_prohibit_time_sl;

  /// Time-averaged PUCCH SINR, in dB, for PUCCH Format 0/1.
  exp_average_fast_start<float> pucch_f0_f1_sinr_dB;
  /// Time-averaged PUCCH SINR, in dB, for PUCCH Format 2/3/4.
  exp_average_fast_start<float> pucch_f2_f3_f4_sinr_dB;

  pucch_format format_set_0 = pucch_format::NOF_FORMATS;
  pucch_format format_set_1 = pucch_format::NOF_FORMATS;

  /// \brief Latest Power control adjustment value used for PUCCH for this UE.
  /// With reference to TS 38.213, Section 7.2.1, this is the value of \f$g_{b, f, c}(i, l)\f$ for the only BWP,
  /// for the only carrier and for this cell used for the latest transmitted PUCCH.
  struct pucch_pw_control {
    int        g_cl_pw_control;
    slot_point latest_tpc_slot;
  };
  std::optional<pucch_pw_control> latest_pucch_pw_control = std::nullopt;

  /// \brief Entry for PUCCH power-control parameters.
  struct pucch_pw_ctrl_data {
    pucch_pw_ctrl_data(slot_point      sl,
                       unsigned        nof_prbs_,
                       pucch_format    format_,
                       pucch_uci_bits& uci_bits_,
                       float           delta_tf_) :
      slot_rx(sl), nof_prbs(nof_prbs_), format(format_), uci_bits(uci_bits_), delta_tf(delta_tf_)
    {
    }

    slot_point     slot_rx  = slot_point{};
    unsigned       nof_prbs = 0U;
    pucch_format   format   = pucch_format::NOF_FORMATS;
    pucch_uci_bits uci_bits = {};
    float          delta_tf = 0.0f;
  };

  using pucch_grants_pw_ctrl = static_vector<pucch_pw_ctrl_data, MAX_SCHED_PUCCH_GRANTS_PER_UE>;
  pucch_grants_pw_ctrl pucch_grants;
  /// \brief Ring of PUCCH power allocation parameters indexed by slot.
  circular_array<pucch_grants_pw_ctrl, get_allocator_ring_size_gt_min(MAX_UCI_IND_DELAY_SLOTS)> pucch_pw_ctrl_grid;

  srslog::basic_logger& logger;
};

} // namespace srsran
