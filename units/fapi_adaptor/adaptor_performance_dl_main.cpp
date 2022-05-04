#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/fapi_adaptor/phy/ssb.h"
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

  fmt::print("MAC -> FAPI -> PHY conversion Benchmark\n"
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

  fmt::print("Success\n");
  return 0;
}
