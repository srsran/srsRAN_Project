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

#include "lib/du/du_high/du_manager/ran_resource_management/du_srs_resource_manager.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

namespace {

struct srs_params {
  // If set, it's a TDD cell and the parameters indicates the number of UL symbols in the flexible slot.
  std::optional<unsigned> nof_ul_symbols_p1;
  bool                    test_optimality = false;
};

std::ostream& operator<<(std::ostream& out, const srs_params& params)
{
  if (params.nof_ul_symbols_p1.has_value()) {
    out << fmt::format("TDD_nof_ul_symbols_p1_{}_test_optimality_{}",
                       params.nof_ul_symbols_p1.value(),
                       params.test_optimality ? "true" : "false");
  } else {
    out << fmt::format("FDD_test_optimality_{}", params.test_optimality ? "true" : "false");
  }
  return out;
}

} // namespace

template <>
struct fmt::formatter<srs_params> : ostream_formatter {};

static bool is_ul_slot(unsigned offset, const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
  return has_active_tdd_ul_symbols(tdd_cfg, slot_index);
}

static bool is_partially_ul_slot(unsigned offset, const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
  return is_tdd_partial_ul_slot(tdd_cfg, slot_index);
}

static cell_config_builder_params make_cell_cfg_params(const srs_params& params)
{
  const bool                 is_tdd      = params.nof_ul_symbols_p1.has_value();
  cell_config_builder_params cell_params = {.dl_f_ref_arfcn = not is_tdd ? 365000U : 520002U};
  if (is_tdd) {
    auto& tdd_cfg                              = cell_params.tdd_ul_dl_cfg_common.emplace();
    tdd_cfg.pattern1.dl_ul_tx_period_nof_slots = 10;
    tdd_cfg.pattern1.nof_dl_slots              = 7;
    tdd_cfg.pattern1.nof_dl_symbols            = 2;
    tdd_cfg.pattern1.nof_ul_slots              = 2;
    tdd_cfg.pattern1.nof_ul_symbols            = params.nof_ul_symbols_p1.value();
    tdd_cfg.ref_scs                            = subcarrier_spacing::kHz30;
  }

  return cell_params;
}

static du_cell_config make_srs_base_du_cell_config(const cell_config_builder_params& params)
{
  // This function generates a configuration which would potentially allow for a very large number of SRS resources.
  du_cell_config du_cfg  = config_helpers::make_default_du_cell_config(params);
  auto&          srs_cfg = du_cfg.srs_cfg;

  // Generates a random SRS configuration.
  srs_cfg.tx_comb                                = test_rgen::bernoulli(0.5) ? tx_comb_size::n2 : tx_comb_size::n4;
  srs_cfg.max_nof_symbols                        = test_rgen::uniform_int<unsigned>(1U, 6U);
  std::array<srs_nof_symbols, 3> nof_symb_values = {srs_nof_symbols::n1, srs_nof_symbols::n2, srs_nof_symbols::n4};
  srs_cfg.nof_symbols = nof_symb_values[test_rgen::uniform_int<unsigned>(0, nof_symb_values.size() - 1)];
  while (srs_cfg.nof_symbols > srs_cfg.max_nof_symbols) {
    srs_cfg.nof_symbols = nof_symb_values[test_rgen::uniform_int<unsigned>(0, nof_symb_values.size() - 1)];
  }

  // The TX comb cyclic shift value depends on the TX comb size.
  if (srs_cfg.tx_comb == srsran::tx_comb_size::n2) {
    std::array<nof_cyclic_shifts, 3> srs_cyclic_shift_values = {
        nof_cyclic_shifts::no_cyclic_shift, nof_cyclic_shifts::two, nof_cyclic_shifts::four};
    srs_cfg.cyclic_shift_reuse_factor =
        srs_cyclic_shift_values[test_rgen::uniform_int<unsigned>(0, srs_cyclic_shift_values.size() - 1)];
  } else {
    std::array<nof_cyclic_shifts, 6> srs_cyclic_shift_values = {nof_cyclic_shifts::no_cyclic_shift,
                                                                nof_cyclic_shifts::two,
                                                                nof_cyclic_shifts::three,
                                                                nof_cyclic_shifts::four,
                                                                nof_cyclic_shifts::six,
                                                                nof_cyclic_shifts::twelve};
    srs_cfg.cyclic_shift_reuse_factor =
        srs_cyclic_shift_values[test_rgen::uniform_int<unsigned>(0, srs_cyclic_shift_values.size() - 1)];
  }
  // [Implementation-defined] These are the values in the gNB, \ref sequence_id_reuse_factor.
  std::array<unsigned, 8> srs_seq_id_values = {1, 2, 3, 5, 6, 10, 15, 30};
  srs_cfg.sequence_id_reuse_factor =
      srs_seq_id_values[test_rgen::uniform_int<unsigned>(0, srs_seq_id_values.size() - 1)];

  if (du_cfg.tdd_ul_dl_cfg_common.has_value()) {
    std::array<srs_periodicity, 8> period_values = {srs_periodicity::sl10,
                                                    srs_periodicity::sl20,
                                                    srs_periodicity::sl40,
                                                    srs_periodicity::sl80,
                                                    srs_periodicity::sl160,
                                                    srs_periodicity::sl320,
                                                    srs_periodicity::sl640,
                                                    srs_periodicity::sl1280};
    srs_cfg.srs_period.emplace(period_values[test_rgen::uniform_int<unsigned>(0, period_values.size() - 1)]);
  } else {
    std::array<srs_periodicity, 10> period_values = {srs_periodicity::sl1,
                                                     srs_periodicity::sl2,
                                                     srs_periodicity::sl4,
                                                     srs_periodicity::sl5,
                                                     srs_periodicity::sl8,
                                                     srs_periodicity::sl10,
                                                     srs_periodicity::sl16,
                                                     srs_periodicity::sl20,
                                                     srs_periodicity::sl32,
                                                     srs_periodicity::sl40};
    srs_cfg.srs_period.emplace(period_values[test_rgen::uniform_int<unsigned>(0, period_values.size() - 1)]);
  }

  return du_cfg;
}

