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

#pragma once

#include "lib/scheduler/config/du_cell_group_config_pool.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_scheduler_impl.h"
#include "tests/test_doubles/scheduler/pucch_res_test_builder_helper.h"
#include "tests/test_doubles/scheduler/scheduler_config_helper.h"
#include "tests/unittests/scheduler/test_utils/dummy_test_components.h"
#include "tests/unittests/scheduler/test_utils/scheduler_test_suite.h"
#include "srsran/ran/pucch/pucch_info.h"
#include "srsran/scheduler/config/serving_cell_config_factory.h"
#include <gtest/gtest.h>

namespace srsran {

namespace test_helpers {

/// Guardbands at the edges of the spectrum for dedicated PUCCH resources, to avoid collisions with common resources.
constexpr unsigned common_pucch_res_guardband = 4;

/// Creates a test uplink configuration using Formats 1 and 2, given the extended parameters.
inline uplink_config make_test_ue_uplink_config(const config_helpers::cell_config_builder_params_extended& params)
{
  // > UL Config.
  uplink_config ul_config{};
  ul_config.init_ul_bwp.pucch_cfg.emplace();

  // >> PUCCH.
  auto& pucch_cfg = ul_config.init_ul_bwp.pucch_cfg.value();
  // PUCCH Resource Set ID 0.
  auto& pucch_res_set_0            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_0.pucch_res_set_id = pucch_res_set_idx::set_0;
  pucch_res_set_0.pucch_res_id_list.emplace_back(pucch_res_id_t{0, 0});
  pucch_res_set_0.pucch_res_id_list.emplace_back(pucch_res_id_t{1, 1});
  pucch_res_set_0.pucch_res_id_list.emplace_back(pucch_res_id_t{2, 2});

  auto& pucch_res_set_1            = pucch_cfg.pucch_res_set.emplace_back();
  pucch_res_set_1.pucch_res_set_id = pucch_res_set_idx::set_1;
  pucch_res_set_1.pucch_res_id_list.emplace_back(pucch_res_id_t{3, 3});
  pucch_res_set_1.pucch_res_id_list.emplace_back(pucch_res_id_t{4, 4});
  pucch_res_set_1.pucch_res_id_list.emplace_back(pucch_res_id_t{5, 5});
  pucch_res_set_1.pucch_res_id_list.emplace_back(pucch_res_id_t{6, 6});
  pucch_res_set_1.pucch_res_id_list.emplace_back(pucch_res_id_t{7, 7});
  pucch_res_set_1.pucch_res_id_list.emplace_back(pucch_res_id_t{8, 8});

  unsigned nof_rbs = params.cell_nof_crbs;

  // PUCCH resource format 1, for HARQ-ACK.
  // >>> PUCCH resource 0.
  pucch_resource res_basic{.res_id           = pucch_res_id_t{0, 0},
                           .starting_prb     = nof_rbs - common_pucch_res_guardband - 1,
                           .nof_symbols      = 14,
                           .starting_sym_idx = 0,
                           .format           = pucch_format::FORMAT_1};

  res_basic.format_params.emplace<pucch_format_1_cfg>(
      pucch_format_1_cfg{.initial_cyclic_shift = 0, .time_domain_occ = 0});
  pucch_cfg.pucch_res_list.push_back(res_basic);
  // >>> PUCCH resource 1.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res1 = pucch_cfg.pucch_res_list.back();
  res1.res_id          = pucch_res_id_t{1, 1};
  res1.starting_prb    = common_pucch_res_guardband + 1;
  res1.second_hop_prb.emplace(nof_rbs - res1.starting_prb - 1);
  // >>> PUCCH resource 2.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res2 = pucch_cfg.pucch_res_list.back();
  res2.res_id          = pucch_res_id_t{2, 2};
  res2.second_hop_prb.emplace(common_pucch_res_guardband + 1);
  res2.starting_prb = nof_rbs - res2.second_hop_prb.value() - common_pucch_res_guardband - 1;

  // PUCCH resource format 2, for HARQ-ACK and CSI.
  // >>> PUCCH resource 3.
  pucch_resource res_basic_f2{.starting_prb     = common_pucch_res_guardband + 2,
                              .nof_symbols      = 2,
                              .starting_sym_idx = 0,
                              .format           = pucch_format::FORMAT_2};
  res_basic_f2.res_id = pucch_res_id_t{3, 3};
  res_basic_f2.format_params.emplace<pucch_format_2_3_cfg>(pucch_format_2_3_cfg{.nof_prbs = 1});
  pucch_cfg.pucch_res_list.push_back(res_basic_f2);
  // >>> PUCCH resource 4.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic_f2);
    pucch_resource& res4  = pucch_cfg.pucch_res_list.back();
    res4.res_id           = pucch_res_id_t{4, 4};
    res4.starting_sym_idx = 2;
  }
  // >>> PUCCH resource 5.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic_f2);
    pucch_resource& res5  = pucch_cfg.pucch_res_list.back();
    res5.res_id           = pucch_res_id_t{5, 5};
    res5.starting_sym_idx = 4;
  }
  // >>> PUCCH resource 6.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic_f2);
    pucch_resource& res6  = pucch_cfg.pucch_res_list.back();
    res6.res_id           = pucch_res_id_t{6, 6};
    res6.starting_sym_idx = 6;
  }
  // >>> PUCCH resource 7.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic_f2);
    pucch_resource& res7  = pucch_cfg.pucch_res_list.back();
    res7.res_id           = pucch_res_id_t{7, 7};
    res7.starting_sym_idx = 8;
  }
  // >>> PUCCH resource 8.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic_f2);
    pucch_resource& res8  = pucch_cfg.pucch_res_list.back();
    res8.res_id           = pucch_res_id_t{8, 8};
    res8.starting_sym_idx = 10;
  }
  // >>> PUCCH resource 9.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic_f2);
    pucch_resource& res9  = pucch_cfg.pucch_res_list.back();
    res9.res_id           = pucch_res_id_t{9, 9};
    res9.starting_sym_idx = 12;
  }

  // >>> PUCCH resource 10.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic);
    pucch_resource& res10 = pucch_cfg.pucch_res_list.back();
    res10.res_id          = pucch_res_id_t{10, 10};
    res10.starting_prb    = common_pucch_res_guardband;
  }

  // >>> PUCCH resource 11.
  {
    pucch_cfg.pucch_res_list.push_back(res_basic);
    pucch_resource& res11 = pucch_cfg.pucch_res_list.back();
    res11.res_id          = pucch_res_id_t{11, 11};
    res11.starting_prb    = nof_rbs - common_pucch_res_guardband - 3;
  }

  // TODO: add more PUCCH resources.

  // >>> SR Resource.
  const unsigned pucch_sr_res_id = pucch_cfg.pucch_res_list.size() - 1;
  pucch_cfg.sr_res_list.push_back(
      scheduling_request_resource_config{.sr_res_id    = 1,
                                         .sr_id        = uint_to_sched_req_id(0),
                                         .period       = sr_periodicity::sl_40,
                                         .offset       = 0,
                                         .pucch_res_id = pucch_res_id_t{pucch_sr_res_id, pucch_sr_res_id}});

  pucch_cfg.format_1_common_param.emplace();
  pucch_cfg.format_2_common_param.emplace(
      pucch_common_all_formats{.max_c_rate = max_pucch_code_rate::dot_25, .simultaneous_harq_ack_csi = true});
  pucch_cfg.format_3_common_param.emplace(
      pucch_common_all_formats{.max_c_rate = max_pucch_code_rate::dot_25, .simultaneous_harq_ack_csi = true});
  pucch_cfg.format_4_common_param.emplace(
      pucch_common_all_formats{.max_c_rate = max_pucch_code_rate::dot_25, .simultaneous_harq_ack_csi = true});

  // >>> dl-DataToUl-Ack
  // TS38.213, 9.1.2.1 - "If a UE is provided dl-DataToUL-ACK, the UE does not expect to be indicated by DCI format 1_0
  // a slot timing value for transmission of HARQ-ACK information that does not belong to the intersection of the set
  // of slot timing values {1, 2, 3, 4, 5, 6, 7, 8} and the set of slot timing values provided by dl-DataToUL-ACK for
  // the active DL BWP of a corresponding serving cell.
  // Inactive for format1_0."
  // Note2: Only k1 >= 4 supported.
  nr_band band =
      params.band.has_value() ? params.band.value() : band_helper::get_band_from_dl_arfcn(params.dl_f_ref_arfcn);
  if (band_helper::get_duplex_mode(band) == duplex_mode::FDD) {
    pucch_cfg.dl_data_to_ul_ack = {params.min_k1};
  } else {
    // TDD
    pucch_cfg.dl_data_to_ul_ack = config_helpers::generate_k1_candidates(params.tdd_ul_dl_cfg_common.value());
  }

  // > PUSCH config.
  ul_config.init_ul_bwp.pusch_cfg.emplace(make_default_pusch_config(params));
  if (band_helper::get_duplex_mode(band) == duplex_mode::TDD) {
    ul_config.init_ul_bwp.pusch_cfg->pusch_td_alloc_list =
        config_helpers::generate_k2_candidates(cyclic_prefix::NORMAL, params.tdd_ul_dl_cfg_common.value());
  }

  // Compute the max UCI payload per format.
  pucch_cfg.format_max_payload[pucch_format_to_uint(pucch_format::FORMAT_1)] = 2U;
  const auto& res_f2 = std::get<pucch_format_2_3_cfg>(res_basic_f2.format_params);
  pucch_cfg.format_max_payload[pucch_format_to_uint(pucch_format::FORMAT_2)] =
      get_pucch_format2_max_payload(res_f2.nof_prbs,
                                    res_basic_f2.nof_symbols,
                                    to_max_code_rate_float(pucch_cfg.format_2_common_param.value().max_c_rate));
  pucch_cfg.set_1_format = pucch_format::FORMAT_2;

  // > SRS config.
  ul_config.init_ul_bwp.srs_cfg.emplace(make_default_srs_config(params));

  return ul_config;
}

} // namespace test_helpers

