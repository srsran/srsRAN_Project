/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "config_generators.h"
#include "lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "lib/mac/mac_ctrl/sched_config_helpers.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "scheduler_test_suite.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsgnb;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Expected results parameters.
struct expected_output_params {
  pucch_format      format;
  prb_interval      prbs;
  prb_interval      second_hop_prbs;
  ofdm_symbol_range symbols;
  bool              intra_slot_freq_hop;
  uint8_t           initial_cyclic_shift;
  sr_nof_bits       sr_bits;
  unsigned          harq_ack_nof_bits;
  uint8_t           time_domain_occ;
};

// Parameters for PUCCH scheduler initialization.
struct pucch_params {
  // Input parameters.
  unsigned pucch_res_common;
  unsigned n_cces;
};

// Parameters to be passed to test for PUCCH output assessment.
struct pucch_test_parameters {
  unsigned               dci_pucch_res_indicator;
  unsigned               dci_harq_timining_indicator;
  expected_output_params output_params;
  pucch_params           pucch_input_params;
};

////////////    Builder of PUCCH scheduler output     ////////////

// Class that builds the PUCCH scheduler output for test.
class pucch_output_test_builder
{
public:
  pucch_output_test_builder(const bwp_configuration* bwp_cfg,
                            unsigned                 pci_,
                            pucch_format             format,
                            prb_interval             prbs,
                            prb_interval             second_hop_prbs,
                            ofdm_symbol_range        symbols,
                            bool                     intra_slot_freq_hop,
                            uint8_t                  initial_cyclic_shift,
                            sr_nof_bits              sr_bits,
                            unsigned                 harq_ack_nof_bits,
                            uint8_t                  time_domain_occ) :
    pci(pci_)
  {
    pucch_test.crnti   = to_rnti(0x4601);
    pucch_test.bwp_cfg = bwp_cfg;

    if (format == pucch_format::FORMAT_0) {
      build_format_0(
          prbs, second_hop_prbs, symbols, intra_slot_freq_hop, initial_cyclic_shift, sr_bits, harq_ack_nof_bits);
    } else {
      build_format_1(prbs,
                     second_hop_prbs,
                     symbols,
                     intra_slot_freq_hop,
                     initial_cyclic_shift,
                     sr_bits,
                     harq_ack_nof_bits,
                     time_domain_occ);
    }
  }

  const pucch_info& get_expected_pucch_output() const { return pucch_test; };

private:
  void build_format_0(prb_interval      prbs,
                      prb_interval      second_hop_prbs,
                      ofdm_symbol_range symbols,
                      bool              intra_slot_freq_hop,
                      uint8_t           initial_cyclic_shift,
                      sr_nof_bits       sr_bits,
                      unsigned          harq_ack_nof_bits);

  void build_format_1(prb_interval      prbs,
                      prb_interval      second_hop_prbs,
                      ofdm_symbol_range symbols,
                      bool              intra_slot_freq_hop,
                      uint8_t           initial_cyclic_shift,
                      sr_nof_bits       sr_bits,
                      unsigned          harq_ack_nof_bits,
                      uint8_t           time_domain_occ);

  pucch_info pucch_test;
  unsigned   pci;
};

void pucch_output_test_builder::build_format_0(prb_interval      prbs,
                                               prb_interval      second_hop_prbs,
                                               ofdm_symbol_range symbols,
                                               bool              intra_slot_freq_hop,
                                               uint8_t           initial_cyclic_shift,
                                               sr_nof_bits       sr_bits,
                                               unsigned          harq_ack_nof_bits)
{
  pucch_test.resources.prbs                = prbs;
  pucch_test.resources.second_hop_prbs     = second_hop_prbs;
  pucch_test.resources.symbols             = symbols;
  pucch_test.resources.intra_slot_freq_hop = intra_slot_freq_hop;

  pucch_test.format_0.initial_cyclic_shift = initial_cyclic_shift;
  pucch_test.format_0.sr_bits              = sr_bits;
  pucch_test.format_0.harq_ack_nof_bits    = harq_ack_nof_bits;

  pucch_test.format_0.group_hopping = pucch_group_hopping::NEITHER;
  pucch_test.format                 = pucch_format::FORMAT_0;
  pucch_test.format_0.n_id_hopping  = pci;
}

