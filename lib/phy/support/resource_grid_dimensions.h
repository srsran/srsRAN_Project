/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/tensor.h"

namespace srsran {

/// Dimension, i.e. number of coordinates, of each indexing level of the resource grid.
enum class resource_grid_dimensions : unsigned { subc = 0, symbol = 1, port = 2, all = 3 };

} // namespace srsran
