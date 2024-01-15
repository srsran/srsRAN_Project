/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "uci_test_utils.h"

using namespace srsran;

pucch_info srsran::build_pucch_info(const bwp_configuration* bwp_cfg,
                                    unsigned                 pci,
                                    pucch_format             format,
                                    prb_interval             prbs,
                                    prb_interval             second_hop_prbs,
                                    ofdm_symbol_range        symbols,
                                    uint8_t                  initial_cyclic_shift,
                                    sr_nof_bits              sr_bits,
                                    unsigned                 harq_ack_nof_bits,
                                    uint8_t                  time_domain_occ)
{
  pucch_info pucch_test{.crnti = to_rnti(0x4601), .bwp_cfg = bwp_cfg};

  if (format == pucch_format::FORMAT_0) {
    pucch_test.resources.prbs            = prbs;
    pucch_test.resources.second_hop_prbs = second_hop_prbs;
    pucch_test.resources.symbols         = symbols;

    pucch_test.format_0.initial_cyclic_shift = initial_cyclic_shift;
    pucch_test.format_0.sr_bits              = sr_bits;
    pucch_test.format_0.harq_ack_nof_bits    = harq_ack_nof_bits;

    pucch_test.format_0.group_hopping = pucch_group_hopping::NEITHER;
    pucch_test.format                 = pucch_format::FORMAT_0;
    pucch_test.format_0.n_id_hopping  = pci;
  } else if (format == pucch_format::FORMAT_1) {
    pucch_test.resources.prbs            = prbs;
    pucch_test.resources.second_hop_prbs = second_hop_prbs;
    pucch_test.resources.symbols         = symbols;

    pucch_test.format_1.initial_cyclic_shift = initial_cyclic_shift;
    pucch_test.format_1.sr_bits              = sr_bits;
    pucch_test.format_1.harq_ack_nof_bits    = harq_ack_nof_bits;
    pucch_test.format_1.time_domain_occ      = time_domain_occ;

    pucch_test.format_1.group_hopping   = pucch_group_hopping::NEITHER;
    pucch_test.format                   = pucch_format::FORMAT_1;
    pucch_test.format_1.n_id_hopping    = pci;
    pucch_test.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
  } else {
    return pucch_info{};
  }

  return pucch_test;
}

// Verify if the PUCCH scheduler output (or PUCCH PDU) is correct.
bool srsran::assess_ul_pucch_info(const pucch_info& expected, const pucch_info& test)
{
  bool is_equal = expected.crnti == test.crnti && *expected.bwp_cfg == *test.bwp_cfg && expected.format == test.format;
  is_equal      = is_equal && expected.resources.prbs == test.resources.prbs &&
             expected.resources.symbols == test.resources.symbols &&
             expected.resources.second_hop_prbs == test.resources.second_hop_prbs;

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
    case pucch_format::FORMAT_2: {
      const pucch_format_2& expected_f = expected.format_2;
      const pucch_format_2& test_f     = test.format_2;
      is_equal                         = is_equal && expected_f.max_code_rate == test_f.max_code_rate &&
                 expected_f.n_id_scambling == test_f.n_id_scambling &&
                 expected_f.n_id_0_scrambling == test_f.n_id_0_scrambling && expected_f.sr_bits == test_f.sr_bits &&
                 expected_f.harq_ack_nof_bits == test_f.harq_ack_nof_bits &&
                 expected_f.csi_part1_bits == test_f.csi_part1_bits;
      break;
    }
    default: {
      return false;
    };
  }

  return is_equal;
}

/////////        TEST BENCH for PUCCH scheduler        /////////

// Test bench with all that is needed for the PUCCH.

