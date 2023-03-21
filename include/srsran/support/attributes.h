/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

// The warn_unused_result attribute causes a warning to be emitted if a caller of the function with this attribute does
// not use its return value.
#define RETURN_NO_DISCARD __attribute__((warn_unused_result))

} // namespace srsran