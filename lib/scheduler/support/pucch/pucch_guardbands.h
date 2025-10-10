/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../config/cell_configuration.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"

namespace srsran {

crb_bitmap compute_pucch_crbs(const cell_configuration& cell_cfg);

} // namespace srsran
