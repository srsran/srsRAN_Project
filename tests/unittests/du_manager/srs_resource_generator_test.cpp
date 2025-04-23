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

#include "lib/du/du_high/du_manager/ran_resource_management/srs_resource_generator.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

namespace {
struct srs_cfg_builder_params {
  std::optional<unsigned>           nof_ul_symbols            = std::nullopt;
  bounded_integer<unsigned, 1U, 6U> max_nof_symbols           = 2U;
  tx_comb_size                      tx_comb                   = tx_comb_size::n4;
  srs_nof_symbols                   nof_symbols               = n1;
  nof_cyclic_shifts                 cyclic_shift_reuse_factor = nof_cyclic_shifts::no_cyclic_shift;
  unsigned                          sequence_id_reuse_factor  = 1;
};
} // namespace

class test_srs_res_list_builder : public ::testing::TestWithParam<srs_cfg_builder_params>
{
protected:
  test_srs_res_list_builder()
  {
    // Validate the input parameters
    if (GetParam().tx_comb == tx_comb_size::n2) {
      /// Cyclic shift that we can use with the n2 TX comb size.
      const std::array<nof_cyclic_shifts, 3> css{
          nof_cyclic_shifts::no_cyclic_shift, nof_cyclic_shifts::two, nof_cyclic_shifts::four};
      srsran_assert(std::find(css.cbegin(), css.cend(), GetParam().cyclic_shift_reuse_factor) != css.end(),
                    "Cyclic shift reuse factor set is not compatible with the TX comb size");
    }
    srsran_assert(GetParam().max_nof_symbols.to_uint() >= static_cast<unsigned>(GetParam().nof_symbols),
                  "The number of symbols per SRS resource cannot be larger than the maximum number of symbols for the "
                  "entire SRS area");

    // Default PCI.
    du_cell_cfg.pci = 1U;

    // In the TDD configuration, the only parameter that matters is the number of UL symbols.
    if (GetParam().nof_ul_symbols.has_value()) {
      tdd_ul_dl_config_common tdd_cfg = {.ref_scs  = srsran::subcarrier_spacing::kHz30,
                                         .pattern1 = tdd_ul_dl_pattern{10U, 6, 0, 3, GetParam().nof_ul_symbols.value()},
                                         .pattern2 = std::nullopt};
      du_cell_cfg.tdd_ul_dl_cfg_common.emplace(tdd_cfg);
    }

    auto& srs_cfg = du_cell_cfg.srs_cfg;
    srs_cfg.srs_period.emplace(srs_periodicity::sl20);
    srs_cfg.tx_comb                   = GetParam().tx_comb;
    srs_cfg.nof_symbols               = GetParam().nof_symbols;
    srs_cfg.cyclic_shift_reuse_factor = GetParam().cyclic_shift_reuse_factor;
    srs_cfg.sequence_id_reuse_factor  = GetParam().sequence_id_reuse_factor;
    srs_cfg.max_nof_symbols           = GetParam().max_nof_symbols;

    nof_symbols_srs_area = du_cell_cfg.srs_cfg.max_nof_symbols.to_uint();
    if (du_cell_cfg.tdd_ul_dl_cfg_common.has_value()) {
      const auto& tdd_cfg  = du_cell_cfg.tdd_ul_dl_cfg_common.value();
      nof_symbols_srs_area = std::max(nof_symbols_srs_area, tdd_cfg.pattern1.nof_ul_symbols);
      if (tdd_cfg.pattern2.has_value()) {
        nof_symbols_srs_area = std::max(nof_symbols_srs_area, tdd_cfg.pattern2.value().nof_ul_symbols);
      }
      // The number of SRS symbols cannot be larger than 6.
      nof_symbols_srs_area = std::min(nof_symbols_srs_area, 6U);
    }
    nof_symbol_intervals = nof_symbols_srs_area / static_cast<unsigned>(du_cell_cfg.srs_cfg.nof_symbols);
  }

