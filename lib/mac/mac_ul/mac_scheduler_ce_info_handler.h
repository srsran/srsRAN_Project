/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ul_bsr.h"
#include "srsran/mac/lcid_dl_sch.h"
#include "srsran/mac/phr_report.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// \brief Information and context relative to a decoded MAC CE BSR.
struct mac_bsr_ce_info {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  bsr_format      bsr_fmt;
  /// \brief List of BSR reports decoded from the MAC CE BSR for different LCG IDs.
  lcg_bsr_report_list lcg_reports;
};

/// \brief Command used by MAC to force the scheduling of an UL grant for a UE in the scheduler.
struct mac_ul_scheduling_command {
  du_cell_index_t cell_index;
  slot_point      sl_rx;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
};

/// \brief Command used by MAC to trigger the scheduling of a DL MAC CE for a UE.
struct mac_ce_scheduling_command {
  du_ue_index_t ue_index;
  lcid_dl_sch_t ce_lcid;
};

/// \brief Information and context relative to a decoded MAC CE PHR.
struct mac_phr_ce_info {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  phr_report      phr;
};

/// \brief Interface between MAC and scheduler that is used by MAC to forward MAC CE information and force UL grants.
class mac_scheduler_ce_info_handler
{
public:
  virtual ~mac_scheduler_ce_info_handler() = default;

  /// \brief Forward to scheduler any decoded UL BSRs for a given UE.
  virtual void handle_ul_bsr_indication(const mac_bsr_ce_info& bsr) = 0;

  /// \brief Force the UL grant scheduling for a given UE.
  virtual void handle_ul_sched_command(const mac_ul_scheduling_command& sched_cmd) = 0;

  /// \brief Command scheduling of DL MAC CE for a given UE.
  /// \param mac_ce DL MAC CE to be scheduled.
  virtual void handle_dl_mac_ce_indication(const mac_ce_scheduling_command& mac_ce) = 0;

  /// \brief Forward to scheduler any decoded UL PHRs for a given UE.
  virtual void handle_ul_phr_indication(const mac_phr_ce_info& phr) = 0;

  /// \brief Forward to scheduler any notification of a received MAC CRNTI CE.
  virtual void handle_crnti_ce_indication(du_ue_index_t old_ue_index, du_cell_index_t cell_index) = 0;
};

} // namespace srsran
