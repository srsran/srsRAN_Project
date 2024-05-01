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

#include "srsran/du/du_wrapper.h"
#include "srsran/du/du_wrapper_config.h"
#include <memory>

namespace srsran {

/// Instantiates a Distributed Unit (DU) wrapper object with the given configuration.
std::unique_ptr<du_wrapper> make_du_wrapper(const du_wrapper_config& du_cfg);

} // namespace srsran
