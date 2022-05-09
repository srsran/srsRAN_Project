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

#ifndef SRSGNB_UNITTESTS_FAPI_VALIDATORS_HELPERS_H
#define SRSGNB_UNITTESTS_FAPI_VALIDATORS_HELPERS_H

#include "srsgnb/fapi/message_builders.h"
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
  // When the PSS to SSS NR profile is configured to use the field beta PSS Profile SSS then generate a random number,
  // otherwise leave it unset, as per SCF-222 Section 3.4.2.4.
  srsgnb::optional<float> beta_pss;
  if (pss_profile == srsgnb::fapi::beta_pss_profile_type::beta_pss_profile_sss) {
    beta_pss = power_dist(gen);
  }
  builder.set_maintenance_v3_tx_power_info(power_dist(gen), beta_pss);

  return pdu;
}

/// Builds and returns a valid DL PDCCH pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.4.
inline srsgnb::fapi::dl_pdcch_pdu build_valid_dl_pdcch_pdu()
{
  srsgnb::fapi::dl_pdcch_pdu         pdu;
  srsgnb::fapi::dl_pdcch_pdu_builder builder(pdu);

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);
  std::uniform_int_distribution<unsigned> duration_symbol_dist(0, 3);
  std::uniform_int_distribution<unsigned> shift_index_dist(0, 275);
  std::uniform_int_distribution<unsigned> n_rnti_dist(0, 65535);
  std::uniform_int_distribution<unsigned> cce_dist(0, 135);
  std::uniform_int_distribution<unsigned> aggregation_dist(0, 4);
  std::uniform_int_distribution<unsigned> nid_dmrs_dist(0, 65535);
  std::uniform_int_distribution<unsigned> nid_data_dist(0, 65535);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<int>      power_dist(-8, 8);
  std::uniform_int_distribution<unsigned> custom_dist(2, 3);

  // :TODO: generate  better frequency domain resource bitmap.
  srsgnb::static_vector<uint8_t, 6> freq_domain = {3, 2, 1, 4, 5, 1};

  // Always work with the biggest numerology.
  builder.set_bwp_parameters(bwp_size_dist(gen),
                             bwp_start_dist(gen),
                             srsgnb::subcarrier_spacing::kHz240,
                             static_cast<srsgnb::fapi::cyclic_prefix_type>(binary_dist(gen)));

  builder.set_coreset_parameters(start_symbol_index_dist(gen),
                                 duration_symbol_dist(gen),
                                 {freq_domain},
                                 static_cast<srsgnb::fapi::cce_to_reg_mapping_type>(binary_dist(gen)),
                                 custom_dist(gen),
                                 custom_dist(gen),
                                 static_cast<srsgnb::fapi::pdcch_coreset_type>(binary_dist(gen)),
                                 shift_index_dist(gen),
                                 static_cast<srsgnb::fapi::precoder_granularity_type>(binary_dist(gen)));

  // Add DCI.
  auto builder_dci = builder.add_dl_dci();

  builder_dci.set_basic_parameters(srsgnb::to_rnti(n_rnti_dist(gen) + 1),
                                   nid_data_dist(gen),
                                   n_rnti_dist(gen),
                                   cce_dist(gen),
                                   std::pow(2, aggregation_dist(gen)));
  srsgnb::optional<float> profile_nr;
  profile_nr.emplace(power_dist(gen));
  builder_dci.set_tx_power_info_parameter(profile_nr);

  // Payload.
  srsgnb::static_vector<uint8_t, 128> payload(32, 1);

  builder_dci.set_payload({payload});

  srsgnb::optional<float> profile_data;
  srsgnb::optional<float> profile_dmrs;
  builder_dci.set_maintenance_v3_dci_parameters(true, profile_dmrs, profile_data);
  builder_dci.set_parameters_v4_dci(nid_dmrs_dist(gen));

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

  // Manually add the SSB PDU to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type = srsgnb::fapi::dl_pdu_type::SSB;
  msg.pdus.back().ssb_pdu  = build_valid_dl_ssb_pdu();

  // Manually add the PDCCH PDU to reuse the functions above.
  msg.pdus.emplace_back();
  msg.pdus.back().pdu_type  = srsgnb::fapi::dl_pdu_type::PDCCH;
  msg.pdus.back().pdcch_pdu = build_valid_dl_pdcch_pdu();

  return msg;
}

} // namespace unittest

#endif // SRSGNB_UNITTESTS_FAPI_VALIDATORS_HELPERS_H
