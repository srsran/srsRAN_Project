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

#include "srsgnb/adt/interval.h"

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
