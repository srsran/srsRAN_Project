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

#include "gtpu_demux.h"
#include <memory>

namespace srsgnb {

/// Creates an instance of an GTP-U demux object.
std::unique_ptr<gtpu_demux> create_gtpu_demux();

} // namespace srsgnb