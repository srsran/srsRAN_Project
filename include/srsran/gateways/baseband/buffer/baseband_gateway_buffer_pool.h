/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
