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

#include "srsran/du/du.h"
#include <memory>

namespace srsran {

/// \brief Instantiates a Distributed Unit (DU) object.
std::unique_ptr<du> make_du();

} // namespace srsran