static du_cell_config make_srs_du_cell_config(const cell_config_builder_params& params, bool limit_srs_res)
{
  auto du_cfg = make_srs_base_du_cell_config(params);

  // For the optimality test, we to have a configuration that doesn't allow for more than 1024 SRS resources.
  if (not limit_srs_res) {
    return du_cfg;
  }

  // This function calculates the total number of SRS resources that can be potentially allocated in the cell with the
  // given SRS parameters.
  auto tot_num_srs_res = [&du_cfg]() {
    auto& tdd_cfg = du_cfg.tdd_ul_dl_cfg_common;
    auto& srs_cfg = du_cfg.srs_cfg;
    // This is the number of SRS resources per symbol interval. A symbol interval is an interval where the SRS resource
    // can be placed within a slot and its width (or length) is given by the corresponding SRS parameter \c nof_symb in
    // the SRS configuration.
    const unsigned nof_res_per_symb_interval = srs_cfg.sequence_id_reuse_factor *
                                               static_cast<unsigned>(srs_cfg.cyclic_shift_reuse_factor) *
                                               static_cast<unsigned>(srs_cfg.tx_comb);

    // This "number of symbols intervals" counts all the symbols intervals within the SRS period.
    unsigned nof_symb_intervals = 0;

    // The number of symbols interval per slot depends on whether it's FDD or TDD.
    if (not tdd_cfg.has_value()) {
      // In FDD, in an SRS period we can "max_nof_symbols / nof_symbols" intervals per slot.
      nof_symb_intervals = srs_cfg.max_nof_symbols.to_uint() / static_cast<unsigned>(srs_cfg.nof_symbols) *
                           static_cast<unsigned>(srs_cfg.srs_period.value());
    } else {
      // In TDD, in an SRS period we can "max_nof_symbols / nof_symbols" intervals per UL slot; in addition to this, the
      // partially-UL slots can have extra symbols intervals, up to a maximum of 6U, which is the max number of symbols
      // usable for SRS resources.
      nof_symb_intervals =
          (srs_cfg.max_nof_symbols.to_uint() / static_cast<unsigned>(srs_cfg.nof_symbols)) *
          tdd_cfg->pattern1.nof_ul_slots *
          (static_cast<unsigned>(srs_cfg.srs_period.value()) / tdd_cfg->pattern1.dl_ul_tx_period_nof_slots);
      if (tdd_cfg->pattern1.nof_ul_symbols != 0) {
        nof_symb_intervals +=
            (std::min(tdd_cfg->pattern1.nof_ul_symbols, srs_cfg.max_nof_symbols.max()) /
             static_cast<unsigned>(srs_cfg.nof_symbols)) *
            (static_cast<unsigned>(srs_cfg.srs_period.value()) / tdd_cfg->pattern1.dl_ul_tx_period_nof_slots);
      }
    }

    // Return the total number of SRS resources.
    return nof_symb_intervals * nof_res_per_symb_interval;
  };

  while (tot_num_srs_res() > static_cast<unsigned>(MAX_NOF_DU_UES)) {
    du_cfg = make_srs_base_du_cell_config(params);
  }

  return du_cfg;
}

