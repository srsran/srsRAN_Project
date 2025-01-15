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

/// Describes the base interface for baseband gateway interfaces.
class baseband_gateway_base
{
protected:
  /// Allow default constructor for child.
  baseband_gateway_base() = default;

public:
  /// Provides default destructor.
  virtual ~baseband_gateway_base() = default;

  /// Forbid copy constructor.
  baseband_gateway_base(const baseband_gateway_base& other) = delete;

  /// Forbid move constructor.
  baseband_gateway_base(const baseband_gateway_base&& other) = delete;

  /// Forbid copy assignment operator.
  baseband_gateway_base& operator=(const baseband_gateway_base&) = delete;

  /// Forbid move assignment operator.
  baseband_gateway_base& operator=(baseband_gateway_base&&) = delete;
};

} // namespace srsran