// Helper function to create a sched_cell_configuration_request_message that allows a configuration with either 15kHz or
// 30kHz SCS. By default, it creates a bandwidth of 20MHz.
inline sched_cell_configuration_request_message
make_default_sched_cell_configuration_request_scs(subcarrier_spacing scs, bool tdd_mode = false)
{
  cell_config_builder_params params{
      .scs_common = scs, .channel_bw_mhz = bs_channel_bandwidth::MHz20, .nof_dl_ports = 1};
  if (scs == subcarrier_spacing::kHz15) {
    // Band n5 for FDD, band n41 for TDD.
    params.dl_f_ref_arfcn = tdd_mode ? 499200 : 530000;
    params.band           = band_helper::get_band_from_dl_arfcn(params.dl_f_ref_arfcn);
  } else if (scs == subcarrier_spacing::kHz30) {
    // Band n5 for FDD, band n77 or n78 for TDD.
    params.dl_f_ref_arfcn = tdd_mode ? 630000 : 176000;
    params.band           = band_helper::get_band_from_dl_arfcn(params.dl_f_ref_arfcn);
  }
  return sched_cell_configuration_request_message{
      sched_config_helper::make_default_sched_cell_configuration_request(params)};
}

////////////    Builder of PUCCH scheduler output     ////////////

