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

/// Notifier of objects of type T, used to minimize the number of copying.
template <typename T>
class zero_copy_notifier
{
protected:
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
  using builder = std::unique_ptr<T, committer>;

  virtual ~zero_copy_notifier() = default;

  /// \brief This method will be called to start preparing a new object to notify.
  builder get_builder() { return builder{get_next(), committer{this}}; }

protected:
  virtual T* get_next() = 0;

  virtual void commit(T& ptr) = 0;
};

} // namespace srsran
