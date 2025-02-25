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

namespace srsran {

template <typename T>
class config_object_pool;

/// \brief View of a configuration object.
///
/// The configuration object pool will try to ensure that each config_ptr<T> is unique within the whole cell group.
/// For this reason, config_ptr<T> objects can be compared via pointer comparison.
template <typename T>
class config_ptr
{
public:
  config_ptr() = default;

  /// If a configuration object is stored.
  bool has_value() const { return obj != nullptr; }

  /// Reset the stored object.
  void reset() { obj = nullptr; }

  /// Access stored config object.
  const T& operator*() const { return *obj; }
  const T& value() const { return *obj; }
  const T* operator->() const { return obj; }

  // Note: Comparison made via pointer comparison. The assumption is that the object is unique.
  bool operator==(const config_ptr& rhs) const { return obj == rhs.obj; }
  bool operator!=(const config_ptr& rhs) const { return not(*this == rhs); }

private:
  friend class config_object_pool<T>;

  // Private constructor to prevent creation of config_ptr<T> from an object that is not managed by a pool.
  config_ptr(const T& obj_) : obj(&obj_) {}

  const T* obj = nullptr;
};

} // namespace srsran