class du_srs_resource_manager_tester : public ::testing::TestWithParam<srs_params>
{
protected:
  explicit du_srs_resource_manager_tester(cell_config_builder_params params_ = make_cell_cfg_params(GetParam())) :
    params(params_),
    cell_cfg_list({make_srs_du_cell_config(params_, GetParam().test_optimality)}),
    srs_params(cell_cfg_list[0].srs_cfg),
    du_srs_res_mng(cell_cfg_list)
  {
  }

  std::optional<cell_group_config> add_ue(du_ue_index_t ue_idx)
  {
    if (ue_idx >= MAX_NOF_DU_UES) {
      return std::nullopt;
    }

    cell_group_config                  cell_grp_cfg;
    std::unique_ptr<cell_group_config> cell_grp_cfg_ptr = std::make_unique<cell_group_config>();
    cell_grp_cfg.cells.insert(
        serv_cell_index_t::SERVING_CELL_PCELL_IDX,
        cell_config_dedicated{serv_cell_index_t::SERVING_CELL_PCELL_IDX,
                              config_helpers::create_default_initial_ue_serving_cell_config(params)});
    ues.insert(ue_idx, cell_grp_cfg);
    auto& ue = ues[ue_idx];

    // Reset the SRS config before allocating resources.
    ue.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();

    bool res = du_srs_res_mng.alloc_resources(ue);
    if (res) {
      return ue;
    }
    // If the allocation was not possible, remove the UE.
    ues.erase(ue_idx);
    return std::nullopt;
  }

  // Helper struct that keeps track of the SRS resource parameter assigned to the UEs.
  struct srs_res_params {
    unsigned          offset;
    unsigned          tx_comb_offset;
    ofdm_symbol_range symbols;
    unsigned          sequence_id;
    unsigned          cs;

    // Constructor for the srs_res_params struct given srs_resource.
    explicit srs_res_params(const srs_config::srs_resource& srs_res) :
      offset(srs_res.periodicity_and_offset->offset),
      tx_comb_offset(srs_res.tx_comb.tx_comb_offset),
      symbols(ofdm_symbol_range{NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - srs_res.res_mapping.start_pos - 1,
                                NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - srs_res.res_mapping.start_pos - 1 +
                                    srs_res.res_mapping.nof_symb}),
      sequence_id(srs_res.sequence_id),
      cs(srs_res.tx_comb.tx_comb_cyclic_shift)
    {
    }

    // Returns true is SRS resource collides with another SRS resource; it is used to check if the SRS resources are
    // orthogonal.
    bool collides(const srs_res_params& rhs) const
    {
      return offset == rhs.offset and tx_comb_offset == rhs.tx_comb_offset and symbols.overlaps(rhs.symbols) and
             sequence_id == rhs.sequence_id and cs == rhs.cs;
    }

    bool operator==(const srs_res_params& rhs) const
    {
      return offset == rhs.offset and tx_comb_offset == rhs.tx_comb_offset and symbols == rhs.symbols and
             sequence_id == rhs.sequence_id and cs == rhs.cs;
    }

    bool operator!=(const srs_res_params& rhs) const { return not operator==(rhs); }
  };

  // Helper that computes the C_SRS parameter (see Section 6.4.1.4.3, TS 38.211).
  unsigned compute_c_srs() const
  {
    // In this test, we only consider the case where B_SRS is 0.
    const uint8_t b_srs           = 0U;
    unsigned      candidate_c_srs = 0U;
    unsigned      candidate_m_srs = 0U;
    // Spans over Table 6.4.1.4.3-1 in TS 38.211 and find the smallest C_SRS that maximizes m_srs_0 under the
    // constraint of m_SRS <= nof_BW_RBs.
    for (unsigned c_srs_it = 0; c_srs_it != 64; ++c_srs_it) {
      std::optional<srs_configuration> srs_cfg = srs_configuration_get(c_srs_it, b_srs);
      srsran_assert(srs_cfg.has_value(), "C_SRS is required for this unittest");
      if (srs_cfg.value().m_srs <= cell_cfg_list[0].ul_cfg_common.init_ul_bwp.generic_params.crbs.length() and
          srs_cfg.value().m_srs > candidate_m_srs) {
        candidate_m_srs = srs_cfg->m_srs;
        candidate_c_srs = c_srs_it;
      }
    }
    return candidate_c_srs;
  }

