/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

namespace srsran {

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

} // namespace srsran
