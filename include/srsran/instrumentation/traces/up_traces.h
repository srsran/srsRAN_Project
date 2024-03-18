/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/support/event_tracing.h"

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
