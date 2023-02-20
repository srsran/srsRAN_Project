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
#include "srsran/ran/rnti.h"
#include "srsran/scheduler/harq_id.h"

namespace srsran {

/// PUSCH transmission context.
class pusch_context
{
public:
  /// Default constructor.
  pusch_context() = default;

  /// Constructs from values.
  explicit pusch_context(rnti_t rnti_, harq_id_t h_id_) : rnti(rnti_), h_id(h_id_) {}

private:
  friend struct fmt::formatter<pusch_context>;
  rnti_t    rnti = INVALID_RNTI;
  harq_id_t h_id = INVALID_HARQ_ID;
};

} // namespace srsran
