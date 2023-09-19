/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "uci_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

////////////    Structs with expected parameters and PUCCH sched INPUT     ////////////

// Class to test PUCCH schedule with SR occasions only.
class test_pucch_output_sr_only : public ::testing::TestWithParam<sr_periodicity>
{
public:
  test_pucch_output_sr_only() :
    sr_period(GetParam()),
    sr_offset(test_rgen::uniform_int<unsigned>(0, sr_periodicity_to_slot(GetParam()) - 1)),
    t_bench{test_bench_params{.pucch_res_common = pucch_res_common,
                              .n_cces           = n_cces,
                              .period           = sr_period,
                              .offset           = sr_offset}}
  {
    csi_offset = variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(t_bench.get_main_ue()
                                                                                                 .get_pcell()
                                                                                                 .cfg()
                                                                                                 .cfg_dedicated()
                                                                                                 .csi_meas_cfg.value()
                                                                                                 .csi_report_cfg_list[0]
                                                                                                 .report_cfg_type)
                     .report_slot_offset;

    csi_period = variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(t_bench.get_main_ue()
                                                                                                 .get_pcell()
                                                                                                 .cfg()
                                                                                                 .cfg_dedicated()
                                                                                                 .csi_meas_cfg.value()
                                                                                                 .csi_report_cfg_list[0]
                                                                                                 .report_cfg_type)
                     .report_slot_period;

    // Set expected grant for PUCCH Format 1.
    pucch_expected.format  = srsran::pucch_format::FORMAT_1;
    pucch_expected.crnti   = to_rnti(0x4601);
    pucch_expected.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected.resources.prbs            = prb_interval{49, 50};
    pucch_expected.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected.resources.symbols         = ofdm_symbol_range{0, 14};

    pucch_expected.format_1.initial_cyclic_shift = 0;
    pucch_expected.format_1.sr_bits              = sr_nof_bits::one;
    pucch_expected.format_1.harq_ack_nof_bits    = 0;
    pucch_expected.format_1.time_domain_occ      = 0;

    pucch_expected.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_expected.format                   = pucch_format::FORMAT_1;
    pucch_expected.format_1.n_id_hopping    = t_bench.cell_cfg.pci;
    pucch_expected.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

    pucch_expected_f2.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_f2.crnti                     = to_rnti(0x4601);
    pucch_expected_f2.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_f2.resources.prbs            = prb_interval{2, 3};
    pucch_expected_f2.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f2.resources.symbols         = ofdm_symbol_range{12, 14};

    pucch_expected_f2.format_2.harq_ack_nof_bits = 0;
    pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
    pucch_expected_f2.format_2.csi_part1_bits    = 4;
    pucch_expected_f2.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_f2.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_f2.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;
  };

protected:
  // Parameters that are passed by the routing to run the tests.
  sr_periodicity sr_period;
  unsigned       sr_offset;
  // Helper variables.
  csi_report_periodicity csi_period;
  unsigned               csi_offset;
  unsigned               pucch_res_common{11};
  unsigned               n_cces{0};
  test_bench             t_bench;
  // Expected SR PUCCH PDU that would be passed to PHY.
  pucch_info pucch_expected;
  pucch_info pucch_expected_f2;
};

