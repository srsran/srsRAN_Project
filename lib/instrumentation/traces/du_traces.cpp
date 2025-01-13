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

#include "srsran/instrumentation/traces/du_traces.h"

srsran::file_event_tracer<srsran::L1_TRACE_ENABLED> srsran::l1_tracer;

srsran::file_event_tracer<srsran::L2_TRACE_ENABLED> srsran::l2_tracer;

// This recorder will be re-initialized later, when the duration of a slot is known.
using file_late_tracer = srsran::file_event_tracer<srsran::L2_LATE_TRACE_ENABLED>;
std::array<srsran::rusage_trace_recorder<file_late_tracer>, srsran::MAX_NOF_DU_CELLS> srsran::l2_late_tracer{
    file_late_tracer{},
    std::chrono::microseconds::max(),
    0};