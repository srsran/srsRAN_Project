/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

/// Describes the base interface for radio classes.
class radio_base
{
protected:
  /// Allow default constructor for child.
  radio_base() = default;

public:
  /// Provides default destructor.
  virtual ~radio_base() = default;

  /// Forbid copy constructor.
  radio_base(const radio_base& other) = delete;

  /// Forbid move constructor.
  radio_base(const radio_base&& other) = delete;

  /// Forbid copy assigment operator.
  radio_base& operator=(const radio_base&) = delete;

  /// Forbid move assigment operator.
  radio_base& operator=(radio_base&&) = delete;
};
} // namespace srsran