  // Helper that computes the Frequency Shift parameter (see Section 6.4.1.4.3, TS 38.211).
  unsigned compute_freq_shift() const
  {
    // The function computes the frequency shift so that the SRS resources are placed in the center of the band.
    unsigned   c_srs         = compute_c_srs();
    unsigned   ul_bw_nof_rbs = cell_cfg_list[0].ul_cfg_common.init_ul_bwp.generic_params.crbs.length();
    const auto srs_cfg       = srs_configuration_get(c_srs, 0U);
    srsran_assert(srs_cfg.has_value(), "C_SRS is required for this unittest");
    return (ul_bw_nof_rbs - srs_cfg.value().m_srs) / 2U;
  }

  cell_config_builder_params  params;
  std::vector<du_cell_config> cell_cfg_list;
  // This is a reference to the first element of cell_cfg_list's SRS config.
  const srs_builder_params&                        srs_params;
  du_srs_policy_max_ul_rate                        du_srs_res_mng;
  slotted_array<cell_group_config, MAX_NOF_DU_UES> ues;
};

TEST_P(du_srs_resource_manager_tester, ue_are_assigned_orthogonal_srs_resources)
{
  // Keeps track of which SRS resources have been assigned to the UEs.
  std::vector<srs_res_params> used_srs_resources;
  // > Created UEs have unique SRS resources.
  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    std::optional<cell_group_config> ue = add_ue(to_du_ue_index(i));
    if (not ue.has_value()) {
      break;
    }

    // Check if the SRS has been assigned to the UE.
    const auto& srs_res_list = ue.value().cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list;
    ASSERT_FALSE(srs_res_list.empty());
    // Check if the SRS resource collides (i.e., is not orthogonal) with any other SRS resource.
    const auto&          srs_res = srs_res_list[0];
    const srs_res_params res_params(srs_res);
    ASSERT_FALSE(std::any_of(used_srs_resources.begin(),
                             used_srs_resources.end(),
                             [&res_params](const srs_res_params& res) { return res.collides(res_params); }));

    used_srs_resources.push_back(res_params);
  }

  // Erase a random UE and attempt.
  const du_ue_index_t ue_idx_to_rem    = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, ues.size() - 1));
  auto&               ue_to_be_removed = ues[ue_idx_to_rem];
  // First, find the SRS resource of the ue to be removed and removed it from the vector of used resources.
  srs_res_params srs_res_to_be_removed(
      ue_to_be_removed.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list[0]);
  auto res_to_remove_it = std::find(used_srs_resources.begin(), used_srs_resources.end(), srs_res_to_be_removed);
  ASSERT_FALSE(res_to_remove_it == used_srs_resources.end());
  used_srs_resources.erase(res_to_remove_it);
  du_srs_res_mng.dealloc_resources(ues[ue_idx_to_rem]);
  ues.erase(ue_idx_to_rem);

  // Attempt a new allocation and verify it is successful.
  std::optional<cell_group_config> ue = add_ue(ue_idx_to_rem);
  ASSERT_TRUE(ue.has_value());

  // Check if the SRS has been assigned to the UE.
  const auto& srs_res_list = ue.value().cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg->srs_res_list;
  ASSERT_FALSE(srs_res_list.empty());
  // Check if the SRS resource collides (i.e., is not orthogonal) with any other SRS resource.
  const auto&          srs_res = srs_res_list[0];
  const srs_res_params res_params(srs_res);
  ASSERT_FALSE(std::any_of(used_srs_resources.begin(),
                           used_srs_resources.end(),
                           [&res_params](const srs_res_params& res) { return res.collides(res_params); }));
}

