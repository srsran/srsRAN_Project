/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/dl_ssb_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static const std::vector<test_group<dl_ssb_pdu> > vector_test = {
    {[](dl_ssb_pdu& msg, int value) { msg.phys_cell_id = value; },
     "Physical cell ID",
     {{0, true}, {511, true}, {1007, true}, {1008, false}}},
    {[](dl_ssb_pdu& msg, int value) {
       msg.beta_pss_profile_nr                     = static_cast<beta_pss_profile_type>(value);
       msg.ssb_maintenance_v3.beta_pss_profile_sss = (value == 255) ? 0 : std::numeric_limits<int16_t>::min();
     },
     "Beta PSS profile NR",
     {{0, true}, {1, true}, {2, false}, {254, false}, {255, true}}},
    {[](dl_ssb_pdu& msg, int value) { msg.ssb_block_index = value; },
     "SS/PBCH block index",
     {{0, true}, {32, true}, {63, true}, {64, false}}},
    {[](dl_ssb_pdu& msg, int value) { msg.ssb_subcarrier_offset = value; },
     "Subcarrier offset",
     {{0, true}, {16, true}, {31, true}, {32, false}}},
    {[](dl_ssb_pdu& msg, int value) { msg.ssb_offset_pointA = value; },
     "Offset point A",
     {{0, true}, {1100, true}, {2199, true}, {2200, false}}},
    {[](dl_ssb_pdu& msg, int value) { msg.ssb_maintenance_v3.case_type = static_cast<ssb_pattern_case>(value); },
     "SSB Case type",
     {{0, true}, {2, true}, {4, true}, {5, false}}},
    {[](dl_ssb_pdu& msg, int value) { msg.ssb_maintenance_v3.scs = static_cast<subcarrier_spacing>(value); },
     "Subcarrier spacing",
     {{0, true}, {2, true}, {4, true}, {5, false}}},
    {[](dl_ssb_pdu& msg, int value) { msg.ssb_maintenance_v3.lmax = value; },
     "LMax",
     {{3, false}, {4, true}, {5, false}, {7, false}, {8, true}, {9, false}, {63, false}, {64, true}, {65, false}}},
    {[](dl_ssb_pdu& msg, int value) { msg.ssb_maintenance_v3.ss_pbch_block_power_scaling = value; },
     "Baseband power scaling applied to SS-PBCH",
     {{static_cast<unsigned>(int16_t(-32768)), true},
      {static_cast<unsigned>(int16_t(-32767)), false},
      {static_cast<unsigned>(int16_t(-11001)), false},
      {static_cast<unsigned>(int16_t(-11000)), true},
      {0, true},
      {12000, true},
      {12001, false}}}};

static void test_validate_each_field_error()
{
  for (const auto& group : vector_test) {
    for (const auto& test_case : group) {
      auto msg = build_valid_dl_ssb_pdu();
      group.update_msg(msg, test_case.value);
      validator_report report(0, 0);
      auto             result = validate_dl_ssb_pdu(msg, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::dl_tti_request, rep.message_type);
        TESTASSERT_EQ(dl_pdu_type::SSB, static_cast<dl_pdu_type>(rep.pdu_type.value()));
      }
    }
  }
}

/// Tests that a valid SSB pdu validates correctly.
static void test_dl_ssb_pdu_ok()
{
  dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_dl_ssb_pdu(pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

/// Tests that a SSB pdu that contains errors fails to validate.
static void test_dl_ssb_pdu_error()
{
  dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  // Force 3 errors.
  pdu.phys_cell_id                                   = 2000;
  pdu.ssb_block_index                                = 100;
  pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling = -12000;

  validator_report report(0, 0);
  TESTASSERT(!validate_dl_ssb_pdu(pdu, report));
  // Assert 3 reports were generated.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_dl_ssb_pdu_ok();
  test_dl_ssb_pdu_error();
  test_validate_each_field_error();
  fmt::print("DL SSB PDU validator -> Ok\n");
}
