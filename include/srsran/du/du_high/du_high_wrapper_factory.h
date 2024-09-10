/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 *
 */

#pragma once

#include "srsran/du/du_high/du_high_wrapper.h"
#include "srsran/du/du_high/du_high_wrapper_config.h"
#include <memory>

namespace srsran {
namespace srs_du {

/// Creates a DU high wrapper with the given configuration and dependencies.
std::unique_ptr<du_high_wrapper> make_du_high_wrapper(const du_high_wrapper_config&  config,
                                                      du_high_wrapper_dependencies&& dependencies);

} // namespace srs_du
} // namespace srsran
