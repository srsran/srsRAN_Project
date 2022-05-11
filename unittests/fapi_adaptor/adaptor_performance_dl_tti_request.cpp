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

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi_adaptor/phy/messages/pdcch.h"
#include "srsgnb/fapi_adaptor/phy/ssb.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/support/test_utils.h"
#include <chrono>
#include <random>

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

/// Returns random values for the maintenance v3 basic parameters.
static std::tuple<ssb_pattern_case, subcarrier_spacing, unsigned> generate_random_maintenance_v3_basic_params()
{
  std::uniform_int_distribution<unsigned> enum_dist(0, 4);

  static constexpr std::array<unsigned, 3> lmax{4u, 8u, 64u};
  std::uniform_int_distribution<unsigned>  lmax_dist(0, 2);

  return {static_cast<ssb_pattern_case>(enum_dist(gen)),
          static_cast<subcarrier_spacing>(enum_dist(gen)),
          lmax[lmax_dist(gen)]};
}

/// Benchmark that measures the performance converting SSB data structures from MAC -> FAPI -> PHY.
static void ssb_conversion_benchmark()
{
  static constexpr unsigned iterations = 10000;
  std::vector<unsigned>     results;
  results.reserve(iterations);

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 5);
  std::uniform_int_distribution<unsigned> pci_dist(0, 3000);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> block_index_dist(0, 3000);
  std::uniform_int_distribution<unsigned> subcarrier_offset_dist(0, 3000);
  std::uniform_int_distribution<unsigned> offset_pointA_dist(0, 3000);
  std::uniform_int_distribution<unsigned> sib1_dist(0, 255);
  std::uniform_real_distribution<>        power_dist(-30.8, 30.5);

  for (unsigned i = 0; i != iterations; ++i) {
    // :TODO: Begin with the MAC structure when it is defined.
    dl_tti_request_message         msg;
    dl_tti_request_message_builder builder(msg);
    // :TODO: when the groups are available, add them.
    builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen), 0);
    auto ssb_builder = builder.add_ssb_pdu(pci_dist(gen),
                                           static_cast<beta_pss_profile_type>(binary_dist(gen)),
                                           block_index_dist(gen),
                                           subcarrier_offset_dist(gen),
                                           offset_pointA_dist(gen));

    ssb_builder.set_bch_payload_phy_full(binary_dist(gen), sib1_dist(gen), binary_dist(gen), binary_dist(gen));
    const auto& v3 = generate_random_maintenance_v3_basic_params();
    ssb_builder.set_maintenance_v3_basic_parameters(std::get<0>(v3), std::get<1>(v3), std::get<2>(v3));
    ssb_builder.set_maintenance_v3_tx_power_info(power_dist(gen), power_dist(gen));

    ssb_processor::pdu_t pdu;

    // Conversion block.
    auto start = std::chrono::high_resolution_clock::now();
    convert_ssb_fapi_to_phy(pdu, msg.pdus[0].ssb_pdu, msg.sfn, msg.slot, std::get<1>(v3));
    auto end = std::chrono::high_resolution_clock::now();

    // Print how much time it took.
    results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
  }

  std::sort(results.begin(), results.end());

  fmt::print("SSB MAC -> FAPI -> PHY conversion Benchmark\n"
             "All values in nanoseconds\n"
             "Percentiles: | 50th | 75th | 90th | 99th | 99.9th | Worst |\n"
             "             |{:6}|{:6}|{:6}|{:6}|{:8}|{:7}|\n",
             results[static_cast<size_t>(results.size() * 0.5)],
             results[static_cast<size_t>(results.size() * 0.75)],
             results[static_cast<size_t>(results.size() * 0.9)],
             results[static_cast<size_t>(results.size() * 0.99)],
             results[static_cast<size_t>(results.size() * 0.999)],
             results.back());
}

static void pdcch_conversion_benchark()
{
  static constexpr unsigned iterations = 10000;
  std::vector<unsigned>     results;
  results.reserve(iterations);

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

  for (unsigned i = 0; i != iterations; ++i) {
    // :TODO: Begin with the MAC structure when it is defined.
    dl_tti_request_message         msg;
    dl_tti_request_message_builder builder(msg);
    // :TODO: when the groups are available, add them.
    builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen), 0);

    dl_pdcch_pdu_builder builder_pdcch = builder.add_pdcch_pdu();

    // :TODO: generate  better frequency domain resource bitmap.
    static_vector<uint8_t, 6> freq_domain = {3, 2, 1, 4, 5, 1};

    // Always work with the biggest numerology.
    builder_pdcch.set_bwp_parameters(bwp_size_dist(gen),
                                     bwp_start_dist(gen),
                                     subcarrier_spacing::kHz240,
                                     static_cast<cyclic_prefix_type>(binary_dist(gen)));

    builder_pdcch.set_coreset_parameters(start_symbol_index_dist(gen),
                                         duration_symbol_dist(gen),
                                         {freq_domain},
                                         static_cast<cce_to_reg_mapping_type>(binary_dist(gen)),
                                         custom_dist(gen),
                                         custom_dist(gen),
                                         static_cast<pdcch_coreset_type>(binary_dist(gen)),
                                         shift_index_dist(gen),
                                         static_cast<precoder_granularity_type>(binary_dist(gen)));

    // Add DCI.
    auto builder_dci = builder_pdcch.add_dl_dci();

    builder_dci.set_basic_parameters(
        to_rnti(0), nid_data_dist(gen), n_rnti_dist(gen), cce_dist(gen), aggregation_dist(gen));
    optional<float> profile_nr;
    profile_nr.emplace(power_dist(gen));
    builder_dci.set_tx_power_info_parameter(profile_nr);

    // Payload.
    static_vector<uint8_t, 128> payload(128,1);
    static_vector<uint8_t, 128> packed_payload;
    packed_payload.resize(std::ceil(payload.size() / 8.F));
    srsvec::bit_pack({packed_payload}, {payload});

    builder_dci.set_payload({packed_payload});

    optional<float> profile_data;
    optional<float> profile_dmrs;
    builder_dci.set_maintenance_v3_dci_parameters(true, profile_dmrs, profile_data);
    builder_dci.set_parameters_v4_dci(nid_dmrs_dist(gen));

    pdcch_processor::pdu_t proc_pdu;

    // Conversion block.
    auto start = std::chrono::high_resolution_clock::now();
    convert_pdcch_fapi_to_phy(proc_pdu, msg.pdus[0].pdcch_pdu, msg.sfn, msg.slot);
    auto end = std::chrono::high_resolution_clock::now();

    // Print how much time it took.
    results.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
  }

  std::sort(results.begin(), results.end());

  fmt::print("PDCCH MAC -> FAPI -> PHY conversion Benchmark\n"
             "All values in nanoseconds\n"
             "Percentiles: | 50th | 75th | 90th | 99th | 99.9th | Worst |\n"
             "             |{:6}|{:6}|{:6}|{:6}|{:8}|{:7}|\n",
             results[static_cast<size_t>(results.size() * 0.5)],
             results[static_cast<size_t>(results.size() * 0.75)],
             results[static_cast<size_t>(results.size() * 0.9)],
             results[static_cast<size_t>(results.size() * 0.99)],
             results[static_cast<size_t>(results.size() * 0.999)],
             results.back());
}

int main()
{
  ssb_conversion_benchmark();
  pdcch_conversion_benchark();

  fmt::print("Success\n");
  return 0;
}
