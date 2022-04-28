#include "srsgnb/fapi/messages_builder.h"
#include "srsgnb/fapi_adaptor/phy/ssb.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/support/test_utils.h"
#include <chrono>
#include <random>

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

/// Benchmark that measures the performance converting SSB data structures from MAC -> FAPI -> PHY.
static void ssb_conversion_test()
{
  static constexpr std::array<unsigned, 3> LMAX{4u, 8u, 64u};
  static constexpr unsigned                ITERATIONS = 10000;

  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 5);
  std::uniform_int_distribution<unsigned> pci_dist(0, 3000);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> block_index_dist(0, LMAX.back() - 1);
  std::uniform_int_distribution<unsigned> subcarrier_offset_dist(0, 15);
  std::uniform_int_distribution<unsigned> offset_pointA_dist(0, 2199);
  std::uniform_int_distribution<unsigned> sib1_dist(0, 255);
  std::uniform_real_distribution<>        power_dist(-30.8, 69.5);
  std::uniform_int_distribution<unsigned> enum_dist(0, 4);
  std::uniform_int_distribution<unsigned> lmax_dist(0, 2);

  for (unsigned iteration = 0; iteration != ITERATIONS; ++iteration) {
    unsigned              sfn                   = sfn_dist(gen);
    unsigned              slot                  = slot_dist(gen);
    unsigned              pci                   = pci_dist(gen);
    beta_pss_profile_type beta_pss              = static_cast<beta_pss_profile_type>(binary_dist(gen));
    unsigned              lmax                  = LMAX[lmax_dist(gen)];
    unsigned              ssb_idx               = block_index_dist(gen) % lmax;
    unsigned              ssb_subcarrier_offset = subcarrier_offset_dist(gen);
    unsigned              offset_pointA         = offset_pointA_dist(gen);
    ssb_pattern_case      pattern_case          = static_cast<ssb_pattern_case>(enum_dist(gen));
    subcarrier_spacing    scs                   = static_cast<subcarrier_spacing>(enum_dist(gen));

    // :TODO: Begin with the MAC structure when it is defined.
    dl_tti_request_message         msg = {};
    dl_tti_request_message_builder builder(msg);
    // :TODO: when the groups are available, add them.
    builder.set_basic_parameters(sfn, slot, 0);
    auto ssb_builder = builder.add_ssb_pdu(pci, beta_pss, ssb_idx, ssb_subcarrier_offset, offset_pointA);

    ssb_builder.set_bch_payload_phy_full(binary_dist(gen), sib1_dist(gen), binary_dist(gen), binary_dist(gen));
    ssb_builder.set_maintenance_v3_basic_parameters(pattern_case, scs, lmax);

    float power_scaling_ss_pbch_dB = power_dist(gen);
    float pss_to_sss_ratio_dB      = power_dist(gen);
    ssb_builder.set_maintenance_v3_tx_power_info(power_scaling_ss_pbch_dB, pss_to_sss_ratio_dB);

    // PHY processor PDU.
    ssb_processor::pdu_t pdu;

    // Conversion block.
    convert_ssb_fapi_to_phy(pdu, msg.pdus[0].ssb_pdu, msg.sfn, msg.slot, scs);

    // Assert contents.
    TESTASSERT_EQ(pdu.slot.sfn(), sfn);
    TESTASSERT_EQ(pdu.slot.slot_index(), slot);
    TESTASSERT_EQ(pdu.phys_cell_id, pci);
    switch (beta_pss) {
      case beta_pss_profile_type::dB_0:
        TESTASSERT_EQ(pdu.beta_pss, 0.0);
        break;
      case beta_pss_profile_type::dB_3:
        TESTASSERT_EQ(pdu.beta_pss, 3.0);
        break;
      case beta_pss_profile_type::beta_pss_profile_sss:
        TESTASSERT(std::abs(pdu.beta_pss - pss_to_sss_ratio_dB) < 0.01);
        break;
    }
    TESTASSERT_EQ(pdu.ssb_idx, ssb_idx);
    TESTASSERT_EQ(pdu.L_max, lmax);
    TESTASSERT_EQ(pdu.ssb_subcarrier_offset, ssb_subcarrier_offset);
    TESTASSERT_EQ(pdu.ssb_offset_pointA, offset_pointA);
    TESTASSERT_EQ(pdu.pattern_case, pattern_case);
    TESTASSERT(srsvec::equal(pdu.ports, std::vector<uint8_t>{0}));
  }
}

int main()
{
  ssb_conversion_test();

  fmt::print("Success\n");
  return 0;
}
