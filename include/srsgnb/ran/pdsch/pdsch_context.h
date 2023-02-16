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
#include "srsgnb/ran/rnti.h"
#include "srsgnb/scheduler/harq_id.h"

namespace srsran {

/// PDSCH transmission context.
class pdsch_context
{
public:
  /// Default constructor.
  pdsch_context() = default;

  /// Constructs from value.
  explicit pdsch_context(harq_id_t h_id_) : h_id(h_id_) {}

private:
  friend struct fmt::formatter<pdsch_context>;
  harq_id_t h_id = INVALID_HARQ_ID;
};

} // namespace srsran
