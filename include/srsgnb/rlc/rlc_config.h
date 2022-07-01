/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_CONFIG_H
#define SRSGNB_RLC_CONFIG_H

#include <cstdint>
#include <string>

namespace srsgnb {

/// RLC NR modes
enum class rlc_mode { tm, um_bidir, um_unidir_ul, um_unidir_dl, am };
inline const char* to_string(const rlc_mode& mode)
{
  constexpr static const char* options[] = {"TM", "UM Bi-dir", "UM Uni-dir-UL", "UM Uni-dir-DL", "AM"};
  return options[static_cast<unsigned>(mode)];
}

/// RLC UM NR sequence number field
enum class rlc_um_sn_size : uint16_t { size6bits = 6, size12bits = 12 };
constexpr uint16_t to_number(const rlc_um_sn_size& sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

/// RLC AM NR sequence number field
enum class rlc_am_sn_size : uint16_t { size12bits = 12, size18bits = 18 };

///
/// \brief Converts sequence number field to numeric its value
/// \param sn_size sequence number size
/// \return numeric value of the sequence number field
///
constexpr uint16_t to_number(const rlc_am_sn_size& sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

///
/// \brief Returns the value range of the sequence numbers
/// \param sn_size Length of the serial number field in bits
/// \return cardinality of sn_size
///
constexpr uint32_t cardinality(const uint16_t sn_size)
{
  return (1 << sn_size);
}

/// RLC AM NR segmentation info
enum class rlc_dc_field : unsigned { control = 0b00, data = 0b01 };
constexpr unsigned to_number(const rlc_dc_field& dc)
{
  return static_cast<unsigned>(dc);
}

/// RLC AM NR segmentation info
enum class rlc_si_field : unsigned {
  full_sdu                       = 0b00,
  first_segment                  = 0b01,
  last_segment                   = 0b10,
  neither_first_nor_last_segment = 0b11
};

constexpr uint16_t to_number(const rlc_si_field& si_field)
{
  return static_cast<uint16_t>(si_field);
}

inline std::string to_string(const rlc_si_field& si)
{
  constexpr static const char* options[] = {"full", "first", "last", "middle"};
  return options[to_number(si)];
}

enum class rlc_control_pdu_type : unsigned { status_pdu = 0b000 };
constexpr uint16_t to_number(const rlc_control_pdu_type& type)
{
  return static_cast<uint16_t>(type);
}

inline std::string to_string(const rlc_control_pdu_type& type)
{
  constexpr static const char* options[] = {"Control PDU"};
  return options[to_number(type)];
}

/// Configuration of RLC bearer.
struct rlc_config {
  rlc_mode mode;
};

} // namespace srsgnb
#endif