  unsigned compute_expected_srs_list_size() const
  {
    return nof_symbol_intervals * static_cast<unsigned>(du_cell_cfg.srs_cfg.tx_comb) *
           static_cast<unsigned>(du_cell_cfg.srs_cfg.cyclic_shift_reuse_factor) *
           static_cast<unsigned>(du_cell_cfg.srs_cfg.sequence_id_reuse_factor);
  }

  bool check_res_list_elements(span<const du_srs_resource> srs_res_list) const
  {
    const unsigned max_cs = du_cell_cfg.srs_cfg.tx_comb == srsran::tx_comb_size::n2 ? 8U : 12U;
    for (const auto& srs_res : srs_res_list) {
      if (srs_res.tx_comb_offset >= static_cast<unsigned>(du_cell_cfg.srs_cfg.tx_comb)) {
        return false;
      }
      if (srs_res.cs >= max_cs) {
        return false;
      }
    }

    // Count the number of SRS resources for each TX comb offset.
    const std::vector<unsigned> tx_comb_offsets = du_cell_cfg.srs_cfg.tx_comb == srsran::tx_comb_size::n2
                                                      ? std::vector<unsigned>{0U, 1U}
                                                      : std::vector<unsigned>{0U, 1U, 2U, 3U};
    for (const auto& comb_offset : tx_comb_offsets) {
      const auto nof_elements = static_cast<unsigned>(
          std::count_if(srs_res_list.begin(), srs_res_list.end(), [comb_offset](const du_srs_resource& res) {
            return res.tx_comb_offset.to_uint() == comb_offset;
          }));

      if (nof_elements != compute_expected_srs_list_size() / tx_comb_offsets.size()) {
        return false;
      }
    }

    // Count the number of SRS resources for each cyclic shift value.
    const unsigned        cs_step = max_cs / static_cast<unsigned>(du_cell_cfg.srs_cfg.cyclic_shift_reuse_factor);
    std::vector<unsigned> cs_values;
    for (unsigned cs = 0; cs < max_cs; cs += cs_step) {
      cs_values.push_back(cs);
    }
    for (auto cs : cs_values) {
      const auto nof_elements = static_cast<unsigned>(std::count_if(
          srs_res_list.begin(), srs_res_list.end(), [cs](const du_srs_resource& res) { return res.cs == cs; }));
      if (nof_elements != compute_expected_srs_list_size() / cs_values.size()) {
        return false;
      }
    }

    // Count the number of SRS resources for each Sequence Index.
    constexpr unsigned max_seq_id_values = 30U;
    const unsigned     seq_id_step =
        max_seq_id_values / static_cast<unsigned>(du_cell_cfg.srs_cfg.sequence_id_reuse_factor);
    std::vector<unsigned> seq_id_values;
    for (unsigned seq_id = 0; seq_id < max_seq_id_values; seq_id += seq_id_step) {
      seq_id_values.push_back(static_cast<unsigned>(du_cell_cfg.pci) + seq_id);
    }
    for (auto seq_id : seq_id_values) {
      const auto nof_elements = static_cast<unsigned>(
          std::count_if(srs_res_list.begin(), srs_res_list.end(), [seq_id](const du_srs_resource& res) {
            return res.sequence_id == seq_id;
          }));
      if (nof_elements != compute_expected_srs_list_size() / seq_id_values.size()) {
        return false;
      }
    }

    // Count the number of SRS resources for each symbol interval.
    for (unsigned sym_start = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - static_cast<unsigned>(du_cell_cfg.srs_cfg.nof_symbols);
         sym_start >= NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - nof_symbols_srs_area;
         sym_start -= static_cast<unsigned>(du_cell_cfg.srs_cfg.nof_symbols)) {
      const auto nof_elements = static_cast<unsigned>(std::count_if(
          srs_res_list.begin(),
          srs_res_list.end(),
          [sym_start, srs_sym_length = du_cell_cfg.srs_cfg.nof_symbols](const du_srs_resource& res) {
            return res.symbols == ofdm_symbol_range{sym_start, sym_start + static_cast<unsigned>(srs_sym_length)};
          }));
      if (nof_elements != compute_expected_srs_list_size() / nof_symbol_intervals) {
        return false;
      }
    }

    return true;
  }

