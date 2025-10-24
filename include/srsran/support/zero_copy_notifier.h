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

#pragma once

#include <memory>

namespace srsran {

/// Notifier of objects of type T. The interface uses a builder type to minimize the amount of copying.
template <typename T>
class zero_copy_notifier
{
protected:
  /// Deleter of builder that commits/notifies the filled object, when the builder goes out of scope.
  struct committer {
    zero_copy_notifier* parent;

    void operator()(T* ptr)
    {
      if (ptr != nullptr) {
        parent->commit(*ptr);
      }
    }
  };

public:
  /// Builder used to fill an object of type T and later commit it.
  using builder = std::unique_ptr<T, committer>;

  virtual ~zero_copy_notifier() = default;

  /// \brief This method will be called to start preparing a new object to notify.
  builder get_builder() { return builder{&get_next(), committer{this}}; }

protected:
  /// Internal method that retrieves the next pooled object of type T to be filled by the caller to get_builder.
  virtual T& get_next() = 0;

  /// \brief Method called when the object is filled and it is ready to be committed. This method will be automatically
  /// called when the builder goes out of scope.
  virtual void commit(T& ptr) = 0;
};

} // namespace srsran
