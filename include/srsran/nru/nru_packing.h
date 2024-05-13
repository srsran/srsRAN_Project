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

#include "srsran/adt/byte_buffer.h"
#include "srsran/nru/nru_message.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// NR-U PDU type field
///
/// Ref: TS 38.425 Sec. 5.5.3.1
enum class nru_pdu_type : uint8_t {
  dl_user_data            = 0, ///< DL user data (PDU type 0)
  dl_data_delivery_status = 1, ///< DL data delivery status (PDU type 1)
  assistance_information  = 2, ///< Assistance information (PDU type 2)
  reserved                = 3  ///< Reserved for future use (PDU type 3-15)
};

/// Convert NR-U PDU type to unsigned integer.
constexpr inline uint8_t nru_pdu_type_to_uint(nru_pdu_type pdu_type)
{
  return static_cast<uint8_t>(pdu_type);
}

/// Convert unsigned integer to NR-U PDU type.
constexpr inline nru_pdu_type uint_to_nru_pdu_type(uint8_t num)
{
  return static_cast<nru_pdu_type>(num);
}

/// Packing and unpacking of NR user plane protocol (NR-U) messages
///
/// Ref: TS 38.425
class nru_packing
{
public:
  nru_packing(srslog::basic_logger& logger_) : logger(logger_) {}

  static nru_pdu_type get_pdu_type(byte_buffer_view container);

  bool unpack(nru_dl_user_data& dl_user_data, byte_buffer_view container) const;
  bool pack(byte_buffer& out_buf, const nru_dl_user_data& dl_user_data) const;

  bool unpack(nru_dl_data_delivery_status& dl_data_delivery_status, byte_buffer_view container) const;
  bool pack(byte_buffer& out_buf, const nru_dl_data_delivery_status& dl_data_delivery_status) const;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran

namespace fmt {

// NR-U PDU type field formatter
template <>
struct formatter<srsran::nru_pdu_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::nru_pdu_type pdu_type, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"dl_user_data", "dl_data_delivery_status", "assistance_information"};
    if (nru_pdu_type_to_uint(pdu_type) < nru_pdu_type_to_uint(srsran::nru_pdu_type::reserved)) {
      format_to(ctx.out(), "{}", options[nru_pdu_type_to_uint(pdu_type)]);
    }
    return format_to(ctx.out(), "reserved ({})", nru_pdu_type_to_uint(pdu_type));
  };
};
} // namespace fmt