test_bench::test_bench(const test_bench_params& params,
                       unsigned                 max_pucchs_per_slot_,
                       unsigned                 max_ul_grants_per_slot_) :
  expert_cfg{config_helpers::make_default_scheduler_expert_config()},
  cell_cfg{[this, &params]() -> const cell_configuration& {
    cell_cfg_list.emplace(
        to_du_cell_index(0),
        std::make_unique<cell_configuration>(
            expert_cfg, make_custom_sched_cell_configuration_request(params.pucch_res_common, params.is_tdd)));
    return *cell_cfg_list[to_du_cell_index(0)];
  }()},
  dci_info{make_default_dci(params.n_cces, &cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value())},
  k0(cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0),
  max_pucchs_per_slot{max_pucchs_per_slot_},
  max_ul_grants_per_slot{max_ul_grants_per_slot_},
  pucch_f2_more_prbs{params.pucch_f2_more_prbs},
  pucch_alloc{cell_cfg, max_pucchs_per_slot, max_ul_grants_per_slot},
  uci_alloc(pucch_alloc),
  uci_sched{cell_cfg, uci_alloc, ues},
  sl_tx{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
{
  cell_config_builder_params cfg_params{};
  cfg_params.csi_rs_enabled                = true;
  cfg_params.scs_common                    = params.is_tdd ? subcarrier_spacing::kHz30 : subcarrier_spacing::kHz15;
  cfg_params.dl_arfcn                      = params.is_tdd ? 520000U : 365000U;
  sched_ue_creation_request_message ue_req = test_helpers::create_default_sched_ue_creation_request(cfg_params);
  ue_req.ue_index                          = main_ue_idx;

  srsran_assert(
      ue_req.cfg.cells.has_value() and not ue_req.cfg.cells->empty() and
          ue_req.cfg.cells->back().serv_cell_cfg.ul_config.has_value() and
          ue_req.cfg.cells->back().serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.has_value() and
          ue_req.cfg.cells->back().serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg->sr_res_list.size() == 1,
      "sched_ue_creation_request_message initialization is not complete.");

  // Add custom PUCCH config from this test file.
  ue_req.cfg.cells->back().serv_cell_cfg.ul_config = test_helpers::make_test_ue_uplink_config(cfg_params);

  auto& ul_cfg = ue_req.cfg.cells->back().serv_cell_cfg.ul_config.value();

  ul_cfg.init_ul_bwp.pucch_cfg->sr_res_list[0].period = params.period;
  ul_cfg.init_ul_bwp.pucch_cfg->sr_res_list[0].offset = params.offset;

  // Change the number of PRBs for PUCCH format 2 if the test bench parameter is set.
  if (pucch_f2_more_prbs) {
    const unsigned pucch_f2_nof_prbs = 3U;
    for (auto& pucch_res : ul_cfg.init_ul_bwp.pucch_cfg.value().pucch_res_list) {
      if (pucch_res.format == pucch_format::FORMAT_2 and
          variant_holds_alternative<pucch_format_2_3_cfg>(pucch_res.format_params)) {
        variant_get<pucch_format_2_3_cfg>(pucch_res.format_params).nof_prbs = pucch_f2_nof_prbs;
      }
    }
  }

  if (params.cfg_for_mimo_4x4) {
    ue_req.cfg.cells->back().serv_cell_cfg.csi_meas_cfg =
        csi_helper::make_csi_meas_config(csi_helper::csi_builder_params{.nof_ports = 4});
    auto& beta_offsets = variant_get<uci_on_pusch::beta_offsets_semi_static>(ue_req.cfg.cells->back()
                                                                                 .serv_cell_cfg.ul_config.value()
                                                                                 .init_ul_bwp.pusch_cfg.value()
                                                                                 .uci_cfg.value()
                                                                                 .beta_offsets_cfg.value());
    beta_offsets.beta_offset_csi_p2_idx_1.value() = 6;
  }

  auto& csi_report = variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
      ue_req.cfg.cells->back().serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type);
  csi_report.report_slot_period = params.csi_period;
  csi_report.report_slot_offset = params.csi_offset;

  ue_ded_cfgs.push_back(std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_list, ue_req.cfg));
  ues.add_ue(
      std::make_unique<ue>(ue_creation_command{*ue_ded_cfgs.back(), ue_req.starts_in_fallback, harq_timeout_handler}));
  last_allocated_rnti   = ue_req.crnti;
  last_allocated_ue_idx = main_ue_idx;
  slot_indication(sl_tx);
}

const ue& test_bench::get_main_ue() const
{
  return get_ue(main_ue_idx);
}

const ue& test_bench::get_ue(du_ue_index_t ue_idx) const
{
  srsran_assert(ues.contains(ue_idx), "User not found");
  return ues[ue_idx];
}

void test_bench::add_ue()
{
  cell_config_builder_params cfg_params{};
  cfg_params.csi_rs_enabled                = true;
  sched_ue_creation_request_message ue_req = test_helpers::create_default_sched_ue_creation_request(cfg_params);
  last_allocated_ue_idx =
      to_du_ue_index(static_cast<std::underlying_type<du_ue_index_t>::type>(last_allocated_ue_idx) + 1);
  ue_req.ue_index = last_allocated_ue_idx;

  ue_req.cfg.cells->begin()->serv_cell_cfg.ul_config.reset();
  ue_req.cfg.cells->begin()->serv_cell_cfg.ul_config.emplace(test_helpers::make_test_ue_uplink_config(cfg_params));

  ue_req.crnti = to_rnti(static_cast<std::underlying_type<rnti_t>::type>(last_allocated_rnti) + 1);

  ue_ded_cfgs.push_back(std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_list, ue_req.cfg));
  ues.add_ue(
      std::make_unique<ue>(ue_creation_command{*ue_ded_cfgs.back(), ue_req.starts_in_fallback, harq_timeout_handler}));
  last_allocated_rnti = ue_req.crnti;
}

void test_bench::slot_indication(slot_point slot_tx)
{
  pucch_alloc.slot_indication(slot_tx);
  mac_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
  test_logger.set_context(slot_tx.sfn(), slot_tx.slot_index());
  res_grid.slot_indication(slot_tx);
}

void test_bench::fill_all_grid(slot_point slot_tx)
{
  cell_slot_resource_allocator& pucch_slot_alloc = res_grid[slot_tx];
  pucch_slot_alloc.ul_res_grid.fill(grant_info{cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs,
                                               ofdm_symbol_range{0, 14},
                                               cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs});
}
