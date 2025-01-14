/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/tracing/event_tracing.h"

namespace srsran {

/// Set to true for enabling OFH trace.
#ifndef SRSRAN_OFH_TRACE
constexpr bool OFH_TRACE_ENABLED = false;
#else
constexpr bool OFH_TRACE_ENABLED = true;
#endif

/// OFH event tracing. This tracer is used to analyze latencies in the OFH processing.
extern file_event_tracer<OFH_TRACE_ENABLED> ofh_tracer;

} // namespace srsran
