/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/dl_pdcch_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static const std::vector<test_group<dl_pdcch_pdu>> vector_test = {
    {[](dl_pdcch_pdu& msg, int value) { msg.coreset_bwp_size = value; },
     "CORESET BWP size",
     {{0, false}, {1, true}, {140, true}, {275, true}, {276, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.coreset_bwp_start = value; },
     "CORESET BWP start",
     {{0, true}, {140, true}, {274, true}, {275, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.scs = static_cast<subcarrier_spacing>(value); },
     "Subcarrier spacing",
     {{0, true}, {2, true}, {4, true}, {5, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.cyclic_prefix = static_cast<cyclic_prefix_type>(value); },
     "Cyclic prefix",
     {{0, true}, {1, true}, {2, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.start_symbol_index = value; },
     "Start symbol index",
     {{0, true}, {7, true}, {13, true}, {14, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.duration_symbols     = value;
       msg.cce_reg_mapping_type = cce_to_reg_mapping_type::non_interleaved;
       msg.reg_bundle_size      = 6;
     },
     "Duration symbols",
     {{0, false}, {1, true}, {3, true}, {4, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.cce_reg_mapping_type = static_cast<cce_to_reg_mapping_type>(value);
       if (msg.cce_reg_mapping_type == cce_to_reg_mapping_type::interleaved) {
         msg.duration_symbols = 2;
         msg.reg_bundle_size  = 2;
         msg.shift_index      = 1;
         msg.interleaver_size = 2;
       } else {
         msg.reg_bundle_size = 6;
       }
     },
     "CCE to REG mapping type",
     {{0, true}, {1, true}, {2, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.cce_reg_mapping_type = cce_to_reg_mapping_type::non_interleaved;
       msg.reg_bundle_size      = value;
     },
     "REG bundle size",
     {{1, false}, {2, false}, {3, false}, {4, false}, {5, false}, {6, true}, {7, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.cce_reg_mapping_type = cce_to_reg_mapping_type::interleaved;
       msg.duration_symbols     = 2;
       msg.reg_bundle_size      = value;
     },
     "REG bundle size",
     {{1, false}, {2, true}, {3, false}, {4, false}, {5, false}, {6, true}, {7, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.cce_reg_mapping_type = cce_to_reg_mapping_type::interleaved;
       msg.duration_symbols     = 1;
       msg.reg_bundle_size      = value;
     },
     "REG bundle size",
     {{1, false}, {2, true}, {3, false}, {4, false}, {5, false}, {6, true}, {7, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.cce_reg_mapping_type = cce_to_reg_mapping_type::interleaved;
       msg.duration_symbols     = 3;
       msg.reg_bundle_size      = value;
     },
     "REG bundle size",
     {{1, false}, {2, false}, {3, true}, {4, false}, {5, false}, {6, true}, {7, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.interleaver_size = value; },
     "Interleaver size",
     {{1, false}, {2, true}, {3, true}, {4, false}, {5, false}, {6, true}, {7, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.coreset_type = static_cast<pdcch_coreset_type>(value); },
     "CORESET type",
     {{0, true}, {1, true}, {2, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.coreset_type = pdcch_coreset_type::pbch_or_sib1;
       msg.shift_index  = value;
     },
     "Shift index - PBCH or SIB1",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.coreset_type = pdcch_coreset_type::other;
       msg.shift_index  = value;
     },
     "Shift index - Other",
     {{0, true}, {140, true}, {275, true}, {276, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.precoder_granularity = static_cast<coreset_configuration::precoder_granularity_type>(value);
     },
     "Precoder granularity",
     {{0, true}, {1, true}, {2, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.dl_dci.back().rnti = to_rnti(value); },
     "RNTI",
     {{0, false}, {1, true}, {32768, true}, {65535, true}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.dl_dci.back().cce_index = value; },
     "CCE index",
     {{0, true}, {63, true}, {135, true}, {136, false}}},
    {[](dl_pdcch_pdu& msg, int value) { msg.dl_dci.back().aggregation_level = value; },
     "Aggregation level",
     {{0, false},
      {1, true},
      {2, true},
      {3, false},
      {4, true},
      {5, false},
      {7, false},
      {8, true},
      {9, false},
      {12, false},
      {15, false},
      {16, true},
      {17, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.dl_dci.back().power_control_offset_ss_profile_nr               = value;
       msg.maintenance_v3.info.back().pdcch_dmrs_power_offset_profile_sss = -32768;
     },
     "DMRS Power offset",
     {{static_cast<unsigned>(int16_t(-126)), false},
      {static_cast<unsigned>(int16_t(-9)), false},
      {static_cast<unsigned>(int16_t(-8)), true},
      {0, true},
      {8, true},
      {9, false}}},
    {[](dl_pdcch_pdu& msg, int value) {
       msg.dl_dci.back().power_control_offset_ss_profile_nr               = -127;
       msg.maintenance_v3.info.back().pdcch_dmrs_power_offset_profile_sss = value;
     },
     "DMRS Power offset",
     {
         {static_cast<unsigned>(int16_t(-32768)), false},
         {static_cast<unsigned>(int16_t(-32767)), true},
         {0, true},
         {32767, true},
     }}};

static void test_validate_each_field_error()
{
  static const message_type_id msg_id = message_type_id::dl_tti_request;
  for (const auto& group : vector_test) {
    for (const auto& test_case : group) {
      auto msg = build_valid_dl_pdcch_pdu();
      group.update_msg(msg, test_case.value);
      validator_report report(0, 0);
      auto             result = validate_dl_pdcch_pdu(msg_id, msg, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(msg_id, rep.message_type);
        TESTASSERT_EQ(dl_pdu_type::PDCCH, static_cast<dl_pdu_type>(rep.pdu_type.value()));
      }
    }
  }
}

/// Tests that a valid PDCCH PDU validates correctly.
static void test_dl_pdcch_pdu_ok()
{
  dl_pdcch_pdu pdu = build_valid_dl_pdcch_pdu();

  validator_report report(0, 0);
  TESTASSERT(validate_dl_pdcch_pdu(message_type_id::ul_dci_request, pdu, report));
  // Assert no reports were generated.
  TESTASSERT(report.reports.empty());
}

/// Tests that a PDCCH PDU that contains errors fails to validate.
static void test_dl_pdcch_pdu_error()
{
  dl_pdcch_pdu pdu = build_valid_dl_pdcch_pdu();

  // Force 3 errors.
  pdu.coreset_bwp_size = 2690;
  pdu.reg_bundle_size  = 8;
  pdu.dl_dci[0].rnti   = to_rnti(0);

  validator_report report(0, 0);
  TESTASSERT(!validate_dl_pdcch_pdu(message_type_id::ul_dci_request, pdu, report));
  // Assert 3 reports were generated.
  TESTASSERT_EQ(report.reports.size(), 3u);
}

int main()
{
  test_dl_pdcch_pdu_ok();
  test_dl_pdcch_pdu_error();
  test_validate_each_field_error();

  return 0;
}
