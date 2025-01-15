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

#include "srsran/support/tracing/event_tracing.h"

namespace srsran {

/// Set to true for enabling user-plane related traces.
#ifndef SRSRAN_UP_TRACE
constexpr bool UP_TRACE_ENABLED = false;
#else
constexpr bool UP_TRACE_ENABLED = true;
#endif

/// UP event tracing. This tracer is used to analyze latencies in the CU-UP/DU high processing
/// of the user plane traffic.
extern file_event_tracer<UP_TRACE_ENABLED> up_tracer;

} // namespace srsran