TEST_P(du_srs_resource_manager_tester, srs_resources_parameters_are_valid)
{
  // > Created UEs have unique SRS resources.
  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    std::optional<cell_group_config> ue = add_ue(to_du_ue_index(i));
    if (not ue.has_value()) {
      break;
    }

    // Verify all parameters of the SRS resource are as expected.
    ASSERT_TRUE(ue.value().cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value());
    const auto& ue_srs_config = ue.value().cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
    // Verify first the SRS resource set list.
    ASSERT_EQ(ue_srs_config.srs_res_set_list.size(), 1U);
    ASSERT_EQ(ue_srs_config.srs_res_set_list[0].id, 0U);
    ASSERT_TRUE(std::holds_alternative<srs_config::srs_resource_set::periodic_resource_type>(
        ue_srs_config.srs_res_set_list[0].res_type));
    ASSERT_EQ(ue_srs_config.srs_res_set_list[0].srs_res_id_list.size(), 1U);
    ASSERT_EQ(ue_srs_config.srs_res_set_list[0].srs_res_id_list[0], 0U);
    ASSERT_EQ(ue_srs_config.srs_res_list.size(), 1U);
    const auto& srs_res = ue_srs_config.srs_res_list[0];
    ASSERT_EQ(srs_res.id.ue_res_id, static_cast<srs_config::srs_res_id>(0U));
    ASSERT_TRUE(srs_res.periodicity_and_offset.has_value());
    ASSERT_EQ(srs_res.periodicity_and_offset->period, srs_params.srs_period.value());
    ASSERT_LT(srs_res.periodicity_and_offset->offset, static_cast<unsigned>(srs_res.periodicity_and_offset->period));
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
      ASSERT_TRUE(is_ul_slot(srs_res.periodicity_and_offset->offset, cell_cfg_list[0].tdd_ul_dl_cfg_common.value()));
    }
    ASSERT_LT(srs_res.tx_comb.tx_comb_offset, static_cast<unsigned>(srs_res.tx_comb.size));
    ASSERT_LT(srs_res.tx_comb.tx_comb_cyclic_shift, srs_res.tx_comb.size == tx_comb_size::n2 ? 8U : 12U);

    ASSERT_EQ(srs_res.freq_hop.c_srs, compute_c_srs());
    ASSERT_EQ(srs_res.freq_hop.b_srs, 0U);
    ASSERT_EQ(srs_res.freq_hop.b_hop, 0U);
    ASSERT_EQ(srs_res.freq_domain_shift, compute_freq_shift());
    ASSERT_EQ(srs_res.freq_domain_pos, 0U);

    // Verify the symbols, depending on whether it's FDD, or TDD.
    ASSERT_EQ(srs_res.res_mapping.nof_symb, srs_params.nof_symbols);
    if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value() and
        is_partially_ul_slot(srs_res.periodicity_and_offset->offset, cell_cfg_list[0].tdd_ul_dl_cfg_common.value())) {
      ASSERT_LT(srs_res.res_mapping.start_pos, cell_cfg_list[0].tdd_ul_dl_cfg_common.value().pattern1.nof_ul_symbols);
    } else {
      ASSERT_LT(srs_res.res_mapping.start_pos, srs_params.max_nof_symbols.to_uint());
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_du_srs_res_mng_for_different_ul_symbols,
                         du_srs_resource_manager_tester,
                         ::testing::Values(srs_params{.nof_ul_symbols_p1 = std::nullopt},
                                           srs_params{.nof_ul_symbols_p1 = 0U},
                                           srs_params{.nof_ul_symbols_p1 = 1U},
                                           srs_params{.nof_ul_symbols_p1 = 2U},
                                           srs_params{.nof_ul_symbols_p1 = 3U},
                                           srs_params{.nof_ul_symbols_p1 = 4U},
                                           srs_params{.nof_ul_symbols_p1 = 5U},
                                           srs_params{.nof_ul_symbols_p1 = 6U},
                                           srs_params{.nof_ul_symbols_p1 = 7U}),
                         [](const testing::TestParamInfo<srs_params>& params_item) {
                           return fmt::format("{}", params_item.param);
                         });

/////////     Test the optimality of DU SRS resource manager policy      /////////

