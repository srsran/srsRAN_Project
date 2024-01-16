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

#include "srsran/adt/circular_array.h"
#include "srsran/ran/slot_point.h"
#include <mutex>
#include <utility>

namespace srsran {

/// \brief Thread-safe resource grid request pool.
///
/// Contains a pool of requests of slot points and resource grids indexed by slots. A request consist of a slot point a
/// resource grid. The pool access is thread-safe and it is done by exchange requests.
///
/// \tparam GridType Resource grid type.
template <typename GridType>
class resource_grid_request_pool
{
public:
  /// Internal storage type.
  struct request_type {
    slot_point slot;
    GridType*  grid;
  };

  /// \brief Exchanges a request from the pool by the given one.
  /// \param[in] request The given request, it is copied into <tt>request.slot.system_slot() % REQUEST_ARRAY_SIZE</tt>.
  /// \return The previous request at position  <tt> request.slot.system_slot() % REQUEST_ARRAY_SIZE </tt>.
  request_type exchange(const request_type& request) { return requests[request.slot.system_slot()].exchange(request); }

private:
  /// Number of requests contained in the array.
  static constexpr unsigned REQUEST_ARRAY_SIZE = 16;

  /// Wraps the request in a thread-safe access.
  class request_wrapper
  {
  public:
    /// Default constructor - constructs an empty request.
    request_wrapper() : request({slot_point(), nullptr}) {}

    /// \brief Exchanges the previous request with a new request.
    /// \param[in] new_request New request.
    /// \return A copy of the previous request.
    request_type exchange(const request_type& new_request)
    {
      std::unique_lock<std::mutex> lock(mutex);
      return std::exchange(request, new_request);
    }

  private:
    request_type request;
    std::mutex   mutex;
  };

  /// Request storage, indexed by slots.
  circular_array<request_wrapper, REQUEST_ARRAY_SIZE> requests;
};

} // namespace srsran
