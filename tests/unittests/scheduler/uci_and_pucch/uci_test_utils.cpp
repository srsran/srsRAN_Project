/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/scheduler/config/csi_helper.h"
#include "srsran/scheduler/config/scheduler_expert_config_factory.h"

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

    auto& format_0                        = pucch_test.format_params.emplace<pucch_format_0>();
    format_0.initial_cyclic_shift         = initial_cyclic_shift;
    pucch_test.uci_bits.sr_bits           = sr_bits;
    pucch_test.uci_bits.harq_ack_nof_bits = harq_ack_nof_bits;

    format_0.group_hopping = pucch_group_hopping::NEITHER;
    format_0.n_id_hopping  = pci;
  } else if (format == pucch_format::FORMAT_1) {
    pucch_test.resources.prbs            = prbs;
    pucch_test.resources.second_hop_prbs = second_hop_prbs;
    pucch_test.resources.symbols         = symbols;

    auto& format_1                        = pucch_test.format_params.emplace<pucch_format_1>();
    format_1.initial_cyclic_shift         = initial_cyclic_shift;
    pucch_test.uci_bits.sr_bits           = sr_bits;
    pucch_test.uci_bits.harq_ack_nof_bits = harq_ack_nof_bits;
    format_1.time_domain_occ              = time_domain_occ;

    format_1.group_hopping   = pucch_group_hopping::NEITHER;
    format_1.n_id_hopping    = pci;
    format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
  } else {
    return pucch_info{};
  }

  return pucch_test;
}

bool srsran::pucch_info_match(const pucch_info& expected, const pucch_info& test)
{
  bool is_equal =
      expected.crnti == test.crnti && *expected.bwp_cfg == *test.bwp_cfg && expected.format() == test.format();
  is_equal = is_equal && expected.resources.prbs == test.resources.prbs &&
             expected.resources.symbols == test.resources.symbols &&
             expected.resources.second_hop_prbs == test.resources.second_hop_prbs;

  switch (expected.format()) {
    case pucch_format::FORMAT_0: {
      const auto& expected_f = std::get<pucch_format_0>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_0>(test.format_params);
      is_equal               = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits;
    } break;
    case pucch_format::FORMAT_1: {
      const auto& expected_f = std::get<pucch_format_1>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_1>(test.format_params);
      is_equal               = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping &&
                 expected_f.initial_cyclic_shift == test_f.initial_cyclic_shift &&
                 expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
                 expected_f.slot_repetition == test_f.slot_repetition &&
                 expected_f.time_domain_occ == test_f.time_domain_occ;
    } break;
    case pucch_format::FORMAT_2: {
      const auto& expected_f = std::get<pucch_format_2>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_2>(test.format_params);
      is_equal               = is_equal && expected_f.max_code_rate == test_f.max_code_rate &&
                 expected_f.n_id_scambling == test_f.n_id_scambling &&
                 expected_f.n_id_0_scrambling == test_f.n_id_0_scrambling &&
                 expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
                 expected.uci_bits.csi_part1_nof_bits == test.uci_bits.csi_part1_nof_bits;
    } break;
    case pucch_format::FORMAT_3: {
      const auto& expected_f = std::get<pucch_format_3>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_3>(test.format_params);
      is_equal               = is_equal && expected_f.group_hopping == test_f.group_hopping &&
                 expected_f.n_id_hopping == test_f.n_id_hopping && expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
                 expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
                 expected.uci_bits.csi_part1_nof_bits == test.uci_bits.csi_part1_nof_bits &&
                 expected_f.slot_repetition == test_f.slot_repetition &&
                 expected_f.n_id_scrambling == test_f.n_id_scrambling && expected_f.pi_2_bpsk == test_f.pi_2_bpsk &&
                 expected_f.max_code_rate == test_f.max_code_rate &&
                 expected_f.additional_dmrs == test_f.additional_dmrs &&
                 expected_f.n_id_0_scrambling == test_f.n_id_0_scrambling;
    } break;
    case pucch_format::FORMAT_4: {
      const auto& expected_f = std::get<pucch_format_4>(expected.format_params);
      const auto& test_f     = std::get<pucch_format_4>(test.format_params);
      is_equal =
          is_equal && expected_f.group_hopping == test_f.group_hopping &&
          expected_f.n_id_hopping == test_f.n_id_hopping && expected.uci_bits.sr_bits == test.uci_bits.sr_bits &&
          expected.uci_bits.harq_ack_nof_bits == test.uci_bits.harq_ack_nof_bits &&
          expected.uci_bits.csi_part1_nof_bits == test.uci_bits.csi_part1_nof_bits &&
          expected_f.slot_repetition == test_f.slot_repetition &&
          expected_f.n_id_scrambling == test_f.n_id_scrambling && expected_f.pi_2_bpsk == test_f.pi_2_bpsk &&
          expected_f.max_code_rate == test_f.max_code_rate && expected_f.orthog_seq_idx == test_f.orthog_seq_idx &&
          expected_f.n_sf_pucch_f4 == test_f.n_sf_pucch_f4 && expected_f.additional_dmrs == test_f.additional_dmrs &&
          expected_f.n_id_0_scrambling == test_f.n_id_0_scrambling;
    } break;
    default: {
      return false;
    };
  }

  return is_equal;
}