class du_srs_resource_manager_tester_optimality : public du_srs_resource_manager_tester
{
protected:
  explicit du_srs_resource_manager_tester_optimality()
  {
    nof_srs_res_per_symb_interval = static_cast<unsigned>(srs_params.tx_comb) *
                                    static_cast<unsigned>(srs_params.cyclic_shift_reuse_factor) *
                                    static_cast<unsigned>(srs_params.sequence_id_reuse_factor);
    srs_res_tracker.reserve(static_cast<unsigned>(srs_params.srs_period.value()));

    // Build the SRS resource tracker.
    for (unsigned offset = 0; offset != static_cast<unsigned>(srs_params.srs_period.value()); ++offset) {
      if (cell_cfg_list[0].tdd_ul_dl_cfg_common.has_value()) {
        const auto& tdd_cfg = cell_cfg_list[0].tdd_ul_dl_cfg_common.value();

        if (not is_ul_slot(offset, cell_cfg_list[0].tdd_ul_dl_cfg_common.value())) {
          // In TDD, no SRS resources can be allocated in DL slots.
          srs_res_tracker.emplace_back(0U);
        } else if (is_partially_ul_slot(offset, cell_cfg_list[0].tdd_ul_dl_cfg_common.value())) {
          const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
          const unsigned nof_ul_symbols =
              srsran::get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length();
          // The number of symbols intervals is limited by the number of symbols where the SRS can be placed.
          const unsigned nof_intervals = std::min(nof_ul_symbols / static_cast<unsigned>(srs_params.nof_symbols),
                                                  srs_params.max_nof_symbols.max());
          srs_res_tracker.emplace_back(nof_intervals);
        } else {
          const unsigned nof_intervals =
              srs_params.max_nof_symbols.to_uint() / static_cast<unsigned>(srs_params.nof_symbols);
          srs_res_tracker.emplace_back(nof_intervals);
        }
      }
      // FDD case.
      else {
        const unsigned nof_intervals =
            srs_params.max_nof_symbols.to_uint() / static_cast<unsigned>(srs_params.nof_symbols);
        srs_res_tracker.emplace_back(nof_intervals);
      }
    }
  }

  // Helper struct that keeps track of the number of SRS resources per symbol interval for a given offset.
  struct res_per_slot_tracker {
    explicit res_per_slot_tracker(unsigned nof_srs_symb_intervals_per_slot_) :
      nof_srs_symb_intervals_per_slot(nof_srs_symb_intervals_per_slot_)
    {
      // Initialize the vector with zeros, as at the beginning no SRS resources have been allocated.
      res_per_symb_interval.assign(nof_srs_symb_intervals_per_slot, 0U);
    }

    const unsigned nof_srs_symb_intervals_per_slot;
    // This vector keeps track of the number of SRS resources per symbol interval. The symbol interval with index 0 is
    // the last one in the slot; the symbol interval with index 1 is the second to last, and so on.
    std::vector<unsigned> res_per_symb_interval;
  };

  // The number of SRS resources per symbol interval, which depends on the TX comb size, on the cyclic shift reuse
  // factor, and on the sequence ID reuse factor.
  unsigned nof_srs_res_per_symb_interval = 0;
  // This vector keeps track of the number of SRS resources per symbol interval for each offset.
  std::vector<res_per_slot_tracker> srs_res_tracker;

  // Save the SRS resource allocation in the tracker.
  void track_srs_res_alloc(const srs_config::srs_resource& srs_res)
  {
    srsran_assert(srs_res.periodicity_and_offset.has_value(), "SRS resource must have a periodicity and offset");
    const unsigned srs_offset   = srs_res.periodicity_and_offset->offset;
    const unsigned interval_idx = srs_res.res_mapping.start_pos / static_cast<uint8_t>(srs_res.res_mapping.nof_symb);
    srsran_assert(interval_idx < srs_res_tracker[srs_offset].res_per_symb_interval.size(),
                  "SRS resource must have a periodicity and offset");
    ++srs_res_tracker[srs_offset].res_per_symb_interval[interval_idx];
  }

  // Check if there is any partially-UL slot that has not completely filled with SRS resources.
  bool has_free_partial_ul_slots()
  {
    for (unsigned n = 0; n != srs_res_tracker.size(); ++n) {
      if (not is_partially_ul_slot(n, params.tdd_ul_dl_cfg_common.value())) {
        continue;
      }
      if (std::any_of(srs_res_tracker[n].res_per_symb_interval.begin(),
                      srs_res_tracker[n].res_per_symb_interval.end(),
                      [this](unsigned cnt) { return cnt < nof_srs_res_per_symb_interval; })) {
        return true;
      }
    }
    return false;
  }

