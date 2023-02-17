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

namespace srsran {

/// PDCCH transmission context.
class pdcch_context
{
public:
  /// Default constructor.
  pdcch_context() = default;

  explicit pdcch_context(search_space_id ss_id_, const char* dci_format_, optional<unsigned> harq_feedback_timing_) :
    ss_id(ss_id_), dci_format(dci_format_), harq_feedback_timing(harq_feedback_timing_)
  {
  }

private:
  friend struct fmt::formatter<pdcch_context>;
  search_space_id    ss_id      = MAX_NOF_SEARCH_SPACES;
  const char*        dci_format = nullptr;
  optional<unsigned> harq_feedback_timing;
};

} // namespace srsran