// Builds the PUCCH scheduler output for test.
pucch_info build_pucch_info(const bwp_configuration* bwp_cfg,
                            unsigned                 pci,
                            pucch_format             format,
                            prb_interval             prbs,
                            prb_interval             second_hop_prbs,
                            ofdm_symbol_range        symbols,
                            uint8_t                  initial_cyclic_shift,
                            sr_nof_bits              sr_bits,
                            unsigned                 harq_ack_nof_bits,
                            uint8_t                  time_domain_occ);

bool pucch_info_match(const pucch_info& expected, const pucch_info& test);

// Wrapper for std::find_if() to find a PUCCH PDU in a vector of PUCCH PDUs.
template <typename F>
bool find_pucch_pdu(const static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucch_pdus, const F& func)
{
  return std::find_if(pucch_pdus.begin(), pucch_pdus.end(), func) != pucch_pdus.end();
}

// Makes a default DCI for PUCCH test purposes but some given parameters.
inline pdcch_dl_information make_default_dci(unsigned n_cces, const coreset_configuration* coreset_cfg_)
{
  pdcch_dl_information dci{};
  dci.ctx.cces.ncce   = n_cces;
  dci.ctx.coreset_cfg = coreset_cfg_;
  return dci;
}

inline sched_cell_configuration_request_message
make_custom_sched_cell_configuration_request(unsigned pucch_res_common, bool is_tdd = false, unsigned nof_dl_ports = 1)
{
  subcarrier_spacing                     scs = is_tdd ? subcarrier_spacing::kHz30 : subcarrier_spacing::kHz15;
  std::optional<tdd_ul_dl_config_common> tdd_cfg;
  if (is_tdd) {
    tdd_cfg = {scs, {10, 6, 8, 3, 0}, std::nullopt};
  }
  sched_cell_configuration_request_message req = sched_config_helper::make_default_sched_cell_configuration_request(
      cell_config_builder_params{.scs_common           = is_tdd ? subcarrier_spacing::kHz30 : subcarrier_spacing::kHz15,
                                 .channel_bw_mhz       = bs_channel_bandwidth::MHz10,
                                 .dl_f_ref_arfcn       = is_tdd ? 520000U : 365000U,
                                 .nof_dl_ports         = nof_dl_ports,
                                 .tdd_ul_dl_cfg_common = tdd_cfg});
  req.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common = pucch_res_common;
  return req;
}

