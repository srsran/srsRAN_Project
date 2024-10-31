/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/instrumentation/traces/du_traces.h"

srsran::file_event_tracer<srsran::L1_TRACE_ENABLED> srsran::l1_tracer;

srsran::file_event_tracer<srsran::L2_TRACE_ENABLED> srsran::l2_tracer;

static srsran::file_event_tracer<srsran::L2_LATE_TRACE_ENABLED> l2_late_tracer;

// This recorder will be re-initialized later, when the duration of a slot is known.
thread_local srsran::rusage_trace_recorder<decltype(l2_late_tracer)> srsran::l2_slot_thres_tracer{
    l2_late_tracer,
    std::chrono::microseconds::max(),
    0};