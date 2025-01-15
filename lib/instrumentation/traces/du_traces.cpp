/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/adt/to_array.h"

srsran::file_event_tracer<srsran::L1_TRACE_ENABLED> srsran::l1_tracer;

srsran::file_event_tracer<srsran::L2_TRACE_ENABLED> srsran::l2_tracer;

// This recorder will be re-initialized later, when the duration of a slot is known.
using file_late_tracer   = srsran::file_event_tracer<srsran::L2_LATE_TRACE_ENABLED>;
using file_rusage_tracer = srsran::rusage_trace_recorder<file_late_tracer>;
std::array<file_rusage_tracer, srsran::MAX_NOF_DU_CELLS> srsran::l2_late_tracer =
    make_array<file_rusage_tracer, srsran::MAX_NOF_DU_CELLS>([](std::size_t /* unused */) {
      return file_rusage_tracer{"invalid", file_late_tracer{}, std::chrono::microseconds::max(), 0};
    });
