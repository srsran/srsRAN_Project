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
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;
using namespace fapi;

static std::mt19937 gen(0);

static void test_rach_indication_validator_ok()
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

  for (unsigned i = 0; i != 2; ++i) {
    rach_indication_message         msg;
    rach_indication_message_builder builder(msg);

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

    builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen));

    auto pdu_builder =
        builder.add_pdu(handle_dist(gen), symb_id_dist(gen), slot_id_dist(gen), ra_id_dist(gen), rssi, rsrp, snr, i);

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

    const auto& result = validate_rach_indication(msg);

    TESTASSERT(result);
  }
}

static void test_rach_indication_validator_error()
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

  rach_indication_message         msg;
  rach_indication_message_builder builder(msg);

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

  builder.set_basic_parameters(sfn_dist(gen), slot_dist(gen));

  auto pdu_builder = builder.add_pdu(handle_dist(gen), 14, 80, 8, rssi, rsrp, snr);

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

  const auto& result = validate_rach_indication(msg);

  TESTASSERT(!result);
  const auto& report = result.error();
  // Check that the 3 errors are reported.
  TESTASSERT_EQ(report.reports.size(), 3U);
}

int main()
{
  test_rach_indication_validator_ok();
  test_rach_indication_validator_error();
  fmt::print("RACH.indication builder -> OK\n");
}
