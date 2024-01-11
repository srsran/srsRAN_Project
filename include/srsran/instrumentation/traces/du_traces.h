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

#include "srsran/support/event_tracing.h"

namespace srsran {

/// Set to true for enabling layer 1 trace.
constexpr bool L1_TRACE_ENABLED = false;

/// Set to true for enabling layer 2 trace.
constexpr bool L2_TRACE_ENABLED = false;

/// L1 event tracing. This tracer is used to analyze latencies in the L1 processing.
extern file_event_tracer<L1_TRACE_ENABLED> l1_tracer;

/// L2 event tracing. This tracer is used to analyze latencies in the L2 processing of slot indications.
extern file_event_tracer<L2_TRACE_ENABLED> l2_tracer;

} // namespace srsran