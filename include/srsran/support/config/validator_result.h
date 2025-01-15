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

#include "srsran/adt/expected.h"
#include <string>

namespace srsran {

/// This type represents the result of a validator function. In case of failure, it contains a string describing the
/// error. In case of success, the .has_value() method returns true.
using validator_result = error_type<std::string>;

} // namespace srsran
