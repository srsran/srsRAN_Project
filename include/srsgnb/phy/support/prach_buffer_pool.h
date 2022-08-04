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

#include <vector>

namespace srsgnb {

class prach_buffer;

/// \brief Pool of PRACH buffers.
///
/// As the pool owns the PRACH buffers that it stores, the lifespan of the buffers is determined by lifespan of the
/// pool.
///
/// \note The pool must ensure that, when assigning a PRACH buffer upon request, the buffer is not currently
/// being used.
class prach_buffer_pool
{
public:
  /// Default destructor.
  virtual ~prach_buffer_pool() = default;

  /// Returns an idle PRACH buffer from the pool.
  virtual prach_buffer& get_prach_buffer() = 0;
};

/// Returns a pool for the given PRACH buffers.
std::unique_ptr<prach_buffer_pool> create_prach_buffer_pool(std::vector<std::unique_ptr<prach_buffer>>&& elements);

} // namespace srsgnb
