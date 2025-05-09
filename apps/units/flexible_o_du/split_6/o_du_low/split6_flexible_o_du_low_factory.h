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

#include "split6_flexible_o_du_low_impl.h"
#include <memory>

namespace srsran {

/// Creates a split 6 flexible O-DU low.
std::unique_ptr<split6_flexible_o_du_low_impl> create_split6_flexible_o_du_low_impl();

} // namespace srsran