/////////        TEST BENCH for PUCCH scheduler        /////////

struct test_bench_params {
  unsigned               pucch_res_common      = 11;
  unsigned               n_cces                = 0;
  sr_periodicity         period                = sr_periodicity::sl_40;
  unsigned               offset                = 0;
  csi_report_periodicity csi_period            = csi_report_periodicity::slots320;
  unsigned               csi_offset            = 9;
  bool                   is_tdd                = false;
  bool                   pucch_f2_f3_more_prbs = false;
  bool                   cfg_for_mimo_4x4      = false;
  bool                   use_format_0          = false;
  pucch_format           set1_format           = pucch_format::FORMAT_2;
  bool                   no_csi                = false;
  max_pucch_code_rate    max_c_rate            = max_pucch_code_rate::dot_25;
};

// Test bench with all that is needed for the PUCCH.
class test_bench
{
public:
  explicit test_bench(const test_bench_params& params                  = {},
                      unsigned                 max_pucchs_per_slot_    = 32U,
                      unsigned                 max_ul_grants_per_slot_ = 32U);

  // Return the main UE, which has RNTI 0x4601.
  const ue& get_main_ue() const;
  const ue& get_ue(du_ue_index_t ue_idx) const;

  // Add an extra UE, whose RNTI will have RNTI +1 with respect to the last_allocated_rnti.
  void add_ue();

  void slot_indication(slot_point slot_tx);

  void fill_all_grid(slot_point slot_tx);

  scheduler_expert_config                        expert_cfg;
  sched_cfg_dummy_notifier                       mac_notif;
  du_cell_group_config_pool                      cfg_pool;
  cell_common_configuration_list                 cell_cfg_list{};
  const cell_configuration&                      cell_cfg;
  std::vector<std::unique_ptr<ue_configuration>> ue_ded_cfgs;

  cell_resource_allocator res_grid{cell_cfg};
  cell_harq_manager       cell_harqs;
  pdcch_dl_information    dci_info;
  const unsigned          k0;
  const unsigned          k1{4};
  const unsigned          max_pucchs_per_slot;
  const unsigned          max_ul_grants_per_slot;
  du_ue_index_t           main_ue_idx{MIN_DU_UE_INDEX};
  ue_repository           ues;
  bool                    pucch_f2_f3_more_prbs;
  const bool              use_format_0;
  const pucch_format      set1_format;

  // last_allocated_rnti keeps track of the last RNTI allocated.
  rnti_t                        last_allocated_rnti;
  du_ue_index_t                 last_allocated_ue_idx;
  pucch_allocator_impl          pucch_alloc;
  uci_allocator_impl            uci_alloc;
  uci_scheduler_impl            uci_sched;
  slot_point                    sl_tx;
  pucch_res_builder_test_helper pucch_builder;
  srslog::basic_logger&         mac_logger  = srslog::fetch_basic_logger("SCHED", true);
  srslog::basic_logger&         test_logger = srslog::fetch_basic_logger("TEST");

  // Scheduler creation request message used to create the main UE.
  sched_ue_creation_request_message ue_req_main;
};

} // namespace srsran
