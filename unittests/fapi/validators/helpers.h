#ifndef SRSGNB_UNITTESTS_FAPI_VALIDATORS_TOOLS_H
#define SRSGNB_UNITTESTS_FAPI_VALIDATORS_TOOLS_H

#include "srsgnb/fapi/messages_builder.h"
#include <random>

namespace unittest {

static std::mt19937 gen(0);

/// Returns random values for the maintenance v3 basic parameters. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.4.2.4.
inline std::tuple<srsgnb::ssb_pattern_case, srsgnb::subcarrier_spacing, unsigned>
generate_random_maintenance_v3_basic_params()
{
  std::uniform_int_distribution<unsigned> enum_dist(0, 4);

  static constexpr std::array<unsigned, 3> lmax{4u, 8u, 64u};
  std::uniform_int_distribution<unsigned>  lmax_dist(0, 2);

  return {static_cast<srsgnb::ssb_pattern_case>(enum_dist(gen)),
          static_cast<srsgnb::subcarrier_spacing>(enum_dist(gen)),
          lmax[lmax_dist(gen)]};
}

/// Builds and returns a valid DL SSB pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.4.
inline srsgnb::fapi::dl_ssb_pdu build_valid_dl_ssb_pdu()
{
  srsgnb::fapi::dl_ssb_pdu                pdu;
  srsgnb::fapi::dl_ssb_pdu_builder        builder(pdu);
  std::uniform_int_distribution<unsigned> pci_dist(0, 1007);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> block_index_dist(0, 63);
  std::uniform_int_distribution<unsigned> subcarrier_offset_dist(0, 31);
  std::uniform_int_distribution<unsigned> offset_pointA_dist(0, 2199);
  std::uniform_int_distribution<unsigned> sib1_dist(0, 255);
  std::uniform_real_distribution<>        power_dist(-30.8, 69.5);
  auto pss_profile = static_cast<srsgnb::fapi::beta_pss_profile_type>(binary_dist(gen));
  builder.set_basic_parameters(
      pci_dist(gen), pss_profile, block_index_dist(gen), subcarrier_offset_dist(gen), offset_pointA_dist(gen));
  builder.set_bch_payload_phy_full(binary_dist(gen), sib1_dist(gen), binary_dist(gen), binary_dist(gen));
  const auto& v3 = generate_random_maintenance_v3_basic_params();
  builder.set_maintenance_v3_basic_parameters(std::get<0>(v3), std::get<1>(v3), std::get<2>(v3));
  // When the PSS to SSS NR profile is configured to use the field beta PSS Profile SSS generate random number,
  // otherwise set to the minimum value of the range, as per SCF-222 Section 3.4.2.4.
  builder.set_maintenance_v3_tx_power_info(
      power_dist(gen),
      ((pss_profile == srsgnb::fapi::beta_pss_profile_type::beta_pss_profile_sss) ? power_dist(gen) : -32768));

  return pdu;
}

/// Builds and returns a valid DL TTI request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.
inline srsgnb::fapi::dl_tti_request_message build_valid_dl_tti_request()
{
  srsgnb::fapi::dl_tti_request_message         msg;
  srsgnb::fapi::dl_tti_request_message_builder builder(msg);

  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);

  // :TODO: at this moment code does not support groups, the number of groups is set to 0.
  builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen), 0);

  // Manually add the ssb pdu to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type = srsgnb::fapi::dl_pdu_type::SSB;
  msg.pdus.back().ssb_pdu  = build_valid_dl_ssb_pdu();

  return msg;
}

} // namespace unittest

#endif // SRSGNB_UNITTESTS_FAPI_VALIDATORS_TOOLS_H