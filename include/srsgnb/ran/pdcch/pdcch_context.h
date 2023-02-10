/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsgnb/ran/pdcch/search_space.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

/// PDCCH transmission context.
class pdcch_context
{
public:
  /// Default constructor.
  pdcch_context() = default;

private:
  friend struct fmt::formatter<pdcch_context>;
  rnti_t          rnti                 = INVALID_RNTI;
  search_space_id ss_id                = MAX_NOF_SEARCH_SPACES;
  unsigned        dci_format           = 0;
  unsigned        harq_feedback_timing = 0;
};

} // namespace srsgnb