  // Check if all the symbol intervals with index lower than the one of the given SRS resource have
  // been used (in fully-UL slots).
  bool all_lower_intervals_used(const srs_config::srs_resource& srs_res)
  {
    const unsigned interval_idx = srs_res.res_mapping.start_pos / static_cast<uint8_t>(srs_res.res_mapping.nof_symb);
    if (interval_idx == 0) {
      return true;
    }

    for (unsigned n = 0; n != srs_res_tracker.size(); ++n) {
      if (params.tdd_ul_dl_cfg_common.has_value() and (not is_ul_slot(n, params.tdd_ul_dl_cfg_common.value()) or
                                                       is_partially_ul_slot(n, params.tdd_ul_dl_cfg_common.value()))) {
        continue;
      }
      if (std::any_of(srs_res_tracker[n].res_per_symb_interval.begin(),
                      srs_res_tracker[n].res_per_symb_interval.begin() + interval_idx,
                      [this](unsigned cnt) { return cnt < nof_srs_res_per_symb_interval; })) {
        return false;
      }
    }
    return true;
  }

  // Check if there is any symbol interval of the same index of the one of the given SRS resource that is not empty, but
  // not full either.
  bool non_empty_non_full_interval(const srs_config::srs_resource& srs_res)
  {
    const unsigned interval_idx = srs_res.res_mapping.start_pos / static_cast<uint8_t>(srs_res.res_mapping.nof_symb);
    for (unsigned n = 0; n != srs_res_tracker.size(); ++n) {
      // Skip the slot where the SRS resource has been allocated, otherwise this will always return true.
      if (n == srs_res.periodicity_and_offset->offset) {
        continue;
      }
      if (params.tdd_ul_dl_cfg_common.has_value() and (not is_ul_slot(n, params.tdd_ul_dl_cfg_common.value()) or
                                                       is_partially_ul_slot(n, params.tdd_ul_dl_cfg_common.value()))) {
        continue;
      }
      srsran_assert(interval_idx < srs_res_tracker[n].res_per_symb_interval.size(),
                    "Interval index exceeds the size of the vector");
      if (srs_res_tracker[n].res_per_symb_interval[interval_idx] > 0U and
          srs_res_tracker[n].res_per_symb_interval[interval_idx] < nof_srs_res_per_symb_interval) {
        return true;
      }
    }
    return false;
  }
};

TEST_P(du_srs_resource_manager_tester_optimality, srs_are_assigned_according_to_class_policy)
{
  // > Created UEs have unique SRS resources.
  for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
    std::optional<cell_group_config> ue = add_ue(to_du_ue_index(i));
    if (not ue.has_value()) {
      break;
    }

    // Verify all parameters of the SRS resource are as expected.
    ASSERT_TRUE(ue.value().cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value());
    const auto& ue_srs_config = ue.value().cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
    const auto& srs_res       = ue_srs_config.srs_res_list[0];

    // Save the SRS resource allocation in the tracker.
    track_srs_res_alloc(srs_res);

    // The SRS allocation assigns resources according to the policy:
    // - If there is a partially-UL slot available, then the SRS resources are allocated in the partially-UL slots.
    // - If there are no partially-UL slots, then the SRS resources are allocated in the fully-UL slots.
    // - If there are no fully-UL slots with a symbol interval of index i available, then the SRS resources are
    // allocated in a symbol interval of index i+1, in any slot.
    // - If there is a fully-UL slots with a symbol interval of index i available that has already SRS resources
    // allocated, but has not been completely filled, then the SRS resources are allocated in that slot.
    if (params.tdd_ul_dl_cfg_common.has_value()) {
      if (not is_partially_ul_slot(srs_res.periodicity_and_offset.value().offset,
                                   params.tdd_ul_dl_cfg_common.value())) {
        ASSERT_FALSE(has_free_partial_ul_slots());
        ASSERT_TRUE(all_lower_intervals_used(srs_res));
        ASSERT_FALSE(non_empty_non_full_interval(srs_res));
      }
    } else {
      ASSERT_TRUE(all_lower_intervals_used(srs_res));
      ASSERT_FALSE(non_empty_non_full_interval(srs_res));
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_du_srs_res_mng_for_different_ul_symbols,
                         du_srs_resource_manager_tester_optimality,
                         ::testing::Values(srs_params{.nof_ul_symbols_p1 = std::nullopt, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 0U, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 1U, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 2U, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 3U, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 4U, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 5U, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 6U, .test_optimality = true},
                                           srs_params{.nof_ul_symbols_p1 = 7U, .test_optimality = true}),
                         [](const testing::TestParamInfo<srs_params>& params_item) {
                           return fmt::format("{}", params_item.param);
                         });
