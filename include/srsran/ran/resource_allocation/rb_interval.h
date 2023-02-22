/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/interval.h"

namespace srsran {

/// Struct to express a {min,...,max} range of CRBs within a carrier.
struct crb_interval : public interval<unsigned> {
  using interval::interval;
};

/// Struct to express a {min,...,max} range of PRBs within a BWP.
struct prb_interval : public interval<unsigned> {
  using interval::interval;
};

} // namespace srsran

namespace fmt {

/// FMT formatter for prb_intervals.
template <>
struct formatter<srsran::prb_interval> : public formatter<srsran::interval<uint32_t>> {
};

/// FMT formatter for crb_intervals.
template <>
struct formatter<srsran::crb_interval> : public formatter<srsran::interval<uint32_t>> {
};

} // namespace fmt
