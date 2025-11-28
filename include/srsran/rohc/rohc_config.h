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

#include "srsran/support/srsran_assert.h"
#include "fmt/std.h"
#include <cstdint>

namespace srsran::rohc {

/// \brief Type of header compression.
///
/// The types of header compression choices are listed in TS 38.331.
enum class rohc_type_t : uint8_t { rohc, uplink_only_rohc };

constexpr const char* to_string(rohc_type_t rohc_type)
{
  constexpr const char* rohc_type_names[] = {"rohc", "uplink_only_rohc"};
  return rohc_type_names[static_cast<uint8_t>(rohc_type)];
}

/// \brief ROHC profile selector.
///
/// The value maps each profile to the bit of the bitmap "ROHC profiles" defined in TS 38.463 Sec. 9.3.1.40 starting
/// with 0 (LSB 0) for profile0x0001.
///
/// Note: profile0x0000 (ROHCv1 uncompressed, RFC 5795) is always implicitly enabled, because one CID shall always be
/// reserved for uncompressed flows (TS 38.323 Sec. 5.7.3).
enum class rohc_profile : uint8_t {
  profile0x0001 = 0, ///< ROHCv1 RTP/UDP/IP, RFC 3095, RFC 4815.
  profile0x0002 = 1, ///< ROHCv1 UDP/IP, RFC 3095, RFC 4815.
  profile0x0003 = 2, ///< ROHCv1 ESP/IP, RFC 3095, RFC 4815.
  profile0x0004 = 3, ///< ROHCv1 IP, RFC 3095, RFC 4815.
  profile0x0006 = 4, ///< ROHCv1 TCP/IP, RFC 6846.
  profile0x0101 = 5, ///< ROHCv2 RTP/UDP/IP, RFC 5225.
  profile0x0102 = 6, ///< ROHCv2 UDP/IP, RFC 5225.
  profile0x0103 = 7, ///< ROHCv2 ESP/IP, RFC 5225.
  profile0x0104 = 8, ///< ROHCv2 IP, RFC 5225.
};

/// \brief List of all ROHC profiles enumerated in \c rohc_profile.
constexpr static const rohc_profile all_rohc_profiles[] = {
    rohc_profile::profile0x0001,
    rohc_profile::profile0x0002,
    rohc_profile::profile0x0003,
    rohc_profile::profile0x0004,
    rohc_profile::profile0x0006,
    rohc_profile::profile0x0101,
    rohc_profile::profile0x0102,
    rohc_profile::profile0x0103,
    rohc_profile::profile0x0104,
};

/// \brief Translate ROHC profile into textual description.
constexpr const char* to_string(rohc_profile profile)
{
  constexpr const char* profile_names[] = {"v1-RTP/UDP/IP", // profile0x0001
                                           "v1-UDP/IP",     // profile0x0002
                                           "v1-ESP/IP",     // profile0x0003
                                           "v1-IP",         // profile0x0004
                                           "v1-TCP/IP",     // profile0x0006
                                           "v2-RTP/UDP/IP", // profile0x0101
                                           "v2-UDP/IP",     // profile0x0102
                                           "v2-ESP/IP",     // profile0x0103
                                           "v2-IP"};        // profile0x0104
  return profile_names[static_cast<uint8_t>(profile)];
}

/// \brief Translate ROHC profile into number (see \c rohc_profile).
constexpr const uint8_t to_number(rohc_profile profile)
{
  return static_cast<uint8_t>(profile);
}

/// \brief ROHC profile configuration.
///
/// This class holds the information on configured ROHC profiles as defined in TS 38.323 Sec. 5.7.1.
///
/// Note: profile0x0000 (ROHCv1 uncompressed, RFC 5795) is always implicitly enabled, because one CID shall always be
/// reserved for uncompressed flows (TS 38.323 Sec. 5.7.3).
class rohc_profile_config
{
public:
  /// \brief get_profile_bitmap Get the bitmap of configured ROHC profiles.
  ///
  /// The extracted bitmap corresponds the bitmap "ROHC profiles" defined in TS 38.463 Sec. 9.3.1.40.
  ///
  /// \return The bitmap of configured ROHc profiles.
  uint16_t get_profile_bitmap() const { return profile_bitmap; }

  /// \brief set_profile_bitmap Set the bitmap of configured ROHC profiles.
  ///
  /// The provided bitmap is interpreted according to "ROHC profiles" defined in TS 38.463 Sec. 9.3.1.40.
  ///
  /// \param bitmap The bitmap of configured ROHC profiles.
  void set_profile_bitmap(uint16_t bitmap)
  {
    srsran_assert(bitmap <= 0x1ff, "Invalid ROHC profile bitmap={:#x} exceeds 0x1ff", bitmap);
    profile_bitmap = bitmap;
  }

  /// \brief is_profile_enabled Query whether a ROHC profile is configured.
  ///
  /// \param profile The profile to query.
  /// \return True if the profile is enabled, false if the profile is disabled.
  bool is_profile_enabled(rohc_profile profile) const { return profile_bitmap & (1 << to_number(profile)); }

  /// \brief set_profile Enable or disable a ROHC profile.
  ///
  /// \param profile The profile to configure.
  /// \param enable Enable (true) or disable (false) the profile.
  void set_profile(rohc_profile profile, bool enable)
  {
    if (enable) {
      profile_bitmap |= (1 << to_number(profile));
    } else {
      profile_bitmap &= ~(1 << to_number(profile));
    }
  }

private:
  /// Bitmap that specifies which ROHC profiles are enabled (1) or disabled (0). Offsets are defined in \c rohc_profile.
  ///
  /// The profile0x0000 (uncompressed) is not included because it is always implicitly enabled.
  uint16_t profile_bitmap = 0;
};

/// Default maximum CID (Default 15 defined in TS 38.331 Sec. 6.3.2).
constexpr static uint16_t default_rohc_max_cid = 15;

/// Header compression config used by PDCP.
struct rohc_config {
  /// Type of header compression.
  rohc_type_t rohc_type = rohc_type_t::rohc;
  /// Maximum CID.
  uint16_t max_cid = default_rohc_max_cid;
  /// Configured ROHC profiles.
  rohc_profile_config profiles = {};
  /// Continue ROHC.
  bool continue_rohc = false;
};

} // namespace srsran::rohc

//
// Formatters
//
namespace fmt {

template <>
struct formatter<srsran::rohc::rohc_type_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rohc::rohc_type_t rohc_type, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", srsran::rohc::to_string(rohc_type));
  }
};

template <>
struct formatter<srsran::rohc::rohc_profile> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rohc::rohc_profile profile, FormatContext& ctx) const
  {
    return format_to(ctx.out(), "{}", srsran::rohc::to_string(profile));
  }
};

template <>
struct formatter<srsran::rohc::rohc_profile_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rohc::rohc_profile_config profile_config, FormatContext& ctx) const
  {
    auto out   = ctx.out();
    bool first = true;
    for (auto profile : srsran::rohc::all_rohc_profiles) {
      if (profile_config.is_profile_enabled(profile)) {
        out   = format_to(out, "{}{}", first ? "" : ",", profile);
        first = false;
      }
    }
    return out;
  }
};

template <>
struct formatter<srsran::rohc::rohc_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::rohc::rohc_config cfg, FormatContext& ctx) const
  {
    return format_to(ctx.out(),
                     "rohc_type={} max_cid={} profiles={} continue_rohc={}",
                     cfg.rohc_type,
                     cfg.max_cid,
                     cfg.profiles,
                     cfg.continue_rohc);
  }
};

} // namespace fmt
