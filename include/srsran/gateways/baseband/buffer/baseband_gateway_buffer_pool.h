/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief Baseband gateway buffer pool definition.
///
/// The baseband gateway buffer pool wraps dynamic buffers in a buffer pool. The pool and its contents are created
/// during the initialization of the application.
///
/// The pool provides a baseband buffer smart pointer type that can be forwarded among different interfaces. When the
/// smart pointer is destroyed, the buffer returns to the pool.

#pragma once

#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// Thread safe baseband buffer pool.
using baseband_gateway_buffer_pool = bounded_unique_object_pool<baseband_gateway_buffer_dynamic>;

/// Smart pointer for managing baseband buffers.
using baseband_gateway_buffer_ptr = baseband_gateway_buffer_pool::ptr;

} // namespace srsran
