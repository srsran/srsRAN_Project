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

#include "dl_logical_channel_manager.h"
#include "srsran/adt/static_vector.h"
#include "srsran/mac/time_alignment_group_config.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/slot_point.h"
#include <cstdint>

namespace srsran {

/// Timing Advance manager for an UE.
class ta_manager
{
public:
  explicit ta_manager(subcarrier_spacing ul_scs_, dl_logical_channel_manager* dl_lc_ch_mgr_);

  /// \brief Handles Timing Advance adaptation related tasks at slot indication.
  void slot_indication(slot_point current_sl);

  /// \brief Handles N_TA update indication.
  void handle_ul_n_ta_update_indication(uint8_t tag_id, int64_t n_ta_diff_);

private:
  /// Measurement time interval in nof. slots.
  static const unsigned MEAS_TIME_INTERVAL_IN_SLOTS = 20;
  /// Timing Advance Command (T_A) offset threshold above which Timing Advance Command is triggered.
  /// Values {0,...,32}.
  static const uint8_t T_A_OFFSET_THRESHOLD = 2;

  /// State of the Timing Advance manager.
  enum class state_t {
    /// Waiting for start order to perform measurements.
    idle,
    /// Performing measurements.
    measure,
  };

  /// \brief Computes the average of N_TA update measurements.
  int64_t compute_avg_n_ta_difference(uint8_t tag_id);

  /// \brief Computes new Timing Advance Command value (T_A) as per TS 38.213, clause 4.2.
  /// \return Timing Advance Command value. Values [0,...,63].
  unsigned compute_new_t_a(int64_t n_ta_diff);

  /// \brief Resets stored N_TA update measurements and sets state to idle.
  void reset_measurements(uint8_t tag_id);

  /// Subcarrier spacing of UL BWP for which Timing Advance Command is applicable.
  subcarrier_spacing ul_scs;
  /// DL logical channel manager to push Timing Advance Command to UE.
  dl_logical_channel_manager* dl_lc_ch_mgr = nullptr;

  /// Starting point of the measurement interval.
  slot_point meas_start_time;
  /// List of N_TA update (N_TA_new - N_TA_old value in T_C units) measurements maintained per Timing Advance Group.
  /// The array index corresponds to TAG ID.
  std::array<static_vector<int64_t, MEAS_TIME_INTERVAL_IN_SLOTS>, MAX_NOF_TAGS> tag_n_ta_diff_measurements;
  /// State of the Timing Advance manager.
  state_t state;
};

} // namespace srsran
