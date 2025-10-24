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

#include "srsran/ran/du_types.h"
#include "srsran/support/tracing/rusage_trace_recorder.h"

namespace srsran {

/// Set to true for enabling layer 1 downlink trace.
#ifndef SRSRAN_L1_DL_TRACE
constexpr bool L1_DL_TRACE_ENABLED = false;
#else
constexpr bool L1_DL_TRACE_ENABLED = true;
#endif

/// Set to true for enabling layer 1 uplink trace.
#ifndef SRSRAN_L1_UL_TRACE
constexpr bool L1_UL_TRACE_ENABLED = false;
#else
constexpr bool L1_UL_TRACE_ENABLED = true;
#endif

/// Set to true for enabling layer 2 trace.
#ifndef SRSRAN_L2_TRACE
constexpr bool L2_TRACE_ENABLED = false;
#else
constexpr bool L2_TRACE_ENABLED = true;
#endif

/// L1 downlink event tracing. This tracer is used to analyze latencies in the L1 downlink processing.
extern file_event_tracer<L1_DL_TRACE_ENABLED || L1_UL_TRACE_ENABLED> l1_common_tracer;

/// L1 downlink event tracing. This tracer is used to analyze latencies in the L1 downlink processing.
extern file_event_tracer<L1_DL_TRACE_ENABLED> l1_dl_tracer;

/// L1 uplink event tracing. This tracer is used to analyze latencies in the L1 uplink processing.
extern file_event_tracer<L1_UL_TRACE_ENABLED> l1_ul_tracer;

/// L2 event tracing. This tracer is used to analyze latencies in the L2 processing of slot indications.
extern file_event_tracer<L2_TRACE_ENABLED> l2_tracer;

} // namespace srsran
