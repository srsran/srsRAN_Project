/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/ul_pusch_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

static std::vector<test_group<ul_pusch_pdu> > vector_test = {
    {[](ul_pusch_pdu& pdu, int value) { pdu.rnti = to_rnti(value); },
     "RNTI",
     {{0, false}, {1, true}, {32768, true}, {65535, true}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.bwp_size = value; },
     "BWP size",
     {{0, false}, {1, true}, {128, true}, {275, true}, {276, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.bwp_start = value; },
     "BWP start",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.scs = static_cast<subcarrier_spacing>(value); },
     "Subcarrier spacing",
     {{0, true}, {2, true}, {4, true}, {5, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.cyclic_prefix = static_cast<cyclic_prefix_type>(value); },
     "Cyclic prefix",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) {
       pdu.transform_precoding = false;
       pdu.qam_mod_order       = value;
     },
     "QAM modulation order",
     {{1, false}, {2, true}, {3, false}, {4, true}, {5, false}, {6, true}, {7, false}, {8, true}, {9, false}}},
    {[](ul_pusch_pdu& pdu, int value) {
       pdu.transform_precoding = true;
       pdu.qam_mod_order       = value;
     },
     "QAM modulation order",
     {{0, false},
      {1, true},
      {2, true},
      {3, false},
      {4, true},
      {5, false},
      {6, true},
      {7, false},
      {8, true},
      {9, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.mcs_index = value; },
     "MCS index",
     {{0, true}, {16, true}, {31, true}, {32, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.mcs_table = static_cast<pusch_mcs_table_type>(value); },
     "MCS table",
     {{0, true}, {2, true}, {4, true}, {5, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.nid_pusch = value; },
     "NID PUSCH",
     {{0, true}, {512, true}, {1023, true}, {1024, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.num_layers = value; },
     "Number of layers",
     {{0, false}, {1, true}, {4, true}, {5, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.dmrs_type = static_cast<dmrs_config_type>(value); },
     "DMRS config type",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.low_papr_dmrs = static_cast<low_papr_dmrs_type>(value); },
     "Low PAPR DMRS",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_dmrs_identity = value; },
     "PUSCH DMRS identity",
     {{0, true}, {503, true}, {1007, true}, {1008, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.nscid = value; }, "NSCID", {{0, true}, {1, true}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.num_dmrs_cdm_grps_no_data = value; },
     "Number of DMRS CDM groups without data",
     {{0, false}, {1, true}, {2, true}, {3, true}, {4, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.resource_alloc = static_cast<resource_allocation_type>(value); },
     "Resource allocation type",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) {
       pdu.resource_alloc = resource_allocation_type::type_1;
       pdu.rb_start       = value;
     },
     "RB start",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pusch_pdu& pdu, int value) {
       pdu.resource_alloc = resource_allocation_type::type_1;
       pdu.rb_size        = value;
     },
     "RB size",
     {{0, false}, {1, true}, {128, true}, {275, true}, {276, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.vrb_to_prb_mapping = static_cast<vrb_to_prb_mapping_type>(value); },
     "VRB-to-PRB mapping",
     {{0, true}, {1, false}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.tx_direct_current_location = value; },
     "Tx direct current location",
     {{0, true}, {1, true}, {2048, true}, {4095, true}, {4096, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.start_symbol_index = value; },
     "Start symbol index",
     {{0, true}, {1, true}, {6, true}, {13, true}, {14, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.nr_of_symbols = value; },
     "Number of symbols",
     {{0, false}, {1, true}, {7, true}, {14, true}, {15, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_data.rv_index = value; },
     "RV index",
     {{0, true}, {1, true}, {2, true}, {3, true}, {4, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_data.harq_process_id = value; },
     "HARQ process id",
     {{0, true}, {1, true}, {7, true}, {15, true}, {16, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_data.new_data = value; },
     "New data",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_uci.harq_ack_bit_length = value; },
     "HARQ ACK bit length",
     {{0, true}, {200, true}, {800, true}, {1706, true}, {1707, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_uci.csi_part1_bit_length = value; },
     "CSI part1 bit length",
     {{0, true}, {200, true}, {800, true}, {1706, true}, {1707, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_uci.flags_csi_part2 = value; },
     "Flag CSI part2",
     {{0, true}, {1, false}, {65534, false}, {65535, true}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_uci.alpha_scaling = value; },
     "Alpha scaling",
     {{0, true}, {1, true}, {2, true}, {3, true}, {4, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_uci.beta_offset_harq_ack = value; },
     "Beta offset HARQ ACK",
     {{0, true}, {7, true}, {15, true}, {16, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_uci.beta_offset_csi1 = value; },
     "Beta offset CSI Part1",
     {{0, true}, {9, true}, {18, true}, {19, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_uci.beta_offset_csi2 = value; },
     "Beta offset CSI Part2",
     {{0, true}, {9, true}, {18, true}, {19, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ptrs.port_info.back().ptrs_dmrs_port = value; },
     "PTRS DMRS port",
     {{0, true}, {5, true}, {11, true}, {12, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ptrs.port_info.back().ptrs_re_offset = value; },
     "PTRS RE offset",
     {{0, true}, {5, true}, {11, true}, {12, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ptrs.ptrs_time_density = value; },
     "PTRS time density",
     {{0, true}, {1, true}, {2, true}, {3, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ptrs.ptrs_freq_density = value; },
     "PTRS frequency density",
     {{0, true}, {1, true}, {2, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ptrs.ul_ptrs_power = static_cast<ul_ptrs_power_type>(value); },
     "UL PTRS power",
     {{0, true}, {2, true}, {3, true}, {4, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ofdm.low_papr_group_number = value; },
     "Low PAPR group number",
     {{0, true}, {15, true}, {29, true}, {30, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ofdm.ul_ptrs_sample_density = value; },
     "UL PTRS sample density",
     {{0, false}, {1, true}, {4, true}, {8, true}, {9, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_ofdm.ul_ptrs_time_density_transform_precoding = value; },
     "UL PTRS time density transform precoding",
     {{0, false}, {1, true}, {2, true}, {4, true}, {5, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_maintenance_v3.pusch_trans_type = value; },
     "PUSCH trans type",
     {{0, true}, {2, true}, {2, true}, {3, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_maintenance_v3.delta_bwp0_start_from_active_bwp = value; },
     "Delta BWP0 start from active BWP",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_maintenance_v3.initial_ul_bwp_size = value; },
     "Initial UL BWP size",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_maintenance_v3.group_or_sequence_hopping = value; },
     "Group or sequence hopping",
     {{0, true}, {2, true}, {2, true}, {3, false}}},
    {[](ul_pusch_pdu& pdu, int value) { pdu.pusch_maintenance_v3.pusch_second_hop_prb = value; },
     "PUSCH second hop PRB",
     {{0, true}, {128, true}, {274, true}, {275, false}}},
    {[](ul_pusch_pdu& pdu, int value) {
       pdu.pusch_maintenance_v3.ldpc_base_graph = static_cast<ldpc_base_graph_type>(value);
     },
     "LDPC base graph",
     {{0, false}, {1, true}, {2, true}, {3, false}}},
    {[](ul_pusch_pdu& pdu, int value) {
       pdu.uci_correspondence.part2.back().part2_size_map_scope =
           static_cast<uci_part1_to_part2_correspondence_v3::map_scope_type>(value);
     },
     "Part2 size map scope",
     {{0, true}, {1, true}, {2, false}}}};

static void test_validate_each_field_error()
{
  for (auto& group : vector_test) {
    for (const auto& test_case : group) {
      validator_report report(0, 0);
      auto             pdu = build_valid_ul_pusch_pdu();
      group.update_msg(pdu, test_case.value);
      bool result = validate_ul_pusch_pdu(pdu, report);

      TESTASSERT_EQ(result, test_case.result);
      if (!result) {
        TESTASSERT_EQ(1U, report.reports.size());
        const auto& rep = report.reports.back();
        TESTASSERT_EQ(std::strcmp(group.property(), rep.property_name), 0);
        TESTASSERT_EQ(message_type_id::ul_tti_request, rep.message_type);
        TESTASSERT_EQ(ul_pdu_type::PUSCH, static_cast<ul_pdu_type>(rep.pdu_type.value()));
      } else {
        TESTASSERT(report.reports.empty());
      }
    }
  }
}

static void test_validate_more_that_one_error_simultaneously()
{
  validator_report report(0, 0);
  auto             pdu = build_valid_ul_pusch_pdu();

  // Add 4 errors.
  pdu.bwp_start      = 6500;
  pdu.bwp_size       = 2133;
  pdu.nr_of_symbols  = 32;
  pdu.resource_alloc = static_cast<resource_allocation_type>(54);

  TESTASSERT(!validate_ul_pusch_pdu(pdu, report));
  TESTASSERT_EQ(4U, report.reports.size());
  // Check that the properties that caused the error are different.
  for (unsigned i = 0, e = report.reports.size(); i != e; ++i) {
    for (unsigned j = i + 1; j != e; ++j) {
      TESTASSERT(std::strcmp(report.reports[i].property_name, report.reports[j].property_name) != 0);
    }
  }
}

static void test_validate_pusch_pdu_ok()
{
  validator_report report(0, 0);
  const auto&      pdu = build_valid_ul_pusch_pdu();
  TESTASSERT(validate_ul_pusch_pdu(pdu, report));
  TESTASSERT(report.reports.empty());
}

int main()
{
  test_validate_pusch_pdu_ok();
  test_validate_each_field_error();
  test_validate_more_that_one_error_simultaneously();
  fmt::print("UL PUSCH PDU validator -> OK\n");
}