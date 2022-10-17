/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/ul_pucch_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

// Vector test for the properties valid for all the PUCCH formats.
static const std::vector<test_group<ul_pucch_pdu>> vector_test_common_fields = {
    {[](ul_pucch_pdu& pdu, int value) { pdu.rnti = to_rnti(value); },
     "RNTI",
     {{0, false}, {1, true}, {32768, true}, {65535, true}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.bwp_size = value; },
     "BWP size",
     {{0, false}, {1, true}, {128, true}, {275, true}, {276, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.bwp_start = value; },
     "BWP start",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.scs = static_cast<subcarrier_spacing>(value); },
     "Subcarrier spacing",
     {{0, true}, {2, true}, {4, true}, {5, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); },
     "Cyclic prefix",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.multi_slot_tx_indicator = static_cast<pucch_repetition_tx_slot>(value); },
     "Multi slot tx indicator",
     {{0, true}, {1, true}, {3, true}, {4, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.prb_start = value; },
     "PRB start",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.prb_size = value; },
     "PRB size",
     {{0, false}, {1, true}, {8, true}, {16, true}, {17, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.start_symbol_index = value; },
     "Start symbol index",
     {{0, true}, {6, true}, {13, true}, {14, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.second_hop_prb = value; },
     "Second hop PRB",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pucch_pdu& pdu, int value) {
       pdu.uci_correspondence.part2.back().part2_size_map_scope =
           static_cast<uci_part1_to_part2_correspondence_v3::map_scope_type>(value);
     },
     "Part2 size map scope",
     {{0, true}, {1, true}, {2, false}}}};