namespace {

class dummy_harq_timeout_notifier : public harq_timeout_notifier
{
public:
  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override {}
};

} // namespace

/////////        TEST BENCH for PUCCH scheduler        /////////

// Test bench with all that is needed for the PUCCH.

test_bench::test_bench(const test_bench_params& params,
                       unsigned                 max_pucchs_per_slot_,
                       unsigned                 max_ul_grants_per_slot_) :
  expert_cfg{[]() -> scheduler_expert_config {
    auto default_exp_cfg                            = config_helpers::make_default_scheduler_expert_config();
    default_exp_cfg.ue.min_pucch_pusch_prb_distance = 0U;
    return default_exp_cfg;
  }()},
  cell_cfg{[this, &params]() -> const cell_configuration& {
    auto cell_req = make_custom_sched_cell_configuration_request(
        params.pucch_res_common, params.is_tdd, params.cfg_for_mimo_4x4 ? 4 : 1);
    cfg_pool.add_cell(cell_req);
    cell_cfg_list.emplace(to_du_cell_index(0), std::make_unique<cell_configuration>(expert_cfg, cell_req));
    return *cell_cfg_list[to_du_cell_index(0)];
  }()},
  cell_harqs{MAX_NOF_DU_UES, MAX_NOF_HARQS, std::make_unique<dummy_harq_timeout_notifier>()},
  dci_info{make_default_dci(params.n_cces, &cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value())},
  k0(cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[0].k0),
  max_pucchs_per_slot{max_pucchs_per_slot_},
  max_ul_grants_per_slot{max_ul_grants_per_slot_},
  pucch_f2_f3_more_prbs{params.pucch_f2_f3_more_prbs},
  use_format_0(params.use_format_0),
  set1_format(params.set1_format),
  pucch_alloc{cell_cfg, max_pucchs_per_slot, max_ul_grants_per_slot},
  uci_alloc(pucch_alloc),
  uci_sched{cell_cfg, uci_alloc, ues},
  sl_tx{to_numerology_value(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs), 0}
{
  srsran_assert(not(params.cfg_for_mimo_4x4 and params.no_csi),
                "The configuration with no CSI is not supported for 4x4 MIMO.");

  cell_config_builder_params cfg_params{};
  cfg_params.csi_rs_enabled                = true;
  cfg_params.scs_common                    = params.is_tdd ? subcarrier_spacing::kHz30 : subcarrier_spacing::kHz15;
  cfg_params.dl_f_ref_arfcn                = params.is_tdd ? 520000U : 365000U;
  sched_ue_creation_request_message ue_req = sched_config_helper::create_default_sched_ue_creation_request(cfg_params);
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

  if (params.no_csi) {
    ue_req.cfg.cells->back().serv_cell_cfg.csi_meas_cfg.reset();
  } else {
    auto& csi_report = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
        ue_req.cfg.cells.value().back().serv_cell_cfg.csi_meas_cfg.value().csi_report_cfg_list[0].report_cfg_type);
    csi_report.report_slot_period = params.csi_period;
    csi_report.report_slot_offset = params.csi_offset;
  }

  if (set1_format != pucch_format::FORMAT_2 || use_format_0) {
    pucch_builder_params pucch_params{};
    if (use_format_0) {
      pucch_params.f0_or_f1_params.emplace<pucch_f0_params>();
      pucch_params.nof_ue_pucch_f0_or_f1_res_harq       = 6;
      pucch_params.nof_ue_pucch_f2_or_f3_or_f4_res_harq = 6;
    }
    switch (set1_format) {
      case pucch_format::FORMAT_2:
        break;
      case pucch_format::FORMAT_3:
        pucch_params.f2_or_f3_or_f4_params.emplace<pucch_f3_params>();
        break;
      case pucch_format::FORMAT_4:
        pucch_params.f2_or_f3_or_f4_params.emplace<pucch_f4_params>();
        break;
      default:
        srsran_assertion_failure("Invalid PUCCH Format for Set Id 1 (valid values are 2, 3 or 4)");
    }
    pucch_builder.setup(
        cell_cfg.ul_cfg_common.init_ul_bwp, params.is_tdd ? cell_cfg.tdd_cfg_common : std::nullopt, pucch_params);
    // This function is called so that the PUCCH resource list is generated again with the new parameters.
    bool new_ue_added = pucch_builder.add_build_new_ue_pucch_cfg(ue_req.cfg.cells.value().back().serv_cell_cfg);
    if (not new_ue_added) {
      srsran_terminate("UE PUCCH configuration couldn't be built");
    }
  }

  // Change the number of PRBs for PUCCH format 2/3 if the test bench parameter is set.
  if (pucch_f2_f3_more_prbs) {
    static constexpr unsigned pucch_f2_f3_nof_prbs = 3U;
    for (auto& pucch_res : ul_cfg.init_ul_bwp.pucch_cfg.value().pucch_res_list) {
      if (pucch_res.format == set1_format and std::holds_alternative<pucch_format_2_3_cfg>(pucch_res.format_params)) {
        std::get<pucch_format_2_3_cfg>(pucch_res.format_params).nof_prbs = pucch_f2_f3_nof_prbs;
      }
    }
  }

  // TODO: extend for PUCCH Formats 3/4
  if (params.cfg_for_mimo_4x4) {
    auto& pucch_cfg = ue_req.cfg.cells->back().serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value();
    pucch_cfg.format_2_common_param.value().max_c_rate = max_pucch_code_rate::dot_35;
    if (params.pucch_f2_f3_more_prbs) {
      for (auto& res_it : pucch_cfg.pucch_res_list) {
        if (res_it.format == set1_format and std::holds_alternative<pucch_format_2_3_cfg>(res_it.format_params)) {
          std::get<pucch_format_2_3_cfg>(res_it.format_params).nof_prbs = 2U;
        }
      }
    }
    const auto& res_f2 = std::find_if(pucch_cfg.pucch_res_list.begin(),
                                      pucch_cfg.pucch_res_list.end(),
                                      [](const auto& pucch) { return pucch.format == pucch_format::FORMAT_2; });
    srsran_assert(res_f2 != pucch_cfg.pucch_res_list.end(), "PUCCH format 2 not found");
    const auto& res_f2_cfg = std::get<pucch_format_2_3_cfg>(res_f2->format_params);
    pucch_cfg.format_max_payload[pucch_format_to_uint(pucch_format::FORMAT_2)] =
        get_pucch_format2_max_payload(res_f2_cfg.nof_prbs,
                                      res_f2->nof_symbols,
                                      to_max_code_rate_float(pucch_cfg.format_2_common_param.value().max_c_rate));
    pucch_cfg.set_1_format = pucch_format::FORMAT_2;
    ue_req.cfg.cells->back().serv_cell_cfg.csi_meas_cfg =
        csi_helper::make_csi_meas_config(csi_helper::csi_builder_params{.nof_ports = 4, .max_nof_layers = 4});
    auto& beta_offsets = std::get<uci_on_pusch::beta_offsets_semi_static>(ue_req.cfg.cells->back()
                                                                              .serv_cell_cfg.ul_config.value()
                                                                              .init_ul_bwp.pusch_cfg.value()
                                                                              .uci_cfg.value()
                                                                              .beta_offsets_cfg.value());
    beta_offsets.beta_offset_csi_p2_idx_1.value() = 6;
  }

  if (params.max_c_rate != max_pucch_code_rate::dot_25) {
    auto& pucch_cfg = ue_req.cfg.cells->back().serv_cell_cfg.ul_config->init_ul_bwp.pucch_cfg.value();
    pucch_cfg.format_2_common_param.value().max_c_rate = params.max_c_rate;
    const auto& res_f2                                 = std::find_if(pucch_cfg.pucch_res_list.begin(),
                                      pucch_cfg.pucch_res_list.end(),
                                      [](const auto& pucch) { return pucch.format == pucch_format::FORMAT_2; });
    srsran_assert(res_f2 != pucch_cfg.pucch_res_list.end(), "PUCCH format 2 not found");
    const auto& res_f2_cfg = std::get<pucch_format_2_3_cfg>(res_f2->format_params);
    pucch_cfg.format_max_payload[pucch_format_to_uint(pucch_format::FORMAT_2)] =
        get_pucch_format2_max_payload(res_f2_cfg.nof_prbs,
                                      res_f2->nof_symbols,
                                      to_max_code_rate_float(pucch_cfg.format_2_common_param.value().max_c_rate));
  }

  ue_req_main = ue_req;
  ue_ded_cfgs.push_back(
      std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_list, cfg_pool.add_ue(ue_req)));
  ues.add_ue(std::make_unique<ue>(ue_creation_command{*ue_ded_cfgs.back(), ue_req.starts_in_fallback, cell_harqs, {}}));
  uci_sched.add_ue(ues[ue_req.ue_index].get_pcell().cfg());
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
  sched_ue_creation_request_message ue_req = ue_req_main;
  last_allocated_ue_idx = to_du_ue_index(static_cast<std::underlying_type_t<du_ue_index_t>>(last_allocated_ue_idx) + 1);
  ue_req.ue_index       = last_allocated_ue_idx;

  ue_req.crnti = to_rnti(static_cast<std::underlying_type_t<rnti_t>>(last_allocated_rnti) + 1);

  srsran_assert(not use_format_0 or
                    pucch_builder.add_build_new_ue_pucch_cfg(ue_req.cfg.cells.value().back().serv_cell_cfg),
                "UE PUCCH configuration couldn't be built");

  ue_ded_cfgs.push_back(
      std::make_unique<ue_configuration>(ue_req.ue_index, ue_req.crnti, cell_cfg_list, cfg_pool.add_ue(ue_req)));
  ues.add_ue(std::make_unique<ue>(ue_creation_command{*ue_ded_cfgs.back(), ue_req.starts_in_fallback, cell_harqs}));
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
