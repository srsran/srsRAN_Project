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

#include "srsgnb/io_broker/io_broker.h"
#include <memory>

namespace srsgnb {

enum class io_broker_type { epoll, io_uring };

/// Creates an instance of an IO broker
std::unique_ptr<io_broker> create_io_broker(io_broker_type type);

} // namespace srsgnb
