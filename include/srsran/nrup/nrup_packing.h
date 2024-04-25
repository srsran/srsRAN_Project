/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/nrup/nrup_message.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// NRUP PDU type field
///
/// Ref: TS 38.425 Sec. 5.5.3.1
enum class nrup_pdu_type : uint8_t {
  dl_user_data            = 0, ///< DL user data (PDU type 0)
  dl_data_delivery_status = 1, ///< DL data delivery status (PDU type 1)
  assistance_information  = 2, ///< Assistance information (PDU type 2)
  reserved                = 3  ///< Reserved for future use (PDU type 3-15)
};

/// Convert NRUP PDU type to unsigned integer.
constexpr inline uint8_t nrup_pdu_type_to_uint(nrup_pdu_type pdu_type)
{
  return static_cast<uint8_t>(pdu_type);
}

/// Convert unsigned integer to NRUP PDU type.
constexpr inline nrup_pdu_type uint_to_nrup_pdu_type(uint8_t num)
{
  return static_cast<nrup_pdu_type>(num);
}

/// Packing and unpacking of NR user plane protocol messages
///
/// Ref: TS 38.425
class nrup_packing
{
public:
  nrup_packing(srslog::basic_logger& logger_) : logger(logger_) {}

  nrup_pdu_type get_pdu_type(byte_buffer_view container) const;

  bool unpack(nru_dl_user_data& dl_user_data, byte_buffer_view container) const;
  bool pack(byte_buffer& out_buf, const nru_dl_user_data& dl_user_data) const;

  bool unpack(nru_dl_data_delivery_status& dl_data_delivery_status, byte_buffer_view container) const;
  bool pack(byte_buffer& out_buf, const nru_dl_data_delivery_status& dl_data_delivery_status) const;

private:
  srslog::basic_logger& logger;
};

} // namespace srsran

namespace fmt {

// NRUP PDU type field formatter
template <>
struct formatter<srsran::nrup_pdu_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::nrup_pdu_type pdu_type, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"dl_user_data", "dl_data_delivery_status", "assistance_information"};
    if (nrup_pdu_type_to_uint(pdu_type) < nrup_pdu_type_to_uint(srsran::nrup_pdu_type::reserved)) {
      format_to(ctx.out(), "{}", options[nrup_pdu_type_to_uint(pdu_type)]);
    }
    return format_to(ctx.out(), "reserved ({})", nrup_pdu_type_to_uint(pdu_type));
  };
};
} // namespace fmt
