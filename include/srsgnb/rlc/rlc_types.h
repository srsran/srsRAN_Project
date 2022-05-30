/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_RLC_CONFIG_H
#define SRSRAN_RLC_CONFIG_H

#include <cstdint>
#include <string>

namespace srsgnb {

///< RLC NR modes
enum class rlc_mode { tm, um_bidir, um_unidir_ul, um_unidir_dl, am };
inline const char* to_string(const rlc_mode& mode)
{
  constexpr static const char* options[] = {"TM", "UM Bi-dir", "UM Uni-dir-UL", "UM Uni-dir-DL", "AM"};
  return options[static_cast<unsigned>(mode)];
}

///< RLC UM NR sequence number field
enum class rlc_um_sn_size : uint16_t { size6bits = 6, size12bits = 12 };
constexpr uint16_t to_number(const rlc_um_sn_size& sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

///< RLC AM NR sequence number field
enum class rlc_am_sn_size : uint16_t { size12bits = 12, size18bits = 18 };
constexpr uint16_t to_number(const rlc_am_sn_size& sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

///< RLC AM NR segmentation info
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
} // namespace srsgnb
#endif
