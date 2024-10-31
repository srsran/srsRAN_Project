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

// This recorder will be re-initialized later, when the duration of a slot is known.
using file_late_tracer = srsran::file_event_tracer<srsran::L2_LATE_TRACE_ENABLED>;
std::array<srsran::rusage_trace_recorder<file_late_tracer>, srsran::MAX_NOF_DU_CELLS> srsran::l2_late_tracer{
    file_late_tracer{},
    std::chrono::microseconds::max(),
    0};