/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static std::mt19937 gen(0);

static std::vector<test_group<rach_indication_message> > vector_test = {
    {[](rach_indication_message& msg, int value) { msg.sfn = value; },
     "sfn",
     {{0, true}, {511, true}, {1023, true}, {1024, false}}},
    {[](rach_indication_message& msg, int value) { msg.slot = value; },
     "slot",
     {{0, true}, {79, true}, {159, true}, {160, false}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().symbol_index = value; },
     "Symbol index",
     {{0, true}, {6, true}, {13, true}, {14, false}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().slot_index = value; },
     "Slot index",
     {{0, true}, {40, true}, {79, true}, {80, false}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().ra_index = value; },
     "Index of the received PRACH frequency domain occasion",
     {{0, true}, {4, true}, {7, true}, {8, false}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().avg_rssi = value; },
     "AVG RSSI",
     {{0, true},
      {85000, true},
      {170000, true},
      {170001, false},
      {std::numeric_limits<uint32_t>::max() - 1, false},
      {std::numeric_limits<uint32_t>::max(), true}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().rsrp = value; },
     "RSRP",
     {{0, true},
      {640, true},
      {1280, true},
      {1281, false},
      {std::numeric_limits<uint16_t>::max() - 1, false},
      {std::numeric_limits<uint16_t>::max(), true}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().preambles.back().preamble_index = value; },
     "Preamble index",
     {{0, true}, {32, true}, {63, true}, {64, false}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().preambles.back().timing_advance_offset = value; },
     "Timing advance offset",
     {{0, true},
      {1923, true},
      {3846, true},
      {3847, false},
      {std::numeric_limits<uint16_t>::max() - 1, false},
      {std::numeric_limits<uint16_t>::max(), true}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().preambles.back().timing_advance_offset_ns = value; },
     "Timing advance offset in nanoseconds",
     {{0, true},
      {1002000, true},
      {2005000, true},
      {2005001, false},
      {std::numeric_limits<uint32_t>::max() - 1, false},
      {std::numeric_limits<uint32_t>::max(), true}}},
    {[](rach_indication_message& msg, int value) { msg.pdus.back().preambles.back().preamble_pwr = value; },
     "Preamble power",
     {{0, true},
      {85000, true},
      {170000, true},
      {170001, false},
      {std::numeric_limits<uint32_t>::max() - 1, false},
      {std::numeric_limits<uint32_t>::max(), true}}}};

static void test_validate_each_field_error()
{
  for (auto& group : vector_test) {
    for (const auto& test_case : group) {
      auto msg = build_valid_rach_indication();
      group.update_msg(msg, test_case.value);
      auto result = validate_rach_indication(msg);
      TESTASSERT_EQ(result.operator bool(), test_case.result);
      if (!result) {
        const auto& report = result.error();
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::rach_indication, rep.message_type);
        TESTASSERT(!rep.pdu_type);
      }
    }
  }
}

static void test_rach_indication_validator_ok()
{
  rach_indication_message msg = build_valid_rach_indication();

  const auto& result = validate_rach_indication(msg);

  TESTASSERT(result);
}

static void test_rach_indication_validator_error()
{
  rach_indication_message msg = build_valid_rach_indication();

  msg.pdus.back().symbol_index = 14;
  msg.pdus.back().slot_index   = 80;
  msg.pdus.back().ra_index     = 9;

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
  test_validate_each_field_error();
  fmt::print("RACH.indication builder -> OK\n");
}
