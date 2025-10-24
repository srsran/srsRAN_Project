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

#include "dl_logical_channel_manager.h"
#include "srsran/ran/slot_point.h"
#include "srsran/ran/time_alignment_config.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

/// Timing Advance manager for an UE.
class ta_manager
{
public:
  explicit ta_manager(const scheduler_ue_expert_config& expert_cfg_,
                      subcarrier_spacing                ul_scs_,
                      time_alignment_group::id_t        pcell_tag_id,
                      dl_logical_channel_manager*       dl_lc_ch_mgr_);

  void update_tags(span<const time_alignment_group::id_t> tag_ids);

  /// \brief Handles Timing Advance adaptation related tasks at slot indication.
  void slot_indication(slot_point current_sl);

  /// \brief Handles N_TA update indication.
  void handle_ul_n_ta_update_indication(time_alignment_group::id_t tag_id, int64_t n_ta_diff_, float ul_sinr);

private:
  /// TA command offset for a zero value.
  static constexpr int ta_cmd_offset_zero = 31;

  /// State of the Timing Advance manager.
  enum class state_t {
    /// Waiting for start order to perform measurements.
    idle,
    /// Performing measurements.
    measure,
    /// Prohibit state.
    prohibit,
    /// Disabled.
    disabled,
  };

  struct tag_measurement {
    time_alignment_group::id_t tag_id;
    std::vector<int64_t>       samples;
  };

  /// \brief Computes the average of N_TA update measurements.
  int64_t compute_avg_n_ta_difference(unsigned tag_idx);

  /// \brief Computes new Timing Advance Command value (T_A) as per TS 38.213, clause 4.2.
  /// \return Timing Advance Command value. Values [0,...,63].
  unsigned compute_new_t_a(int64_t n_ta_diff);

  /// \brief Resets stored N_TA update measurements.
  void reset_measurements(unsigned tag_idx);

  /// Subcarrier spacing of UL BWP for which Timing Advance Command is applicable.
  const subcarrier_spacing ul_scs;
  /// DL logical channel manager to push Timing Advance Command to UE.
  dl_logical_channel_manager* dl_lc_ch_mgr = nullptr;
  /// Expert config parameters used for UE scheduler.
  const scheduler_ue_expert_config& expert_cfg;
  srslog::basic_logger&             logger;

  /// Starting point of the measurement interval.
  slot_point meas_start_time;
  /// Starting point of the prohibit interval.
  slot_point prohibit_start_time;
  /// List of N_TA update (N_TA_new - N_TA_old value in T_C units) measurements maintained per Timing Advance Group.
  /// The array index corresponds to TAG ID. And, the corresponding array value (i.e. vector) holds N_TA update
  /// measurements for that TAG ID.
  std::vector<tag_measurement> n_ta_reports;
  /// State of the Timing Advance manager.
  state_t state;
};

} // namespace srsran
