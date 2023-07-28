/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/support/event_tracing.h"

namespace srsran {

/// L2 event tracing. This tracer is used to analyze latencies in the L2 processing of slot indications.
extern file_event_tracer<false> l2_tracer;

} // namespace srsran