/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
