/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

/// \file
/// \brief Configuration structs passed to scheduler implementation.

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/pdcch/aggregation_level.h"
#include "srsgnb/ran/pdsch/pdsch_mcs.h"
#include "srsgnb/ran/sch_mcs.h"
#include "srsgnb/ran/sib_configuration.h"

namespace srsgnb {

/// \brief UE scheduling statically configurable expert parameters.
struct scheduler_ue_expert_config {
  optional<sch_mcs_index> fixed_dl_mcs;
  unsigned                initial_cqi;
  optional<sch_mcs_index> fixed_ul_mcs;
  unsigned                max_nof_harq_retxs;
  /// Maximum MCS index that can be assigned when scheduling MSG4.
  sch_mcs_index max_msg4_mcs;
  /// Maximum consecutive PUSCH KOs, before scheduler de-prioritizes UE.
  unsigned max_consecutive_pusch_kos;
  /// Initial UL SINR value used for Dynamic UL MCS computation (in dB).
  double initial_ul_sinr;
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
  std::chrono::milliseconds      metrics_report_period;
};

} // namespace srsgnb