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

#include "logical_channel_config.h"

namespace srsgnb {
namespace config_helpers {

/// \brief Creates a default logical channel configuration to be used by the scheduler.
logical_channel_config create_default_logical_channel_config(lcid_t lcid);

} // namespace config_helpers
} // namespace srsgnb