void pucch_output_test_builder::build_format_1(prb_interval      prbs,
                                               prb_interval      second_hop_prbs,
                                               ofdm_symbol_range symbols,
                                               bool              intra_slot_freq_hop,
                                               uint8_t           initial_cyclic_shift,
                                               sr_nof_bits       sr_bits,
                                               unsigned          harq_ack_nof_bits,
                                               uint8_t           time_domain_occ)
{
  pucch_test.resources.prbs                = prbs;
  pucch_test.resources.second_hop_prbs     = second_hop_prbs;
  pucch_test.resources.symbols             = symbols;
  pucch_test.resources.intra_slot_freq_hop = intra_slot_freq_hop;

  pucch_test.format_1.initial_cyclic_shift = initial_cyclic_shift;
  pucch_test.format_1.sr_bits              = sr_bits;
  pucch_test.format_1.harq_ack_nof_bits    = harq_ack_nof_bits;
  pucch_test.format_1.time_domain_occ      = time_domain_occ;

  pucch_test.format_1.group_hopping   = pucch_group_hopping::NEITHER;
  pucch_test.format                   = pucch_format::FORMAT_1;
  pucch_test.format_1.n_id_hopping    = pci;
  pucch_test.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
}

////////////      Utilities      ////////

// Verify if the PUCCH scheduler output (or PUCCH PDU) is correct.
bool assess_ul_pucch_info(const pucch_info& expected, const pucch_info& test)
{
  bool is_equal = expected.crnti == test.crnti && *expected.bwp_cfg == *test.bwp_cfg && expected.format == test.format;
  is_equal      = is_equal && expected.resources.prbs == test.resources.prbs &&
             expected.resources.symbols == test.resources.symbols &&
             expected.resources.intra_slot_freq_hop == test.resources.intra_slot_freq_hop;
  if (expected.resources.intra_slot_freq_hop == true) {
    is_equal = is_equal && expected.resources.second_hop_prbs == test.resources.second_hop_prbs;
  }
  switch (expected.format) {
    case pucch_format::FORMAT_0: {
      const pucch_format_0& expected_f = expected.format_0;
      const pucch_format_0& test_f     = test.format_0;
      is_equal                         = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected_f.sr_bits == test_f.sr_bits && expected_f.harq_ack_nof_bits == test_f.harq_ack_nof_bits;

      break;
    }
    case pucch_format::FORMAT_1: {
      const pucch_format_1& expected_f = expected.format_1;
      const pucch_format_1& test_f     = test.format_1;
      is_equal                         = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected_f.sr_bits == test_f.sr_bits && expected_f.harq_ack_nof_bits == test_f.harq_ack_nof_bits &&
                 expected_f.slot_repetition == test_f.slot_repetition &&
                 expected_f.time_domain_occ == test_f.time_domain_occ;
      break;
    }
    default: {
      return false;
    };
  }

  return is_equal;
}

// Builds a default DCI with a given n_CCEs.
static pdcch_dl_information make_default_dci(unsigned n_cces, const coreset_configuration* coreset_cfg)
{
  pdcch_dl_information dci;
  dci.ctx.cces.ncce   = n_cces;
  dci.ctx.coreset_cfg = coreset_cfg;
  return dci;
}

// Makes a \c sched_cell_configuration_request_message object with a given \c pucch_res_common parameter.
static sched_cell_configuration_request_message make_custom_sched_cell_configuration_request(unsigned pucch_res_common)
{
  sched_cell_configuration_request_message cell_req = make_default_sched_cell_configuration_request();
  cell_req.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = pucch_res_common;
  return cell_req;
}

/////////        TEST BENCH for PUCCH scheduler        /////////

// Test bench with all that is needed for the PUCCH.
class test_bench
{
public:
  test_bench(unsigned pucch_res_common, unsigned n_cces) :
    cell_req{make_custom_sched_cell_configuration_request(pucch_res_common)},
    cell_cfg{cell_req},
    user{cell_cfg, make_ue_creation_request(test_helpers::make_default_ue_creation_request())},
    ue_cell_cfg{cell_cfg, serving_cell_ue_configuration_request{}},
    coreset_cfg{config_helpers::make_default_coreset_config()},
    dci_info{make_default_dci(n_cces, &coreset_cfg)},
    k0{0},
    pucch_sched{cell_cfg},
    sl_tx{to_numerology_value(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs), 0}
  {
    res_grid.slot_indication(sl_tx);
  }

  // Variables that are needed to generate the PUCCH scheduler.
  sched_cell_configuration_request_message cell_req;
  cell_configuration                       cell_cfg;
  // Variables that are needed for calling the PUCCH allocator.
  cell_resource_allocator res_grid{cell_cfg};
  ue                      user;
  ue_cell_configuration   ue_cell_cfg;
  coreset_configuration   coreset_cfg;
  pdcch_dl_information    dci_info;
  unsigned                k0;
  // Variables that are needed for the PUCCH scheduler.
  pucch_allocator_impl pucch_sched;
  slot_point           sl_tx;
  // Let's assume k1 is 4 slots.
  unsigned tx_delay{4};
};