static void validate_common()
{
  std::vector<std::function<ul_pucch_pdu()>> builders = {build_valid_ul_pucch_f0_pdu,
                                                         build_valid_ul_pucch_f1_pdu,
                                                         build_valid_ul_pucch_f2_pdu,
                                                         build_valid_ul_pucch_f3_pdu,
                                                         build_valid_ul_pucch_f4_pdu};

  for (auto builder : builders) {
    for (const auto& group : vector_test_common_fields) {
      for (const auto& test_case : group) {
        validator_report report(0, 0);
        auto             pdu = builder();
        group.update_msg(pdu, test_case.value);
        bool result = validate_ul_pucch_pdu(pdu, report);

        TESTASSERT_EQ(result, test_case.result);
        if (!result) {
          TESTASSERT_EQ(1U, report.reports.size());
          const auto& rep = report.reports.back();
          TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
          TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
          TESTASSERT_EQ(ul_pdu_type::PUCCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
        } else {
          TESTASSERT(report.reports.empty());
        }
      }
    }
  }
}

static const std::vector<test_group<ul_pucch_pdu>> vector_test_f0 = {
    {[](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; },
     "Duration in symbols",
     {{0, false}, {1, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_grp_hopping = static_cast<pucch_group_hopping>(value); },
     "PUCCH group hopping",
     {{0, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; },
     "NID PUCCH hopping",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; },
     "Initial cyclic shift",
     {{0, true}, {6, true}, {11, true}, {12, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }, "SR bit len", {{0, true}, {1, true}, {2, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; },
     "HARQ bit len",
     {{0, true}, {1, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_maintenance_v3.max_code_rate = value; },
     "Max code rate",
     {{0, false}, {128, false}, {254, false}, {255, true}}}};

static void validate_f0()
{
  for (const auto& group : vector_test_f0) {
    for (const auto& test_case : group) {
      validator_report report(0, 0);
      auto             pdu = build_valid_ul_pucch_f0_pdu();
      group.update_msg(pdu, test_case.value);
      bool result = validate_ul_pucch_pdu(pdu, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PUCCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      } else {
        TESTASSERT(report.reports.empty());
      }
    }
  }
}

static const std::vector<test_group<ul_pucch_pdu>> vector_test_f1 = {
    {[](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; },
     "Duration in symbols",
     {{3, false}, {4, true}, {14, true}, {15, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.time_domain_occ_index = value; },
     "Time domain occ index",
     {{0, true}, {6, true}, {7, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_grp_hopping = static_cast<pucch_group_hopping>(value); },
     "PUCCH group hopping",
     {{0, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; },
     "NID PUCCH hopping",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; },
     "Initial cyclic shift",
     {{0, true}, {6, true}, {11, true}, {12, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }, "SR bit len", {{0, true}, {1, true}, {2, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; },
     "HARQ bit len",
     {{0, true}, {1, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_maintenance_v3.max_code_rate = value; },
     "Max code rate",
     {{0, false}, {128, false}, {254, false}, {255, true}}}};

static void validate_f1()
{
  for (const auto& group : vector_test_f1) {
    for (const auto& test_case : group) {
      validator_report report(0, 0);
      auto             pdu = build_valid_ul_pucch_f1_pdu();
      group.update_msg(pdu, test_case.value);
      bool result = validate_ul_pucch_pdu(pdu, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PUCCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      } else {
        TESTASSERT(report.reports.empty());
      }
    }
  }
}

static const std::vector<test_group<ul_pucch_pdu>> vector_test_f2 = {
    {[](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; },
     "Duration in symbols",
     {{0, false}, {1, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_scrambling = value; },
     "NID PUCCH scrambling",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }, "SR bit len", {{0, true}, {4, true}, {5, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; },
     "HARQ bit len",
     {{0, true}, {1, false}, {2, true}, {1706, true}, {1707, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.csi_part1_bit_length = value; },
     "CSI part1 bit length",
     {{0, true}, {800, true}, {1706, true}, {1707, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_maintenance_v3.max_code_rate = value; },
     "Max code rate",
     {{0, true}, {3, true}, {7, true}, {8, false}}}};

static void validate_f2()
{
  for (const auto& group : vector_test_f2) {
    for (const auto& test_case : group) {
      validator_report report(0, 0);
      auto             pdu = build_valid_ul_pucch_f2_pdu();
      group.update_msg(pdu, test_case.value);
      bool result = validate_ul_pucch_pdu(pdu, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PUCCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      } else {
        TESTASSERT(report.reports.empty());
      }
    }
  }
}

static const std::vector<test_group<ul_pucch_pdu>> vector_test_f3 = {
    {[](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; },
     "Duration in symbols",
     {{3, false}, {4, true}, {14, true}, {15, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_scrambling = value; },
     "NID PUCCH scrambling",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.m0_pucch_dmrs_cyclic_shift = value; },
     "M0 PUCCH DMRS cyclic shift",
     {{0, true}, {4, true}, {9, true}, {10, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_grp_hopping = static_cast<pucch_group_hopping>(value); },
     "PUCCH group hopping",
     {{0, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; },
     "NID PUCCH hopping",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; },
     "Initial cyclic shift",
     {{0, true}, {6, true}, {11, true}, {12, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }, "SR bit len", {{0, true}, {4, true}, {5, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; },
     "HARQ bit len",
     {{0, true}, {1, false}, {2, true}, {1706, true}, {1707, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.csi_part1_bit_length = value; },
     "CSI part1 bit length",
     {{0, true}, {800, true}, {1706, true}, {1707, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_maintenance_v3.max_code_rate = value; },
     "Max code rate",
     {{0, true}, {3, true}, {7, true}, {8, false}}}};

static const void validate_f3()
{
  for (auto& group : vector_test_f3) {
    for (const auto& test_case : group) {
      validator_report report(0, 0);
      auto             pdu = build_valid_ul_pucch_f3_pdu();
      group.update_msg(pdu, test_case.value);
      bool result = validate_ul_pucch_pdu(pdu, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PUCCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      } else {
        TESTASSERT(report.reports.empty());
      }
    }
  }
}

static const std::vector<test_group<ul_pucch_pdu>> vector_test_f4 = {
    {[](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; },
     "Duration in symbols",
     {{3, false}, {4, true}, {14, true}, {15, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_scrambling = value; },
     "NID PUCCH scrambling",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_grp_hopping = static_cast<pucch_group_hopping>(value); },
     "PUCCH group hopping",
     {{0, true}, {2, true}, {3, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pre_dft_occ_idx = value; },
     "Pre DFT occ index",
     {{0, true}, {2, true}, {3, true}, {4, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pre_dft_occ_len = value; },
     "Pre DFT occ len",
     {{1, false}, {2, true}, {3, false}, {4, true}, {5, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.m0_pucch_dmrs_cyclic_shift = value; },
     "M0 PUCCH DMRS cyclic shift",
     {{0, true}, {4, true}, {9, true}, {10, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; },
     "NID PUCCH hopping",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; },
     "Initial cyclic shift",
     {{0, true}, {6, true}, {11, true}, {12, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }, "SR bit len", {{0, true}, {4, true}, {5, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; },
     "HARQ bit len",
     {{0, true}, {1, false}, {2, true}, {1706, true}, {1707, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.csi_part1_bit_length = value; },
     "CSI part1 bit length",
     {{0, true}, {800, true}, {1706, true}, {1707, false}}},
    {[](ul_pucch_pdu& pdu, int value) { pdu.pucch_maintenance_v3.max_code_rate = value; },
     "Max code rate",
     {{0, true}, {3, true}, {7, true}, {8, false}}}};

static void validate_f4()
{
  for (const auto& group : vector_test_f4) {
    for (const auto& test_case : group) {
      validator_report report(0, 0);
      auto             pdu = build_valid_ul_pucch_f4_pdu();
      group.update_msg(pdu, test_case.value);
      bool result = validate_ul_pucch_pdu(pdu, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PUCCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      } else {
        TESTASSERT(report.reports.empty());
      }
    }
  }
}

static void test_validate_each_field_error()
{
  validate_common();
  validate_f0();
  validate_f1();
  validate_f2();
  validate_f3();
  validate_f4();
}

static const std::vector<test_group<ul_pucch_pdu>> vector_test_pucch_format = {
    {[](ul_pucch_pdu& pdu, int value) { pdu.format_type = static_cast<pucch_format>(value); },
     "Format type",
     {{5, false}}}};

static void test_validate_pucch_format_error()
{
  std::vector<std::function<ul_pucch_pdu()>> builders = {build_valid_ul_pucch_f0_pdu,
                                                         build_valid_ul_pucch_f1_pdu,
                                                         build_valid_ul_pucch_f2_pdu,
                                                         build_valid_ul_pucch_f3_pdu,
                                                         build_valid_ul_pucch_f4_pdu};

  for (auto builder : builders) {
    for (const auto& group : vector_test_pucch_format) {
      for (const auto& test_case : group) {
        validator_report report(0, 0);
        auto             pdu = builder();
        group.update_msg(pdu, test_case.value);
        bool result = validate_ul_pucch_pdu(pdu, report);

        TESTASSERT_EQ(result, test_case.result);
        // Check that the first error is format type.
        const auto& rep = report.reports.front();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PUCCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      }
    }
  }
}

static void test_validate_more_that_one_error_simultaneously()
{
  std::vector<std::function<ul_pucch_pdu()>> builders = {build_valid_ul_pucch_f0_pdu,
                                                         build_valid_ul_pucch_f1_pdu,
                                                         build_valid_ul_pucch_f2_pdu,
                                                         build_valid_ul_pucch_f3_pdu,
                                                         build_valid_ul_pucch_f4_pdu};

  for (auto builder : builders) {
    validator_report report(0, 0);
    auto             pdu = builder();

    // Add 4 errors.
    pdu.start_symbol_index = 20;
    pdu.prb_size           = 30;
    pdu.prb_start          = 300;
    pdu.rnti               = to_rnti(0);

    TESTASSERT(!validate_ul_pucch_pdu(pdu, report));
    TESTASSERT_EQ(4U, report.reports.size());
    // Check that the properties that caused the error are different.
    for (unsigned i = 0, e = report.reports.size(); i != e; ++i) {
      for (unsigned j = i + 1; j != e; ++j) {
        TESTASSERT(std::strcmp(report.reports[i].property_name, report.reports[j].property_name) != 0);
      }
    }
  }
}

static void test_validate_pucch_pdu_ok()
{
  std::vector<std::function<ul_pucch_pdu()>> builders = {build_valid_ul_pucch_f0_pdu,
                                                         build_valid_ul_pucch_f1_pdu,
                                                         build_valid_ul_pucch_f2_pdu,
                                                         build_valid_ul_pucch_f3_pdu,
                                                         build_valid_ul_pucch_f4_pdu};

  for (auto builder : builders) {
    validator_report report(0, 0);
    const auto&      pdu = builder();
    TESTASSERT(validate_ul_pucch_pdu(pdu, report));
    TESTASSERT(report.reports.empty());
  }
}

int main()
{
  test_validate_pucch_pdu_ok();
  test_validate_each_field_error();
  test_validate_pucch_format_error();
  test_validate_more_that_one_error_simultaneously();
  fmt::print("UL PUCCH validator -> OK\n");
}
