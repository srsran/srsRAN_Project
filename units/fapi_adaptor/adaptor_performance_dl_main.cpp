#include "srsgnb/fapi/messages_builder.h"
#include "srsgnb/fapi_adaptor/phy/ssb.h"
#include "srsgnb/support/test_utils.h"
#include <chrono>

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

/// Test to measure the performance converting SSB data structures from MAC -> FAPI -> PHY.
static void test_ssb_conversion_performance()
{
  static constexpr unsigned iterations = 10000;
  std::vector<unsigned>     results;
  results.reserve(iterations);
  for (unsigned i = 0; i != iterations; ++i) {
    // :TODO: Begin with the MAC structure when it is defined.
    dl_tti_request_message         msg;
    dl_tti_request_message_builder builder(msg);
    builder.add_ssb_pdu(1, beta_pss_profile_type::dB_0, 4, 3, 2);

    ssb_processor::pdu_t pdu;

    // Conversion block.
    auto start = std::chrono::high_resolution_clock::now();
    convert_ssb_fapi_to_phy(pdu, msg.pdus[0].ssb_pdu, msg.sfn, msg.slot, 2);
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
  // Try some random conversions.
  for (unsigned i = 0; i != 10; ++i)
    test_ssb_conversion_performance();

  fmt::print("Success\n");
  return 0;
}
