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

/// Set to true for enabling radio unit trace.
#ifndef SRSRAN_RU_TRACE
constexpr bool RU_TRACE_ENABLED = false;
#else
constexpr bool RU_TRACE_ENABLED = true;
#endif

/// RU event tracing. This tracer is used to analyze latencies in the RU processing.
extern file_event_tracer<RU_TRACE_ENABLED> ru_tracer;

} // namespace srsran
