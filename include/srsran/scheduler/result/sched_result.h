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

#include "srsran/scheduler/result/csi_rs_info.h"
#include "srsran/scheduler/result/pdcch_info.h"
#include "srsran/scheduler/result/pdsch_info.h"
#include "srsran/scheduler/result/prach_info.h"
#include "srsran/scheduler/result/pucch_info.h"
#include "srsran/scheduler/result/pusch_info.h"
#include "srsran/scheduler/result/srs_info.h"

namespace srsran {

/// Scheduler decision made for DL in a given slot.
struct dl_sched_result {
  /// Number of DL symbols active for this slot.
  unsigned nof_dl_symbols;
  /// Allocated DL PDCCHs. Includes both SIB, RAR and Data PDCCHs.
  pdcch_dl_info_list dl_pdcchs;
  /// Allocated UL PDCCHs.
  pdcch_ul_info_list ul_pdcchs;
  /// Allocation of SSB and SIBs.
  dl_broadcast_allocation bc;
  /// Allocation of dedicated RARs.
  static_vector<rar_information, MAX_RAR_PDUS_PER_SLOT> rar_grants;
  /// Allocation of Paging messages.
  static_vector<dl_paging_allocation, MAX_PAGING_PDUS_PER_SLOT> paging_grants;
  /// Allocation of dedicated UE messages.
  static_vector<dl_msg_alloc, MAX_UE_PDUS_PER_SLOT> ue_grants;
  /// Allocation of CSI-RS messages.
  static_vector<csi_rs_info, MAX_CSI_RS_PDUS_PER_SLOT> csi_rs;
};

/// Scheduler decision made for UL in a given slot.
struct ul_sched_result {
  /// Number of UL symbols active for this slot.
  unsigned nof_ul_symbols;
  /// PUSCH grants allocated in the current slot.
  static_vector<ul_sched_info, MAX_PUSCH_PDUS_PER_SLOT> puschs;
  /// PRACH occasions within the given slot.
  static_vector<prach_occasion_info, MAX_PRACH_OCCASIONS_PER_SLOT> prachs;
  /// PUCCH grants allocated in the current slot.
  static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT> pucchs;
  /// SRS grants allocated in the current slot.
  static_vector<srs_info, MAX_SRS_PDUS_PER_SLOT> srss;
};

/// Failed allocation attempts in a given slot.
struct failed_alloc_attempts {
  /// Number of failed PDCCH allocation attempts.
  unsigned pdcch = 0;
  /// Number of failed UCI allocation attempts.
  unsigned uci = 0;
};

/// Scheduler decision made for DL and UL in a given slot.
struct sched_result {
  /// Whether the scheduling for the given slot was successful.
  bool success;
  /// DL allocations for the given slot.
  dl_sched_result dl;
  /// UL allocations for the given slot.
  ul_sched_result ul;
  /// Information about failed allocation attempts for the given slot.
  failed_alloc_attempts failed_attempts;
};

} // namespace srsran
