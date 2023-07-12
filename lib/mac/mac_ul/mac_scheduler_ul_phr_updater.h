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

#include "ul_phr.h"

namespace srsran {

/// \brief Information and context relative to a decoded MAC CE BSR.
struct mac_phr_ce_info {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
  phr_report      phr;
};

/// \brief Interface between MAC and scheduler that is used by MAC to forward decoded UL PHRs.
class mac_scheduler_ul_phr_updater
{
public:
  virtual ~mac_scheduler_ul_phr_updater() = default;

  /// \brief Forward to scheduler any decoded UL PHRs for a given UE.
  virtual void handle_ul_phr_indication(const mac_phr_ce_info& phr) = 0;
};

} // namespace srsran
