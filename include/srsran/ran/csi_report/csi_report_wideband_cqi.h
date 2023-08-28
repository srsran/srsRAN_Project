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
#include "srsran/adt/bounded_integer.h"
#include "srsran/ran/sch_mcs.h"

namespace srsran {

/// Channel Quality Indicator value.
using cqi_value = bounded_integer<uint8_t, 0, 15>;

/// Channel Quality Indicator type.
using csi_report_wideband_cqi_type = cqi_value;

} // namespace srsran