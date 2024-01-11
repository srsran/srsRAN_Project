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

constexpr bool UP_TRACE_ENABLED = false;

/// UP event tracing. This tracer is used to analyze latencies in the CU-UP/DU high processing
/// of the user plane traffic.
extern file_event_tracer<UP_TRACE_ENABLED> up_tracer;

} // namespace srsran