// Class to test PUCCH schedule with SR occasions & HARQ-ACK.
class test_pucch_output_sr_harq : public ::testing::TestWithParam<sr_periodicity>
{
public:
  test_pucch_output_sr_harq() :
    sr_period(GetParam()),
    sr_offset(test_rgen::uniform_int<unsigned>(0, sr_periodicity_to_slot(GetParam()) - 1)),
    t_bench{test_bench_params{.pucch_res_common = pucch_res_common,
                              .n_cces           = n_cces,
                              .period           = sr_period,
                              .offset           = sr_offset}}
  {
    csi_offset = variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(t_bench.get_main_ue()
                                                                                                 .get_pcell()
                                                                                                 .cfg()
                                                                                                 .cfg_dedicated()
                                                                                                 .csi_meas_cfg.value()
                                                                                                 .csi_report_cfg_list[0]
                                                                                                 .report_cfg_type)
                     .report_slot_offset;

    csi_period = variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(t_bench.get_main_ue()
                                                                                                 .get_pcell()
                                                                                                 .cfg()
                                                                                                 .cfg_dedicated()
                                                                                                 .csi_meas_cfg.value()
                                                                                                 .csi_report_cfg_list[0]
                                                                                                 .report_cfg_type)
                     .report_slot_period;

    // Set expected grant for PUCCH Format 1.
    pucch_expected.format  = srsran::pucch_format::FORMAT_1;
    pucch_expected.crnti   = to_rnti(0x4601);
    pucch_expected.bwp_cfg = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

    pucch_expected.resources.prbs            = prb_interval{49, 50};
    pucch_expected.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected.resources.symbols         = ofdm_symbol_range{0, 14};

    pucch_expected.format_1.initial_cyclic_shift = 0;
    pucch_expected.format_1.sr_bits              = sr_nof_bits::one;
    pucch_expected.format_1.harq_ack_nof_bits    = 1;
    pucch_expected.format_1.time_domain_occ      = 0;

    pucch_expected.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_expected.format                   = pucch_format::FORMAT_1;
    pucch_expected.format_1.n_id_hopping    = t_bench.cell_cfg.pci;
    pucch_expected.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;

    pucch_expected_csi.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_csi.crnti                     = to_rnti(0x4601);
    pucch_expected_csi.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_csi.resources.prbs            = prb_interval{2, 3};
    pucch_expected_csi.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_csi.resources.symbols         = ofdm_symbol_range{12, 14};

    pucch_expected_csi.format_2.harq_ack_nof_bits = 0;
    pucch_expected_csi.format_2.sr_bits           = sr_nof_bits::one;
    pucch_expected_csi.format_2.csi_part1_bits    = 4;
    pucch_expected_csi.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_csi.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_csi.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;

    pucch_expected_f2.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_f2.crnti                     = to_rnti(0x4601);
    pucch_expected_f2.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_f2.resources.prbs            = prb_interval{2, 3};
    pucch_expected_f2.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_f2.resources.symbols         = ofdm_symbol_range{0, 2};

    pucch_expected_f2.format_2.harq_ack_nof_bits = 1;
    pucch_expected_f2.format_2.sr_bits           = sr_nof_bits::one;
    pucch_expected_f2.format_2.csi_part1_bits    = 4;
    pucch_expected_f2.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_f2.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_f2.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;
  };

protected:
  // Parameters that are passed by the routing to run the tests.
  sr_periodicity sr_period;
  unsigned       sr_offset;
  // Helper variables.
  csi_report_periodicity csi_period;
  unsigned               csi_offset;
  unsigned               pucch_res_common{11};
  unsigned               n_cces{0};
  test_bench             t_bench;
  // Expected SR PUCCH PDU that would be passed to PHY.
  pucch_info pucch_expected;
  pucch_info pucch_expected_csi;
  pucch_info pucch_expected_f2;
};

// Tests the PUCCH scheduler with SR opportunities only.

TEST_P(test_pucch_output_sr_only, test_pucch_sr_only)

{
  const unsigned NOF_SLOTS_TO_TEST = sr_periodicity_to_slot(sr_period) * 4;

  for (unsigned sl_cnt = 0; sl_cnt < NOF_SLOTS_TO_TEST; ++sl_cnt) {
    // t_bench.pucch_alloc.slot_indication(t_bench.sl_tx);
    t_bench.uci_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);
    if ((t_bench.sl_tx - sr_offset).to_uint() % sr_periodicity_to_slot(sr_period) == 0) {
      ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
      // The SR will be mixed with CSI. When there is also a CSI scheduled, then
      if ((t_bench.sl_tx - csi_offset).to_uint() % csi_report_periodicity_to_uint(csi_period) == 0) {
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, t_bench.res_grid[0].result.ul.pucchs.back()));
      } else {
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
      }
    }
    // Update the slot indicator.
    t_bench.slot_indication(++t_bench.sl_tx);
  }
}

