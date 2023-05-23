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

#include "ul_bsr.h"
#include "srsran/adt/variant.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// \brief Information relative to a decoded MAC CE BSR.
struct mac_bsr_ce_info {
  du_cell_index_t                          cell_index;
  du_ue_index_t                            ue_index;
  rnti_t                                   rnti;
  bsr_format                               bsr_fmt;
  variant<lcg_bsr_report, long_bsr_report> report;

  const lcg_bsr_report&  get_sbsr() const { return variant_get<0>(report); }
  const long_bsr_report& get_lbsr() const { return variant_get<1>(report); }
};

/// \brief Command used by MAC to force the scheduling of an UL grant for a UE in the scheduler.
struct mac_ul_scheduling_command {
  du_cell_index_t cell_index;
  slot_point      sl_rx;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
};

/// \brief Adapter interface between MAC and scheduler that allows the srsGNB MAC to configure different scheduler
/// implementations.
/// Note: This adapter interface makes scheduler configuration methods awaitable.
class mac_scheduler_ul_buffer_state_updater
{
public:
  virtual ~mac_scheduler_ul_buffer_state_updater() = default;

  /// \brief Forward to scheduler any decoded UL BSRs for a given UE.
  virtual void handle_ul_bsr_indication(const mac_bsr_ce_info& bsr) = 0;

  /// \brief Force the UL grant scheduling for a given UE.
  virtual void handle_ul_sched_command(const mac_ul_scheduling_command& sched_cmd) = 0;
};

} // namespace srsran