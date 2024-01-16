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

#include <cstdint>
#include <fmt/core.h>
#include <limits>

namespace srsran {

/// Identifies transmit and receive buffers.
class trx_buffer_identifier
{
  /// Internal identifier type.
  using id_type = uint32_t;

  /// Invalid identifier.
  static constexpr id_type invalid_id = std::numeric_limits<id_type>::max();
  /// Unknown identifier.
  static constexpr id_type unknown_id = invalid_id - 1;

  /// Converts RNTI and HARQ identifiers to the internal identifier.
  static constexpr uint32_t to_id(uint16_t rnti, uint8_t harq)
  {
    return (static_cast<id_type>(rnti) << 8U) | static_cast<id_type>(harq);
  }

  /// Create from an internal identifier.
  constexpr trx_buffer_identifier(uint32_t id_) : id(id_) {}

public:
  /// Get an invalid buffer identifier.
  static constexpr trx_buffer_identifier invalid() { return invalid_id; }

  /// Get unknown buffer identifier.
  static constexpr trx_buffer_identifier unknown() { return unknown_id; }

  /// Default constructor - creates an invalid identifier.
  explicit trx_buffer_identifier() noexcept : id(invalid_id) {}

  /// Constructor from RNTI and HARQ process identifier.
  explicit constexpr trx_buffer_identifier(uint16_t rnti, uint8_t harq) noexcept : id(to_id(rnti, harq)) {}

  /// Equal comparison to other identifier.
  constexpr bool operator==(const trx_buffer_identifier& other) const { return other.id == id; }

  /// Not equal comparison with another identifier.
  constexpr bool operator!=(const trx_buffer_identifier& other) const { return !(*this == other); }

  /// Gets the RNTI.
  uint16_t get_rnti() const { return static_cast<uint16_t>(id >> 8); }

  /// Gets the HARQ process identifier.
  uint8_t get_harq() const { return static_cast<uint8_t>(id & 0xff); }

private:
  /// Internal identifier.
  id_type id;
};

} // namespace srsran

namespace fmt {

/// Default formatter for trx_buffer_identifier.
template <>
struct formatter<srsran::trx_buffer_identifier> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::trx_buffer_identifier& value, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "rnti={:#x} h_id={}", value.get_rnti(), value.get_harq());
  }
};

} // namespace fmt