class test_pucch_output : public ::testing::TestWithParam<pucch_test_parameters>
{
public:
  test_pucch_output() :
    params{GetParam()},
    t_bench(params.pucch_input_params.pucch_res_common, params.pucch_input_params.n_cces),
    pucch_output_builder{&t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params,
                         static_cast<unsigned>(t_bench.cell_cfg.pci),
                         params.output_params.format,
                         params.output_params.prbs,
                         params.output_params.second_hop_prbs,
                         params.output_params.symbols,
                         params.output_params.intra_slot_freq_hop,
                         params.output_params.initial_cyclic_shift,
                         params.output_params.sr_bits,
                         params.output_params.harq_ack_nof_bits,
                         params.output_params.time_domain_occ}
  {
    pucch_expected = pucch_output_builder.get_expected_pucch_output();
  };

protected:
  // Parameters that are passed by the routing to run the tests.
  pucch_test_parameters     params;
  test_bench                t_bench;
  pucch_output_test_builder pucch_output_builder;
  pucch_info                pucch_expected;
  // Expected \c pucch_res_indicator value returned by the scheduler.
  unsigned pucch_res_indicator;
  // Expected \c harq_timing_indicator value returned by the scheduler.
  unsigned harq_timing_indicator;
};

// Tests the output of the PUCCH allocator (or PUCCH PDU).
TEST_P(test_pucch_output, test_pucch_output_info)
{
  pucch_info* pucch_test = t_bench.pucch_sched.alloc_pucch_harq_ack_ue(pucch_res_indicator,
                                                                       harq_timing_indicator,
                                                                       t_bench.res_grid,
                                                                       t_bench.k0,
                                                                       t_bench.dci_info,
                                                                       t_bench.user,
                                                                       t_bench.ue_cell_cfg);

  ASSERT_TRUE(pucch_test != nullptr);
  ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, *pucch_test));
}

// Tests whether PUCCH allocator fills the scheduler grid correctly.
TEST_P(test_pucch_output, test_pucch_grid_filling)
{
  t_bench.pucch_sched.alloc_pucch_harq_ack_ue(pucch_res_indicator,
                                              harq_timing_indicator,
                                              t_bench.res_grid,
                                              t_bench.k0,
                                              t_bench.dci_info,
                                              t_bench.user,
                                              t_bench.ue_cell_cfg);

  ASSERT_TRUE(assert_ul_resource_grid_filled(t_bench.cell_cfg, t_bench.res_grid, t_bench.tx_delay));
}

// Tests whether PUCCH allocator returns the correct values for the DCI.
TEST_P(test_pucch_output, test_pucch_output_for_dci)
{
  t_bench.pucch_sched.alloc_pucch_harq_ack_ue(pucch_res_indicator,
                                              harq_timing_indicator,
                                              t_bench.res_grid,
                                              t_bench.k0,
                                              t_bench.dci_info,
                                              t_bench.user,
                                              t_bench.ue_cell_cfg);

  ASSERT_EQ(GetParam().dci_pucch_res_indicator, pucch_res_indicator);
  ASSERT_EQ(GetParam().dci_harq_timining_indicator, harq_timing_indicator);
}

INSTANTIATE_TEST_SUITE_P(
    test_pucch_output_for_dci,
    test_pucch_output,
    testing::Values(pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_0,
                                                                                  prb_interval{0, 1},
                                                                                  prb_interval{51, 52},
                                                                                  ofdm_symbol_range{12, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{0, 1}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_0,
                                                                                  prb_interval{3, 4},
                                                                                  prb_interval{48, 49},
                                                                                  ofdm_symbol_range{12, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{2, 1}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_0,
                                                                                  prb_interval{3, 4},
                                                                                  prb_interval{48, 49},
                                                                                  ofdm_symbol_range{12, 14},
                                                                                  true,
                                                                                  8,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{2, 6}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{4, 5},
                                                                                  prb_interval{47, 48},
                                                                                  ofdm_symbol_range{4, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{10, 0}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{4, 5},
                                                                                  prb_interval{47, 48},
                                                                                  ofdm_symbol_range{4, 14},
                                                                                  true,
                                                                                  6,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{10, 6}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{0, 1},
                                                                                  prb_interval{51, 52},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{11, 0}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{1, 2},
                                                                                  prb_interval{50, 51},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{11, 6}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{13, 14},
                                                                                  prb_interval{38, 39},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  0,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{15, 0}},
                    pucch_test_parameters{.dci_pucch_res_indicator     = 0,
                                          .dci_harq_timining_indicator = 3,
                                          .output_params               = expected_output_params{pucch_format::FORMAT_1,
                                                                                  prb_interval{13, 14},
                                                                                  prb_interval{38, 39},
                                                                                  ofdm_symbol_range{0, 14},
                                                                                  true,
                                                                                  6,
                                                                                  sr_nof_bits::no_sr,
                                                                                  1,
                                                                                  0},
                                          .pucch_input_params          = pucch_params{15, 6}})

);

int main(int argc, char** argv)
{
  srslog::fetch_basic_logger("MAC").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::info);
  srslog::init();

  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
