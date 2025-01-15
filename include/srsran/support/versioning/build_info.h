/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

/// Returns a formatted string containing the current commit hash.
const char* get_build_hash();

/// Returns a formatted string containing the current commit hash and branch name (if available).
const char* get_build_info();

/// Returns the selected (CMake) build mode as string.
const char* get_build_mode();

} // namespace srsran
