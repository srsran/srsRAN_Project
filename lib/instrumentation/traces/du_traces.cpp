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

srsran::file_event_tracer<srsran::L1_DL_TRACE_ENABLED> srsran::l1_common_tracer;

srsran::file_event_tracer<srsran::L1_DL_TRACE_ENABLED> srsran::l1_dl_tracer;

srsran::file_event_tracer<srsran::L1_UL_TRACE_ENABLED> srsran::l1_ul_tracer;

srsran::file_event_tracer<srsran::L2_TRACE_ENABLED> srsran::l2_tracer;
