/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;
using namespace fapi;

static std::mt19937 gen(0);

static void test_rach_indication_builder_ok()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> handle_dist(0, 1023);
  std::uniform_int_distribution<unsigned> symb_id_dist(0, 13);
  std::uniform_int_distribution<unsigned> slot_id_dist(0, 79);
  std::uniform_int_distribution<unsigned> ra_id_dist(0, 7);
  std::uniform_int_distribution<int>      rssi_dist(-150, 40);
  std::uniform_int_distribution<int>      rsrp_dist(-150, 5);
  std::uniform_int_distribution<int>      snr_dist(-70, 70);
  std::uniform_int_distribution<unsigned> timing_dist(0, 3846);
  std::uniform_int_distribution<unsigned> timing_ns_dist(0, 3000000);

  for (unsigned i = 0; i != 10; ++i) {
    rach_indication_message         msg;
    rach_indication_message_builder builder(msg);

    unsigned slot         = sfn_dist(gen);
    unsigned sfn          = slot_dist(gen);
    unsigned handle       = handle_dist(gen);
    unsigned symb_id      = symb_id_dist(gen);
    unsigned slot_id      = slot_id_dist(gen);
    unsigned ra_id_d      = ra_id_dist(gen);
    int      rssi_di      = rssi_dist(gen);
    int      rsrp_di      = rsrp_dist(gen);
    int      snr_dis      = snr_dist(gen);
    unsigned timing_di    = timing_dist(gen);
    unsigned timing_ns_di = timing_ns_dist(gen);

    optional<float> rssi;
    if (-140 <= rssi_di && rssi_di <= 30) {
      rssi.emplace(rssi_di);
    }

    optional<float> rsrp;
    if (-140 <= rsrp_di && rsrp_di <= 5) {
      rssi.emplace(rsrp_di);
    }

    optional<float> snr;
    if (-64 <= snr_dis && snr_dis <= 63) {
      rssi.emplace(snr_dis);
    }

    builder.set_basic_parameters(sfn, slot);

    auto pdu_builder = builder.add_pdu(handle, symb_id, slot_id, ra_id_d, rssi, rsrp, snr, i);

    optional<unsigned> timing;
    if (timing_di <= 3846) {
      timing.emplace(timing_di);
    }

    optional<uint32_t> timing_ns;
    if (timing_ns_di <= 2005000) {
      timing_ns.emplace(timing_ns_di);
    }

    int             pw = rssi_dist(gen);
    optional<float> preamble_power;
    if (-140 <= pw && pw <= 30) {
      preamble_power.emplace(pw);
    }

    int             prem_snr = snr_dist(gen);
    optional<float> preamble_snr;
    if (-64 <= prem_snr && prem_snr <= 63) {
      preamble_snr.emplace(prem_snr);
    }

    pdu_builder.add_preamble(timing, timing_ns, preamble_power, preamble_snr);

    TESTASSERT_EQ(sfn, msg.sfn);
    TESTASSERT_EQ(slot, msg.slot);
    TESTASSERT_EQ(1, msg.pdus.size());

    const auto& pdu = msg.pdus.back();
    TESTASSERT_EQ(handle, pdu.handle);
    TESTASSERT_EQ(symb_id, pdu.symbol_index);
    TESTASSERT_EQ(slot_id, pdu.slot_index);
    TESTASSERT_EQ(ra_id_d, pdu.ra_index);
    TESTASSERT_EQ(rssi ? static_cast<unsigned>((rssi.value() + 140.F) * 1000) : std::numeric_limits<uint32_t>::max(),
                  pdu.avg_rssi);
    TESTASSERT_EQ(rsrp ? static_cast<unsigned>((rsrp.value() + ((i) ? 140.F : 128.F)) * 10.F)
                       : std::numeric_limits<uint16_t>::max(),
                  pdu.rsrp);
    TESTASSERT_EQ(snr ? (snr.value() + 64) * 2 : std::numeric_limits<uint8_t>::max(), pdu.avg_snr);
    TESTASSERT_EQ(1, pdu.preambles.size());

    const auto& preamble = pdu.preambles.back();
    TESTASSERT_EQ(0, preamble.preamble_index);
    TESTASSERT_EQ(timing ? timing.value() : std::numeric_limits<uint16_t>::max(), preamble.timing_advance_offset);
    TESTASSERT_EQ(timing_ns ? timing_ns.value() : std::numeric_limits<uint32_t>::max(),
                  preamble.timing_advance_offset_ns);
    TESTASSERT_EQ(preamble_power ? static_cast<unsigned>((preamble_power.value() + 140.F) * 1000.F)
                                 : std::numeric_limits<uint32_t>::max(),
                  preamble.preamble_pwr);
    TESTASSERT_EQ(preamble_snr ? static_cast<unsigned>((preamble_snr.value() + 64.F) * 2.F)
                               : std::numeric_limits<uint8_t>::max(),
                  preamble.preamble_snr);
  }
}

int main()
{
  test_rach_indication_builder_ok();
  fmt::print("RACH.indication builder -> OK\n");
}