TEST_P(test_pucch_output_sr_harq, test_pucch_sr_harq)
{
  const unsigned NOF_SLOTS_TO_TEST = sr_periodicity_to_slot(sr_period) * 4;

  for (unsigned sl_cnt = 0; sl_cnt < NOF_SLOTS_TO_TEST; ++sl_cnt) {
    // To avoid that the HARQ-ACK grant gets scheduled with some delay (which complicates the checks in this test),
    // assume k1 = 0. This way, the HARQ-ACK grant will get scheduled at each slot, starting from 0.
    t_bench.pucch_alloc.alloc_ded_pucch_harq_ack_ue(
        t_bench.res_grid, t_bench.get_main_ue().crnti, t_bench.get_main_ue().get_pcell().cfg(), t_bench.k0, 0);
    t_bench.uci_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);

    if ((t_bench.sl_tx - sr_offset).to_uint() % sr_periodicity_to_slot(sr_period) == 0) {
      // Check if this slot corresponds to an CSI opportunity for the UE.
      if ((t_bench.sl_tx - csi_offset).to_uint() % csi_report_periodicity_to_uint(csi_period) == 0) {
        ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_f2, t_bench.res_grid[0].result.ul.pucchs.back()));
      } else {
        ASSERT_EQ(2, t_bench.res_grid[0].result.ul.pucchs.size());
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected, t_bench.res_grid[0].result.ul.pucchs.back()));
      }
    }
    // Update the slot indicator.
    t_bench.slot_indication(++t_bench.sl_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(test_pucch_output_for_dci,
                         test_pucch_output_sr_only,
                         testing::Values(sr_periodicity::sl_1,
                                         sr_periodicity::sl_2,
                                         sr_periodicity::sl_4,
                                         sr_periodicity::sl_5,
                                         sr_periodicity::sl_8,
                                         sr_periodicity::sl_16,
                                         sr_periodicity::sl_20,
                                         sr_periodicity::sl_40,
                                         sr_periodicity::sl_80,
                                         sr_periodicity::sl_160,
                                         sr_periodicity::sl_320,
                                         sr_periodicity::sl_640));

INSTANTIATE_TEST_SUITE_P(test_pucch_output_for_dci,
                         test_pucch_output_sr_harq,
                         testing::Values(sr_periodicity::sl_1,
                                         sr_periodicity::sl_2,
                                         sr_periodicity::sl_4,
                                         sr_periodicity::sl_5,
                                         sr_periodicity::sl_8,
                                         sr_periodicity::sl_16,
                                         sr_periodicity::sl_20,
                                         sr_periodicity::sl_40,
                                         sr_periodicity::sl_80,
                                         sr_periodicity::sl_160,
                                         sr_periodicity::sl_320,
                                         sr_periodicity::sl_640));

/////////////  Test CSI scheduling

class test_uci_csi_scheduling : public ::testing::TestWithParam<csi_report_periodicity>
{
public:
  test_uci_csi_scheduling() :
    csi_period(GetParam()),
    csi_offset(test_rgen::uniform_int<unsigned>(0, csi_report_periodicity_to_uint(GetParam()) - 1)),
    t_bench{test_bench_params{.csi_period = csi_period, .csi_offset = csi_offset}}
  {
    sr_period = sr_periodicity_to_slot(t_bench.get_main_ue()
                                           .get_pcell()
                                           .cfg()
                                           .cfg_dedicated()
                                           .ul_config.value()
                                           .init_ul_bwp.pucch_cfg.value()
                                           .sr_res_list[0]
                                           .period);

    sr_offset = t_bench.get_main_ue()
                    .get_pcell()
                    .cfg()
                    .cfg_dedicated()
                    .ul_config.value()
                    .init_ul_bwp.pucch_cfg.value()
                    .sr_res_list[0]
                    .offset;

    // Set expected grant for PUCCH Format 1.
    pucch_expected_sr.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_sr.crnti                     = to_rnti(0x4601);
    pucch_expected_sr.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_sr.resources.prbs            = prb_interval{2, 3};
    pucch_expected_sr.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_sr.resources.symbols         = ofdm_symbol_range{12, 14};

    pucch_expected_sr.format_2.harq_ack_nof_bits = 0;
    pucch_expected_sr.format_2.sr_bits           = sr_nof_bits::one;
    pucch_expected_sr.format_2.csi_part1_bits    = 4;
    pucch_expected_sr.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_sr.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_sr.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;

    pucch_expected_no_sr.format                    = srsran::pucch_format::FORMAT_2;
    pucch_expected_no_sr.crnti                     = to_rnti(0x4601);
    pucch_expected_no_sr.bwp_cfg                   = &t_bench.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_expected_no_sr.resources.prbs            = prb_interval{2, 3};
    pucch_expected_no_sr.resources.second_hop_prbs = prb_interval{0, 0};
    pucch_expected_no_sr.resources.symbols         = ofdm_symbol_range{12, 14};

    pucch_expected_no_sr.format_2.harq_ack_nof_bits = 0;
    pucch_expected_no_sr.format_2.sr_bits           = sr_nof_bits::no_sr;
    pucch_expected_no_sr.format_2.csi_part1_bits    = 4;
    pucch_expected_no_sr.format_2.max_code_rate     = max_pucch_code_rate::dot_25;
    pucch_expected_no_sr.format_2.n_id_scambling    = t_bench.cell_cfg.pci;
    pucch_expected_no_sr.format_2.n_id_0_scrambling = t_bench.cell_cfg.pci;
  };

protected:
  // Parameters that are passed by the routing to run the tests.
  csi_report_periodicity csi_period;
  unsigned               csi_offset;
  // Helper variables.
  unsigned   sr_period;
  unsigned   sr_offset{0};
  test_bench t_bench;
  // Expected SR PUCCH PDU that would be passed to PHY.
  pucch_info pucch_expected_sr;
  pucch_info pucch_expected_no_sr;
};

TEST_P(test_uci_csi_scheduling, test_csi_sched)
{
  const unsigned NOF_SLOTS_TO_TEST = csi_report_periodicity_to_uint(csi_period) * 8;

  for (unsigned sl_cnt = 0; sl_cnt < NOF_SLOTS_TO_TEST; ++sl_cnt) {
    t_bench.uci_sched.run_slot(t_bench.res_grid, t_bench.sl_tx);
    if ((t_bench.sl_tx - csi_offset).to_uint() % csi_report_periodicity_to_uint(csi_period) == 0) {
      ASSERT_EQ(1, t_bench.res_grid[0].result.ul.pucchs.size());
      if ((t_bench.sl_tx - sr_offset).to_uint() % sr_period == 0) {
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_sr, t_bench.res_grid[0].result.ul.pucchs.back()));
      } else {
        ASSERT_TRUE(assess_ul_pucch_info(pucch_expected_no_sr, t_bench.res_grid[0].result.ul.pucchs.back()));
      }
    }
    // Update the slot indicator.
    t_bench.slot_indication(++t_bench.sl_tx);
  }
}

INSTANTIATE_TEST_SUITE_P(test_csi_sched_different_periods_offsets,
                         test_uci_csi_scheduling,
                         testing::Values(csi_report_periodicity::slots4,
                                         csi_report_periodicity::slots5,
                                         csi_report_periodicity::slots8,
                                         csi_report_periodicity::slots10,
                                         csi_report_periodicity::slots16,
                                         csi_report_periodicity::slots20,
                                         csi_report_periodicity::slots40,
                                         csi_report_periodicity::slots80,
                                         csi_report_periodicity::slots160,
                                         csi_report_periodicity::slots320));