  du_cell_config du_cell_cfg;
  // Values that are used to check the SRS list.
  // Number of symbols that are reserved for SRS.
  unsigned nof_symbols_srs_area = 0;
  unsigned nof_symbol_intervals = 0;
};

TEST_P(test_srs_res_list_builder, test_whether_list_is_generated_and_has_correct_size)
{
  auto srs_res_list = generate_cell_srs_list(du_cell_cfg);
  ASSERT_FALSE(srs_res_list.empty());
  ASSERT_EQ(compute_expected_srs_list_size(), srs_res_list.size());
  ASSERT_TRUE(check_res_list_elements(srs_res_list));
}

INSTANTIATE_TEST_SUITE_P(
    test_both_fdd_and_tdd_srs_res_list_builder,
    test_srs_res_list_builder,
    // clang-format off
    ::testing::Values(
        // FDD.
        srs_cfg_builder_params{std::nullopt, 2, tx_comb_size::n4, srs_nof_symbols::n1, nof_cyclic_shifts::no_cyclic_shift,1},
        srs_cfg_builder_params{std::nullopt, 2, tx_comb_size::n4, srs_nof_symbols::n2, nof_cyclic_shifts::no_cyclic_shift, 1},
        srs_cfg_builder_params{std::nullopt, 3, tx_comb_size::n2, srs_nof_symbols::n1, nof_cyclic_shifts::two, 1},
        srs_cfg_builder_params{std::nullopt, 6, tx_comb_size::n2, srs_nof_symbols::n1, nof_cyclic_shifts::no_cyclic_shift, 6},
        srs_cfg_builder_params{std::nullopt, 6, tx_comb_size::n4, srs_nof_symbols::n2, nof_cyclic_shifts::twelve, 5},
        srs_cfg_builder_params{std::nullopt, 4, tx_comb_size::n4, srs_nof_symbols::n2, nof_cyclic_shifts::six, 15},
        srs_cfg_builder_params{std::nullopt, 4, tx_comb_size::n4, srs_nof_symbols::n4, nof_cyclic_shifts::six, 15},
        srs_cfg_builder_params{std::nullopt, 6, tx_comb_size::n4, srs_nof_symbols::n1, nof_cyclic_shifts::twelve, 30},
        // TDD.
        srs_cfg_builder_params{3, 2, tx_comb_size::n4, srs_nof_symbols::n1, nof_cyclic_shifts::no_cyclic_shift,1},
        srs_cfg_builder_params{5, 2, tx_comb_size::n4, srs_nof_symbols::n1, nof_cyclic_shifts::no_cyclic_shift,1},
        srs_cfg_builder_params{4, 2, tx_comb_size::n4, srs_nof_symbols::n2, nof_cyclic_shifts::no_cyclic_shift, 1},
        srs_cfg_builder_params{5, 2, tx_comb_size::n4, srs_nof_symbols::n2, nof_cyclic_shifts::no_cyclic_shift, 1},
        srs_cfg_builder_params{2, 3, tx_comb_size::n2, srs_nof_symbols::n1, nof_cyclic_shifts::two, 1},
        srs_cfg_builder_params{3, 6, tx_comb_size::n2, srs_nof_symbols::n1, nof_cyclic_shifts::no_cyclic_shift, 6},
        srs_cfg_builder_params{2, 4, tx_comb_size::n4, srs_nof_symbols::n4, nof_cyclic_shifts::six, 15},
        srs_cfg_builder_params{6, 4, tx_comb_size::n4, srs_nof_symbols::n2, nof_cyclic_shifts::twelve, 6}
    )
    // clang-format on
);
