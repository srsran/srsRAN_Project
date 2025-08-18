/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/circular_array.h"
#include "srsran/ran/slot_point.h"
#include <mutex>
#include <utility>

namespace srsran {

/// \brief Thread-safe resource request pool.
///
/// Contains a pool of requests. A request consist of a slot point and a templated resource. Requests are indexed by
/// slots. The pool access is thread-safe and it is done by exchange requests.
///
/// \tparam ResourceType Resource type.
template <typename ResourceType>
class resource_request_pool
{
public:
  /// Maximum number of requests contained in the array.
  static constexpr unsigned request_array_size = 16;

  /// Internal storage type.
  struct request_type {
    slot_point   slot;
    ResourceType resource;
  };

  /// \brief Exchanges a request from the pool by the given one.
  /// \param[in] request The given request, it is copied into <tt>request.slot.system_slot() % REQUEST_ARRAY_SIZE</tt>.
  /// \return The previous request at position  <tt> request.slot.system_slot() % REQUEST_ARRAY_SIZE </tt>.
  request_type exchange(request_type request)
  {
    return requests[request.slot.system_slot()].exchange(std::move(request));
  }

private:
  /// Wraps the request in a thread-safe access.
  class request_wrapper
  {
  public:
    /// Default constructor - constructs an empty request.
    request_wrapper() : request({slot_point(), ResourceType()}) {}

    /// \brief Exchanges the previous request with a new request.
    /// \param[in] new_request New request.
    /// \return A copy of the previous request.
    request_type exchange(request_type new_request)
    {
      std::unique_lock<std::mutex> lock(mutex);
      request_type                 old_request = std::move(request);
      request                                  = std::move(new_request);
      return old_request;
    }

  private:
    request_type request;
    std::mutex   mutex;
  };

  /// Request storage, indexed by slots.
  circular_array<request_wrapper, request_array_size> requests;
};

} // namespace srsran
