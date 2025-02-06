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
