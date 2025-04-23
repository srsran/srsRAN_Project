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

#include "../config/cell_configuration.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/sched_consts.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

/// Helper class for checking if paging conditions are satisfied for a given slot.
class paging_slot_helper
{
public:
  explicit paging_slot_helper(const cell_configuration& cell_cfg_, const sched_cell_configuration_request_message& msg);

  /// \brief Checks paging conditions for a given slot and Paging Occasion.
  ///
  /// \param[in] pdcch_slot Slot at which the paging scheduler is called.
  /// \param[in] i_s Index of the Paging Occasion.
  /// \return True if paging conditions are satisfied, false otherwise.
  bool is_paging_slot(slot_point pdcch_slot, unsigned i_s) const;

private:
  const cell_configuration& cell_cfg;

  /// Array of Type0-PDCCH CSS monitoring slots (1 per beam) that will be used for Paging scheduling if
  /// pagingSearchSpace is 0 [TS 38.213, Section 13].
  std::array<slot_point, MAX_NUM_BEAMS> type0_pdcch_css_slots;

  /// Array of Type2-PDCCH CSS monitoring slots that will be used for Paging scheduling if pagingSearchSpace > 0.
  /// NOTE1: nrofPDCCH-MonitoringOccasionPerSSB-InPO is always 1. See \c
  /// pcch_config::NR_OF_PDCCH_MONITORING_OCCASION_PER_SSB_IN_PO.
  /// NOTE2: Row number corresponds to SSB beam index and column number corresponds to Paging Occasion index (i_s).
  std::array<static_vector<slot_point, pcch_config::MAX_PO_PER_PF>, MAX_NUM_BEAMS> type2_pdcch_css_slots;
  /// Search Space configuration when pagingSearchSpace > 0, if configured.
  const search_space_configuration* ss_cfg = nullptr;

  /// \brief Helper function to precompute Type2-PDCCH Monitoring slots when pagingSearchSpace > 0.
  ///
  /// \param[in] scs_common SCS of PDCCH.
  void precompute_type2_pdcch_slots(subcarrier_spacing scs_common);

  /// \brief Checks paging conditions for a UE in SearchSpace > 0 i.e pagingSearchSpace > 0 in its active BWP config.
  ///
  /// \param[in] pdcch_slot Slot at which the paging scheduler is called.
  /// \param[in] i_s Index of the Paging Occasion.
  /// \return True if paging conditions are satisfied, false otherwise.
  bool is_paging_slot_in_search_space_id_gt_0(slot_point pdcch_slot, unsigned i_s) const;

  /// \brief Checks paging conditions for a UE in SearchSpace 0 when pagingSearchSpace is 0 in its active BWP config.
  ///
  /// \param[in] sl_point Slot at which the paging scheduler is called.
  /// \param[in] i_s Index of the Paging Occasion.
  /// \return True if paging conditions are satisfied, false otherwise.
  bool is_paging_slot_in_search_space0(slot_point pdcch_slot, unsigned i_s) const;
};

} // namespace srsran
