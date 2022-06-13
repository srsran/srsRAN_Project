/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/ul_prach_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static const std::vector<test_group<ul_prach_pdu> > vector_test = {
    {[](ul_prach_pdu& pdu, int value) { pdu.phys_cell_id = value; },
     "Physical cell ID",
     {{0, true}, {1007, true}, {100, true}, {1008, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.num_prach_ocas = value; },
     "Number of PRACH occasions",
     {{0, false}, {1, true}, {7, true}, {8, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.prach_format = static_cast<prach_format_type>(value); },
     "PRACH format",
     {{0, true}, {7, true}, {13, true}, {14, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.index_fd_ra = value; },
     "Frequency domain occasion index",
     {{0, true}, {3, true}, {7, true}, {8, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.prach_start_symbol = value; },
     "PRACH start symbol",
     {{0, true}, {7, true}, {13, true}, {14, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.num_cs = value; },
     "Zero-correlation zone configuration number",
     {{0, true}, {200, true}, {419, true}, {420, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.is_msg_a_prach = value; },
     "Is msgA-PRACH",
     {{0, true}, {1, true}, {2, true}, {3, false}}},
    {[](ul_prach_pdu& pdu, int value) {
       pdu.maintenance_v3.prach_config_scope = static_cast<prach_config_scope_type>(value);
     },
     "PRACH config scope",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.maintenance_v3.num_fd_ra = value; },
     "Number of frequency domain occasions",
     {{0, false}, {1, true}, {4, true}, {8, true}, {9, false}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.maintenance_v3.start_preamble_index = value; },
     "Start preamble index",
     {{0, true}, {32, true}, {63, true}, {64, false}, {128, false}, {254, false}, {255, true}}},
    {[](ul_prach_pdu& pdu, int value) { pdu.maintenance_v3.num_preamble_indices = value; },
     "Number of preambles logical indices",
     {{0, false}, {1, true}, {4, true}, {32, true}, {64, true}, {65, false}}}};

static void test_validate_each_field_error()
{
  for (const auto& group : vector_test) {
    for (const auto& test_case : group) {
      validator_report report(0, 0);
      auto             pdu = build_valid_ul_prach_pdu();
      group.update_msg(pdu, test_case.value);
      bool result = validate_ul_prach_pdu(pdu, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PRACH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      } else {
        TESTASSERT(report.reports.empty());
      }
    }
  }
}

static void test_validate_more_that_one_error_simultaneously()
{
  validator_report report(0, 0);
  auto             pdu = build_valid_ul_prach_pdu();

  // Add 6 errors.
  pdu.maintenance_v3.num_preamble_indices = 65;
  pdu.maintenance_v3.start_preamble_index = 254;
  pdu.maintenance_v3.num_fd_ra            = 9;
  pdu.num_cs                              = 500;
  pdu.index_fd_ra                         = 18;
  pdu.num_prach_ocas                      = 10;

  TESTASSERT(!validate_ul_prach_pdu(pdu, report));
  TESTASSERT_EQ(4U, report.reports.size());
  TESTASSERT_EQ(6U, report.nof_errors);
  // Check that the properties that caused the error are different.
  for (unsigned i = 0, e = report.reports.size(); i != e; ++i) {
    for (unsigned j = i + 1; j != e; ++j) {
      TESTASSERT(std::strcmp(report.reports[i].property_name, report.reports[j].property_name) != 0);
    }
  }
}

static void test_validate_prach_pdu_ok()
{
  validator_report report(0, 0);
  const auto&      pdu = build_valid_ul_prach_pdu();
  TESTASSERT(validate_ul_prach_pdu(pdu, report));
  TESTASSERT(report.reports.empty());
}

int main()
{
  test_validate_prach_pdu_ok();
  test_validate_each_field_error();
  test_validate_more_that_one_error_simultaneously();
  fmt::print("UL PRACH PDU validator -> OK\n");
}
