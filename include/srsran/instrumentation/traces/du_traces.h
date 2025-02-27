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

#include "srsran/ran/du_types.h"
#include "srsran/support/tracing/rusage_trace_recorder.h"

namespace srsran {

/// Set to true for enabling layer 1 trace.
#ifndef SRSRAN_L1_TRACE
constexpr bool L1_TRACE_ENABLED = false;
#else
constexpr bool L1_TRACE_ENABLED      = true;
#endif

/// Set to true for enabling layer 2 trace.
#ifndef SRSRAN_L2_TRACE
constexpr bool L2_TRACE_ENABLED = false;
#else
constexpr bool L2_TRACE_ENABLED      = true;
#endif
/// L1 event tracing. This tracer is used to analyze latencies in the L1 processing.
extern file_event_tracer<L1_TRACE_ENABLED> l1_tracer;

/// L2 event tracing. This tracer is used to analyze latencies in the L2 processing of slot indications.
extern file_event_tracer<L2_TRACE_ENABLED> l2_tracer;

/// Set to true for enabling layer 2 tracing when slowdowns are detected.
#ifndef SRSRAN_L2_LATE_TRACE
constexpr bool L2_LATE_TRACE_ENABLED = false;
#else
constexpr bool L2_LATE_TRACE_ENABLED = true;
#endif

} // namespace srsran
