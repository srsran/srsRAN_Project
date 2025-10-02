/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/rnti.h"

namespace srsran {

/// Sounding Reference Signals (SRS) reception context.
class srs_context
{
public:
  /// Default constructor.
  srs_context() = default;

  /// Constructs from values.
  explicit srs_context(unsigned sector_id_, rnti_t rnti_) : sector_id(sector_id_), rnti(rnti_) {}

private:
  friend struct fmt::formatter<srs_context>;
  unsigned sector_id = 0;
  rnti_t   rnti      = rnti_t::MAX_CRNTI;
};

} // namespace srsran
