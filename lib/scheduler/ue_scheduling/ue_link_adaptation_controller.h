/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cell/cell_configuration.h"
#include "math.h"
#include "ue_channel_state_manager.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/sch_mcs.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Outer-layer Link Adaptation (OLLA) algorithm, which modifies SNR thresholds used in the selection of MCS
/// to achieve a specified target BLER.
///
/// This technique was first proposed by A Sampath, P Sarath Kumar, JM Holtzman, in IEEE Vehicular Technology
/// Conference, vol. 2. On setting reverse link target sir in a cdma system, (1997).
class olla_controller
{
public:
  /// \brief Creats an OLLA algorithm instance.
  ///
  /// \param target_bler_ Target BLER to achieve. Values: [0, 0.5).
  /// \param snr_inc_step_db Constant increment for the SNR offset, in decibels (dB).
  /// \param max_snr_offset_db Maximum SNR offset amplitude, in dB.
  olla_controller(float target_bler_, float snr_inc_step_db, float max_snr_offset_db) :
    target_bler(target_bler_),
    delta_down(snr_inc_step_db),
    delta_up((1 - target_bler) * snr_inc_step_db / target_bler),
    max_snr_offset(max_snr_offset_db)
  {
    srsran_assert(target_bler >= 0 and target_bler < 0.5, "Invalid target BLER");
    // See Francisco Blanquez-Casado, Gerardo Gomez, Maria del Carmen Aguayo-Torres and Jose Tomas Entrambasaguas,
    // "eOLLA: an enhanced outer loop link adaptation for cellular networks", EURASIP Journal on Wireless
    // Communications and Networking, 2016.
    srsran_assert(delta_down < 2 * std::exp(1) * target_bler / 1.03,
                  "Convergence guarantees of the OLLA algorithm have not been met");
  }

  /// \brief Update SNR offset with ACK/CRC feedback.
  float update(bool ack, sch_mcs_index used_mcs, interval<sch_mcs_index, true> mcs_boundaries)
  {
    // The OLLA algorithm does not account for the saturation points of the MCS. For instance, if we applied the OLLA
    // formula when MCS=28, and average BLER=0, the SNR offset would increase without bounds. As a workaround, we
    // forbid increases in the SNR offset for MCS=maxMCS, and decreases in the SNR offset when MCS=minMCS.
    float eff_delta_down = used_mcs >= mcs_boundaries.stop() ? 0 : delta_down;
    float eff_delta_up   = used_mcs <= mcs_boundaries.start() ? 0 : delta_up;
    current_offset += ack ? eff_delta_down : -eff_delta_up;
    current_offset = std::max(std::min(current_offset, max_snr_offset), -max_snr_offset);
    return current_offset;
  }

  /// \brief Offset to apply to SNR estimate.
  float offset_db() const { return current_offset; }

private:
  /// Target BLER to achieve. Values: [0, 0.5).
  float target_bler;
  /// Delta down and up variables of the OLLA algorithm.
  float delta_down, delta_up;
  /// Maximum SNR offset amplitude, in dB.
  float max_snr_offset;

  /// Current SNR offset in dB, to apply on top of the estimated SNR.
  float current_offset = 0;
};

class ue_link_adaptation_controller
{
public:
  ue_link_adaptation_controller(const cell_configuration& cell_cfg_, const ue_channel_state_manager& ue_channel_state) :
    cell_cfg(cell_cfg_), ue_ch_st(ue_channel_state)
  {
  }

  /// \brief Update DL link quality with the latest DL HARQ feedback.
  void handle_dl_ack_info(mac_harq_ack_report_status ack_value, sch_mcs_index used_mcs, pdsch_mcs_table mcs_table);

  /// \brief Update UL link quality with the latest UL CRC feedback.
  void handle_ul_crc_info(bool crc, sch_mcs_index used_mcs, pusch_mcs_table mcs_table);

  /// \brief Get the effective CQI to be used for MCS derivation.
  cqi_value get_effective_cqi() const;

private:
  const cell_configuration&       cell_cfg;
  const ue_channel_state_manager& ue_ch_st;

  optional<olla_controller> dl_olla;
  optional<olla_controller> ul_olla;
};

} // namespace srsran