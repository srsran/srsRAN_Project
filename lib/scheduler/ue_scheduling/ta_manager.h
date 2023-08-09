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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/phy_time_unit.h"
#include "srsran/ran/slot_point.h"
#include <cstdint>

namespace srsran {

/// Timing Advance manager for an UE.
class ta_manager
{
public:
  explicit ta_manager(subcarrier_spacing ul_scs_);

  /// \brief Handles Timing Advance adaptation related tasks at slot indication.
  void slot_indication(slot_point current_sl);

  /// \brief Handles N_TA update indication.
  void handle_ul_n_ta_update_indication(slot_point rx_slot_, int64_t n_ta_diff_);

private:
  /// Measurement time interval in nof. slots.
  static const unsigned MEAS_TIME_INTERVAL_IN_SLOTS = 10;
  /// N_TA offset threshold in T_C units above which Timing Advance Command is triggered.
  /// A value of 1024 (numerology 0), 512 (numerology 1), 256 (numerology 2) equates to change in Timing Advance Command
  /// value (T_A) by 1 when plugged in equation found in TS 38.213, clause 4.2.
  static const unsigned N_TA_OFFSET_THRESHOLD = 256;

  /// State of the Timing Advance manager.
  enum class state_t {
    /// Waiting for start order to perform measurements.
    idle,
    /// Performing measurements.
    measure,
    /// Waiting to transmit Timing Advance Command MAC CE to UE. No measurements shall be stored.
    prohibit
  };

  /// N_TA update measurement.
  struct n_ta_update_measurement {
    /// Slot at which N_TA update was received.
    slot_point rx_slot;
    /// N_TA_new - N_TA_old value in T_C units (Basic time unit for NR, see TS 38.211).
    int64_t n_ta_diff;
  };

  /// \brief Computes the average of N_TA update measurements.
  int64_t compute_avg_n_ta_difference();

  /// \brief Computes new Timing Advance Command value (T_A) as per TS 38.213, clause 4.2.
  /// \return Timing Advance Command value. Values [0,...,63].
  unsigned compute_new_t_a(int64_t n_ta_diff);

  /// \brief Resets stored N_TA update measurements and sets state to idle.
  void reset_measurements();

  /// Subcarrier spacing of UL BWP for which Timing Advance Command is applicable.
  subcarrier_spacing ul_scs;

  /// Starting point of the measurement interval.
  slot_point meas_start_time;
  /// List of N_TA update measurements.
  static_vector<n_ta_update_measurement, MEAS_TIME_INTERVAL_IN_SLOTS> measurements;
  /// State of the Timing Advance manager.
  state_t state;
};

} // namespace srsran
