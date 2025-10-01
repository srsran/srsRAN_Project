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

/// General event tracing for critical events such as real-time errors.
extern file_event_tracer<true> general_critical_tracer;

} // namespace srsran
