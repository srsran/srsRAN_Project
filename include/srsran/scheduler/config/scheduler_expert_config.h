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

#pragma once

/// \file
/// \brief Configuration structs passed to scheduler implementation.

#include "srsran/adt/interval.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdsch/pdsch_mcs.h"
#include "srsran/ran/sch_mcs.h"
#include "srsran/ran/sib_configuration.h"

namespace srsran {

/// \brief UE scheduling statically configurable expert parameters.
struct scheduler_ue_expert_config {
  /// Range of allowed MCS indices for DL UE scheduling. To use a fixed mcs, set the minimum mcs equal to the maximum.
  interval<sch_mcs_index, true> dl_mcs;
  /// Sequence of redundancy versions used for PDSCH scheduling. Possible values: {0, 1, 2, 3}.
  std::vector<uint8_t> pdsch_rv_sequence;
  /// Range of allowed MCS indices for UL UE scheduling. To use a fixed mcs, set the minimum mcs equal to the maximum.
  interval<sch_mcs_index, true> ul_mcs;
  /// Sequence of redundancy versions used for PUSCH scheduling. Possible values: {0, 1, 2, 3}.
  std::vector<uint8_t> pusch_rv_sequence;
  unsigned             initial_cqi;
  unsigned             max_nof_harq_retxs;
  /// Maximum MCS index that can be assigned when scheduling MSG4.
  sch_mcs_index max_msg4_mcs;
  /// Initial UL SINR value used for Dynamic UL MCS computation (in dB).
  double initial_ul_sinr;
  /// Enable multiplexing of CSI-RS and PDSCH.
  bool enable_csi_rs_pdsch_multiplexing;
};

/// \brief System Information scheduling statically configurable expert parameters.
struct scheduler_si_expert_config {
  /// As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for SIB1.
  sch_mcs_index     sib1_mcs_index;
  aggregation_level sib1_dci_aggr_lev;
  /// SIB1 retx period.
  sib1_rtx_periodicity sib1_retx_period;
};

/// \brief Random Access scheduling statically configurable expert parameters.
struct scheduler_ra_expert_config {
  sch_mcs_index rar_mcs_index;
  sch_mcs_index msg3_mcs_index;
  unsigned      max_nof_msg3_harq_retxs;
};

/// \brief Paging scheduling statically configurable expert parameters.
struct scheduler_paging_expert_config {
  /// As per TS 38.214, Section 5.1.3.1, only an MCS with modulation order 2 allowed for Paging.
  sch_mcs_index     paging_mcs_index;
  aggregation_level paging_dci_aggr_lev;
  unsigned          max_paging_retries;
};

/// \brief Scheduling statically configurable expert parameters.
struct scheduler_expert_config {
  scheduler_si_expert_config     si;
  scheduler_ra_expert_config     ra;
  scheduler_paging_expert_config pg;
  scheduler_ue_expert_config     ue;
  bool                           log_broadcast_messages;
  std::chrono::milliseconds      metrics_report_period;
};

} // namespace srsran