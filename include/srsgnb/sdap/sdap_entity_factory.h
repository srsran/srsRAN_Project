/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/sdap/sdap_entity.h"
#include <memory>

/// This factory header file depends on the SDAP entity interfaces (see above include list). It is kept separate as
/// clients of the SDAP entity interfaces dont need to call factory methods.

namespace srsgnb {

/// Creates an instance of a SDAP entity interface.
std::unique_ptr<sdap_entity> create_sdap_entity();

} // namespace srsgnb
