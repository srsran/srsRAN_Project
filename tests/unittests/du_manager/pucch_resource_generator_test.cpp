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

#include "lib/du_manager/ran_resource_management/pucch_resource_generator.h"
#include "tests/unittests/scheduler/test_utils/config_generators.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

namespace du_pucch_gen {
// Test struct.
struct pucch_gen_params {
  unsigned                         nof_res_f0;
  unsigned                         nof_res_f1;
  unsigned                         nof_res_f2;
  bounded_integer<unsigned, 1, 2>  f0_nof_symbols{2};
  bool                             f0_intraslot_freq_hopping{false};
  nof_cyclic_shifts                nof_cyc_shifts{nof_cyclic_shifts::no_cyclic_shift};
  bool                             occ_supported{false};
  bounded_integer<unsigned, 4, 14> f1_nof_symbols{14};
  bool                             f1_intraslot_freq_hopping{false};
  bounded_integer<unsigned, 1, 2>  f2_nof_symbols{1};
  unsigned                         max_nof_rbs{1};
  std::optional<unsigned>          max_payload_bits;
  max_pucch_code_rate              max_code_rate{max_pucch_code_rate::dot_25};
  bool                             f2_intraslot_freq_hopping{false};
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const pucch_gen_params& value, ::std::ostream* os)
{
  return;
}

struct pucch_cfg_builder_params {
  unsigned nof_res_f0_harq = 0;
  unsigned nof_res_f1_harq = 3;
  unsigned nof_res_f2_harq = 6;
  unsigned nof_harq_cfg    = 1;
  unsigned nof_res_sr      = 2;
  unsigned nof_res_csi     = 1;
};

} // namespace du_pucch_gen

using namespace du_pucch_gen;

// This class implements a resource grid for PUCCH resources. It allocates the resource on the grid and verify whether a
// resource collides with the previously allocated ones.
// For FORMAT 1, it verifies whether the resource has a different OCC or CS code with respect resource allocated on the
// same PRBs and symbols.
class pucch_grid
{
public:
  pucch_grid(unsigned nof_rbs_, unsigned nof_symbols_) : nof_rbs{nof_rbs_}, nof_symbols{nof_symbols_}
  {
    grid.resize(nof_rbs * nof_symbols);
    for (auto& grid_elem : grid) {
      for (auto& occ_cs : grid_elem.allocated_occ_cs_list) {
        occ_cs = false;
      }
    }
  }

  // Allocate the resource on the grid.
  void add_resource(const pucch_resource& res)
  {
    if (res.format == pucch_format::FORMAT_0) {
      srsran_assert(std::holds_alternative<pucch_format_0_cfg>(res.format_params), "Expected PUCCH Format 0");
      const auto& res_f0 = std::get<pucch_format_0_cfg>(res.format_params);

      if (res.second_hop_prb.has_value()) {
        srsran_assert(res_f0.nof_symbols == 2U, "Intra-slot freq. hopping for PUCCH Format 0 requires 2 symbols");
        // First hop.
        for (unsigned sym_idx = res_f0.starting_sym_idx; sym_idx < res_f0.starting_sym_idx + res_f0.nof_symbols / 2;
             ++sym_idx) {
          auto& grid_elem        = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format       = pucch_format::FORMAT_0;
          grid_elem.element_used = true;
        }
        // Second hop.
        for (unsigned sym_idx = res_f0.starting_sym_idx + res_f0.nof_symbols / 2;
             sym_idx < res_f0.starting_sym_idx + res_f0.nof_symbols;
             ++sym_idx) {
          auto& grid_elem        = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          grid_elem.format       = pucch_format::FORMAT_0;
          grid_elem.element_used = true;
        }
      } else {
        for (unsigned sym_idx = res_f0.starting_sym_idx; sym_idx < res_f0.starting_sym_idx + res_f0.nof_symbols;
             ++sym_idx) {
          auto& grid_elem        = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format       = pucch_format::FORMAT_0;
          grid_elem.element_used = true;
        }
      }

    } else if (res.format == pucch_format::FORMAT_1) {
      srsran_assert(std::holds_alternative<pucch_format_1_cfg>(res.format_params), "Expected PUCCH Format 1");
      const auto& res_f1 = std::get<pucch_format_1_cfg>(res.format_params);

      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned sym_idx = res_f1.starting_sym_idx; sym_idx < res_f1.starting_sym_idx + res_f1.nof_symbols / 2;
             ++sym_idx) {
          auto& grid_elem           = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format          = pucch_format::FORMAT_1;
          grid_elem.element_used    = true;
          const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
          srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
          srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
          grid_elem.allocated_occ_cs_list[occ_cs_idx] = true;
        }
        // Second hop.
        for (unsigned sym_idx = res_f1.starting_sym_idx + res_f1.nof_symbols / 2;
             sym_idx < res_f1.starting_sym_idx + res_f1.nof_symbols;
             ++sym_idx) {
          auto& grid_elem           = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          grid_elem.format          = pucch_format::FORMAT_1;
          grid_elem.element_used    = true;
          const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
          srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
          srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
          grid_elem.allocated_occ_cs_list[occ_cs_idx] = true;
        }
      } else {
        for (unsigned sym_idx = res_f1.starting_sym_idx; sym_idx < res_f1.starting_sym_idx + res_f1.nof_symbols;
             ++sym_idx) {
          auto& grid_elem           = grid[sym_idx + nof_symbols * res.starting_prb];
          grid_elem.format          = pucch_format::FORMAT_1;
          grid_elem.element_used    = true;
          const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
          srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
          srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
          grid_elem.allocated_occ_cs_list[occ_cs_idx] = true;
        }
      }

    } else if (res.format == srsran::pucch_format::FORMAT_2) {
      srsran_assert(std::holds_alternative<pucch_format_2_3_cfg>(res.format_params), "Expected PUCCH Format 2");
      const auto& res_f2 = std::get<pucch_format_2_3_cfg>(res.format_params);

      if (res.second_hop_prb.has_value()) {
        srsran_assert(res_f2.nof_symbols == 2U, "Intra-slot freq. hopping for PUCCH Format 0 requires 2 symbols");
        // First hop.
        for (unsigned rb_idx = res.starting_prb; rb_idx < res.starting_prb + res_f2.nof_prbs; ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx; sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols / 2;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(); rb_idx < res.second_hop_prb.value() + res_f2.nof_prbs;
             ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx + res_f2.nof_symbols / 2;
               sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
      } else {
        for (unsigned rb_idx = res.starting_prb; rb_idx < res.starting_prb + res_f2.nof_prbs; ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx; sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
      }
    }
  }

  // Verify if the given resource collides in the grid with the previously allocated resources.
  bool verify_collision(const pucch_resource& res) const
  {
    if (res.format == pucch_format::FORMAT_0) {
      srsran_assert(std::holds_alternative<pucch_format_0_cfg>(res.format_params), "Expected PUCCH Format 0");
      const auto& res_f0 = std::get<pucch_format_0_cfg>(res.format_params);
      // Intra-slot frequency hopping.
      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned sym_idx = res_f0.starting_sym_idx; sym_idx < res_f0.starting_sym_idx + res_f0.nof_symbols / 2;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            return true;
          }
        }
        // Second hop.
        for (unsigned sym_idx = res_f0.starting_sym_idx + res_f0.nof_symbols / 2;
             sym_idx < res_f0.starting_sym_idx + res_f0.nof_symbols;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          if (grid_elem.element_used) {
            return true;
          }
        }

      }
      // No intra-slot frequency hopping.
      else {
        for (unsigned sym_idx = res_f0.starting_sym_idx; sym_idx < res_f0.starting_sym_idx + res_f0.nof_symbols;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            return true;
          }
        }
      }
    } else if (res.format == pucch_format::FORMAT_1) {
      srsran_assert(std::holds_alternative<pucch_format_1_cfg>(res.format_params), "Expected PUCCH Format 1");
      const auto& res_f1 = std::get<pucch_format_1_cfg>(res.format_params);
      // Intra-slot frequency hopping.
      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned sym_idx = res_f1.starting_sym_idx; sym_idx < res_f1.starting_sym_idx + res_f1.nof_symbols / 2;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            if (grid_elem.format != pucch_format::FORMAT_1) {
              return true;
            }
            const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
            srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
            srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
            if (grid_elem.allocated_occ_cs_list[occ_cs_idx]) {
              return true;
            }
          }
        }
        // Second hop.
        for (unsigned sym_idx = res_f1.starting_sym_idx + res_f1.nof_symbols / 2;
             sym_idx < res_f1.starting_sym_idx + res_f1.nof_symbols;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.second_hop_prb.value()];
          if (grid_elem.element_used) {
            if (grid_elem.format != pucch_format::FORMAT_1) {
              return true;
            }
            const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
            srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
            srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
            if (grid_elem.allocated_occ_cs_list[occ_cs_idx]) {
              return true;
            }
          }
        }

      }
      // No intra-slot frequency hopping.
      else {
        for (unsigned sym_idx = res_f1.starting_sym_idx; sym_idx < res_f1.starting_sym_idx + res_f1.nof_symbols;
             ++sym_idx) {
          const auto& grid_elem = grid[sym_idx + nof_symbols * res.starting_prb];
          if (grid_elem.element_used) {
            if (grid_elem.format != pucch_format::FORMAT_1) {
              return true;
            }
            const unsigned occ_cs_idx = res_f1.initial_cyclic_shift + res_f1.time_domain_occ * max_nof_cs;
            srsran_assert(res_f1.initial_cyclic_shift < max_nof_cs, "Initial Cyclic Shift exceeds maximum value 11");
            srsran_assert(res_f1.time_domain_occ < max_nof_occ, " Time Domain OCC exceeds maximum value 7");
            if (grid_elem.allocated_occ_cs_list[occ_cs_idx]) {
              return true;
            }
          }
        }
      }
    } else if (res.format == srsran::pucch_format::FORMAT_2) {
      srsran_assert(std::holds_alternative<pucch_format_2_3_cfg>(res.format_params), "Expected PUCCH Format 2");
      const auto& res_f2 = std::get<pucch_format_2_3_cfg>(res.format_params);

      // Intra-slot frequency hopping.
      if (res.second_hop_prb.has_value()) {
        // First hop.
        for (unsigned rb_idx = res.starting_prb; rb_idx < res.starting_prb + res_f2.nof_prbs; ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx; sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols / 2;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              return true;
            }
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(); rb_idx < res.second_hop_prb.value() + res_f2.nof_prbs;
             ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx + res_f2.nof_symbols / 2;
               sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              return true;
            }
          }
        }
      }
      // No intra-slot frequency hopping.
      else {
        for (unsigned rb_idx = res.starting_prb; rb_idx < res.starting_prb + res_f2.nof_prbs; ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx; sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols;
               ++sym_idx) {
            const auto& grid_unit = grid[sym_idx + nof_symbols * rb_idx];
            if (grid_unit.element_used) {
              return true;
            }
          }
        }
      }

    } else {
      return true;
    }
    return false;
  }

private:
  // Maximum 7 different OCCs, as per \c PUCCH-format1, in \c PUCCH-Config, TS 38.331.
  static const unsigned max_nof_occ = 7;
  // Maximum 12 different CSs, as per \c PUCCH-format1, in \c PUCCH-Config, TS 38.331.
  static const unsigned max_nof_cs     = 12;
  static const unsigned max_nof_occ_cs = max_nof_occ * max_nof_cs;
  struct grid_element {
    bool         element_used;
    pucch_format format;
    // Saves the used OCCs and CSs for a given resource element.
    std::array<bool, max_nof_occ_cs> allocated_occ_cs_list;
  };

  const unsigned            nof_rbs;
  const unsigned            nof_symbols;
  std::vector<grid_element> grid;
};

class test_pucch_res_generator : public ::testing::Test
{
public:
  test_pucch_res_generator() : grid(bwp_size, nof_symbols_per_slot){};

protected:
  // Parameters that are passed by the routing to run the tests.
  const unsigned bwp_size{106};
  const unsigned nof_symbols_per_slot{14};
  pucch_grid     grid;
};

class test_pucch_res_generator_params : public ::testing::TestWithParam<pucch_gen_params>
{
public:
  test_pucch_res_generator_params() : grid(bwp_size, nof_symbols_per_slot){};

protected:
  // Parameters that are passed by the routing to run the tests.
  const unsigned bwp_size{106};
  const unsigned nof_symbols_per_slot{14};
  pucch_grid     grid;
};

TEST_P(test_pucch_res_generator_params, test_pucch_res_given_number)
{
  const unsigned nof_res_f0 = GetParam().nof_res_f0;
  const unsigned nof_res_f1 = GetParam().nof_res_f1;
  const unsigned nof_res_f2 = GetParam().nof_res_f2;

  ASSERT_FALSE(nof_res_f0 != 0 and nof_res_f1 != 0) << "PUCCH Format 0 and Format 1 resources cannot be used together";

  pucch_f0_params params_f0{.nof_symbols            = GetParam().f0_nof_symbols,
                            .intraslot_freq_hopping = GetParam().f0_intraslot_freq_hopping};

  pucch_f1_params params_f1{.nof_cyc_shifts         = GetParam().nof_cyc_shifts,
                            .occ_supported          = GetParam().occ_supported,
                            .nof_symbols            = GetParam().f1_nof_symbols,
                            .intraslot_freq_hopping = GetParam().f1_intraslot_freq_hopping};

  pucch_f2_params params_f2{.nof_symbols            = GetParam().f2_nof_symbols,
                            .max_nof_rbs            = GetParam().max_nof_rbs,
                            .max_payload_bits       = GetParam().max_payload_bits,
                            .max_code_rate          = GetParam().max_code_rate,
                            .intraslot_freq_hopping = GetParam().f2_intraslot_freq_hopping};

  std::vector<pucch_resource> res_list;
  if (nof_res_f0 != 0) {
    res_list = generate_cell_pucch_res_list(nof_res_f0, nof_res_f2, params_f0, params_f2, bwp_size);
  } else {
    res_list = generate_cell_pucch_res_list(nof_res_f1, nof_res_f2, params_f1, params_f2, bwp_size);
  }

  ASSERT_TRUE(res_list.size() > 0);
  if (nof_res_f0 != 0) {
    ASSERT_EQ(nof_res_f0 + nof_res_f2, res_list.size());
  } else {
    ASSERT_EQ(nof_res_f1 + nof_res_f2, res_list.size());
  }

  for (const auto& pucch_res : res_list) {
    ASSERT_FALSE(grid.verify_collision(pucch_res));
    grid.add_resource(pucch_res);
  }
}

INSTANTIATE_TEST_SUITE_P(test_res_generation_given_number,
                         test_pucch_res_generator_params,
                         ::testing::Values(pucch_gen_params{.nof_res_f1                = 15,
                                                            .nof_res_f2                = 10,
                                                            .nof_cyc_shifts            = nof_cyclic_shifts::twelve,
                                                            .occ_supported             = false,
                                                            .f1_nof_symbols            = 7,
                                                            .f1_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 2,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_25,
                                                            .f2_intraslot_freq_hopping = false},
                                           pucch_gen_params{.nof_res_f1                = 39,
                                                            .nof_res_f2                = 19,
                                                            .nof_cyc_shifts            = nof_cyclic_shifts::twelve,
                                                            .occ_supported             = false,
                                                            .f1_nof_symbols            = 7,
                                                            .f1_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 2,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f1     = 39,
                                                            .nof_res_f2     = 19,
                                                            .nof_cyc_shifts = nof_cyclic_shifts::no_cyclic_shift,
                                                            .occ_supported  = true,
                                                            .f1_nof_symbols = 7,
                                                            .f1_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 2,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_25,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f1     = 39,
                                                            .nof_res_f2     = 19,
                                                            .nof_cyc_shifts = nof_cyclic_shifts::no_cyclic_shift,
                                                            .occ_supported  = true,
                                                            .f1_nof_symbols = 11,
                                                            .f1_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 1,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f1                = 137,
                                                            .nof_res_f2                = 25,
                                                            .nof_cyc_shifts            = nof_cyclic_shifts::twelve,
                                                            .occ_supported             = true,
                                                            .f1_nof_symbols            = 14,
                                                            .f1_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 1,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = false},
                                           pucch_gen_params{.nof_res_f1                = 36,
                                                            .nof_res_f2                = 27,
                                                            .nof_cyc_shifts            = nof_cyclic_shifts::three,
                                                            .occ_supported             = true,
                                                            .f1_nof_symbols            = 9,
                                                            .f1_intraslot_freq_hopping = false,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 7,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f1                = 34,
                                                            .nof_res_f2                = 10,
                                                            .nof_cyc_shifts            = nof_cyclic_shifts::two,
                                                            .occ_supported             = false,
                                                            .f1_nof_symbols            = 9,
                                                            .f1_intraslot_freq_hopping = false,
                                                            .f2_nof_symbols            = 1,
                                                            .max_nof_rbs               = 7,
                                                            .max_payload_bits          = 11,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_08,
                                                            .f2_intraslot_freq_hopping = false},
                                           pucch_gen_params{.nof_res_f1     = 15,
                                                            .nof_res_f2     = 10,
                                                            .nof_cyc_shifts = nof_cyclic_shifts::no_cyclic_shift,
                                                            .occ_supported  = false,
                                                            .f1_nof_symbols = 9,
                                                            .f1_intraslot_freq_hopping = false,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 7,
                                                            .max_payload_bits          = 11,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_08,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f0                = 15,
                                                            .nof_res_f2                = 10,
                                                            .f0_nof_symbols            = 2,
                                                            .f0_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 2,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_25,
                                                            .f2_intraslot_freq_hopping = false},
                                           pucch_gen_params{.nof_res_f0                = 39,
                                                            .nof_res_f2                = 19,
                                                            .f0_nof_symbols            = 2,
                                                            .f0_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 2,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f0                = 39,
                                                            .nof_res_f2                = 19,
                                                            .f0_nof_symbols            = 1,
                                                            .f0_intraslot_freq_hopping = false,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 1,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f0                = 9,
                                                            .nof_res_f2                = 19,
                                                            .f0_nof_symbols            = 2,
                                                            .f0_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 1,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f0                = 39,
                                                            .nof_res_f2                = 25,
                                                            .f0_nof_symbols            = 2,
                                                            .f0_intraslot_freq_hopping = true,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 1,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = false},
                                           pucch_gen_params{.nof_res_f0                = 36,
                                                            .nof_res_f2                = 27,
                                                            .f0_nof_symbols            = 1,
                                                            .f0_intraslot_freq_hopping = false,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 7,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_15,
                                                            .f2_intraslot_freq_hopping = true},
                                           pucch_gen_params{.nof_res_f1                = 34,
                                                            .nof_res_f2                = 10,
                                                            .f0_nof_symbols            = 1,
                                                            .f0_intraslot_freq_hopping = false,
                                                            .f2_nof_symbols            = 1,
                                                            .max_nof_rbs               = 7,
                                                            .max_payload_bits          = 11,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_08,
                                                            .f2_intraslot_freq_hopping = false},
                                           pucch_gen_params{.nof_res_f0                = 15,
                                                            .nof_res_f2                = 10,
                                                            .f0_nof_symbols            = 1,
                                                            .f0_intraslot_freq_hopping = false,
                                                            .f2_nof_symbols            = 2,
                                                            .max_nof_rbs               = 7,
                                                            .max_payload_bits          = 11,
                                                            .max_code_rate = srsran::max_pucch_code_rate::dot_08,
                                                            .f2_intraslot_freq_hopping = true}));

///////////////////

class test_ue_pucch_config_builder : public ::testing::TestWithParam<pucch_cfg_builder_params>
{
protected:
  test_ue_pucch_config_builder() :
    nof_f0_res_harq_per_ue(GetParam().nof_res_f0_harq),
    nof_f1_res_harq_per_ue(GetParam().nof_res_f1_harq),
    nof_f2_res_harq_per_ue(GetParam().nof_res_f2_harq),
    nof_harq_cfg_per_ue(GetParam().nof_harq_cfg),
    nof_sr_res_per_cell(GetParam().nof_res_sr),
    nof_csi_res_per_cell(GetParam().nof_res_csi),
    f0_params(pucch_f0_params{.nof_symbols = 2, .intraslot_freq_hopping = false}),
    f1_params(pucch_f1_params{.nof_cyc_shifts         = nof_cyclic_shifts::no_cyclic_shift,
                              .occ_supported          = false,
                              .nof_symbols            = 14,
                              .intraslot_freq_hopping = true}),
    f2_params(pucch_f2_params{.nof_symbols            = 2,
                              .max_nof_rbs            = 2,
                              .max_code_rate          = max_pucch_code_rate::dot_25,
                              .intraslot_freq_hopping = false}),
    serv_cell_cfg(test_helpers::create_default_sched_ue_creation_request().cfg.cells->front().serv_cell_cfg)
  {
  }

  bool verify_nof_res_and_idx(unsigned harq_cfg_idx, unsigned sr_idx, unsigned csi_idx) const
  {
    srsran_assert(harq_cfg_idx < nof_harq_cfg_per_ue,
                  "HARQ resources config index must be smaller than the number of HARQ resources configurations");
    srsran_assert(sr_idx < nof_sr_res_per_cell,
                  "SR resource config index must be smaller than the number of cell SR resources");
    srsran_assert(csi_idx < nof_csi_res_per_cell,
                  "CSI resource config index must be smaller than the number of cell CSI resources");

    const pucch_config& pucch_cfg          = serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();
    const unsigned      nof_sr_res_per_ue  = 1;
    const unsigned      nof_csi_res_per_ue = 1;

    const bool     has_f0_res                = nof_f0_res_harq_per_ue > 0;
    const unsigned nof_f0_f1_res_harq_per_ue = has_f0_res ? nof_f0_res_harq_per_ue : nof_f1_res_harq_per_ue;

    // Check the number of resources in the PUCCH resource list is correct.
    bool test_result = pucch_cfg.pucch_res_list.size() ==
                       (nof_f0_f1_res_harq_per_ue + nof_f2_res_harq_per_ue + nof_sr_res_per_ue + nof_csi_res_per_ue);

    // Check the number of PUCCH F1 resources in the PUCCH resource sets is correct.
    test_result = test_result && pucch_cfg.pucch_res_set[0].pucch_res_id_list.size() == nof_f0_f1_res_harq_per_ue;
    test_result = test_result && pucch_cfg.pucch_res_set[1].pucch_res_id_list.size() == nof_f2_res_harq_per_ue;

    // Helper to retrives a given PUCCH resource given its ID from the PUCCH resource list.
    auto get_pucch_resource_with_id = [&pucch_cfg](pucch_res_id_t res_id) {
      return std::find_if(pucch_cfg.pucch_res_list.begin(),
                          pucch_cfg.pucch_res_list.end(),
                          [res_id](const pucch_resource& res) { return res.res_id == res_id; });
    };

    // Check the PUCCH resources indexing in PUCCH resource Set 0.
    for (pucch_res_id_t res_idx : pucch_cfg.pucch_res_set[0].pucch_res_id_list) {
      const bool pucch_cell_res_id_test = res_idx.cell_res_id >= nof_f0_f1_res_harq_per_ue * harq_cfg_idx and
                                          res_idx.cell_res_id < nof_f0_f1_res_harq_per_ue * (harq_cfg_idx + 1);
      // The PUCCH resource ID for the ASN1 message for PUCCH F1 resources is expected to be from 0 to
      // nof_f0_f1_res_harq_per_ue for all UEs.
      const bool pucch_ue_res_id_test = res_idx.ue_res_id < nof_f0_f1_res_harq_per_ue;
      const bool pucch_ue_cell_res_id_test =
          res_idx.cell_res_id - nof_f0_f1_res_harq_per_ue * harq_cfg_idx == res_idx.ue_res_id;
      // Make sure the PUCCH resource ID in the set has a corresponding resource in the PUCCH resource list.
      auto* res_it = get_pucch_resource_with_id(res_idx);
      test_result  = test_result and pucch_cell_res_id_test and pucch_ue_res_id_test and pucch_ue_cell_res_id_test and
                    res_it != pucch_cfg.pucch_res_list.end();
    }
    // Check the PUCCH resources indexing in PUCCH resource Set 1.
    for (pucch_res_id_t res_idx : pucch_cfg.pucch_res_set[1].pucch_res_id_list) {
      const bool pucch_cell_res_id_test =
          res_idx.cell_res_id >= nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                     nof_f2_res_harq_per_ue * harq_cfg_idx and
          res_idx.cell_res_id < nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                    nof_f2_res_harq_per_ue * (harq_cfg_idx + 1);
      // The PUCCH resource ID for the ASN1 message for PUCCH F2 resources is expected to be from
      // (nof_f0_f1_res_harq_per_ue + 1) to (nof_f0_f1_res_harq_per_ue + 1 + nof_f2_res_harq_per_ue) for all UEs.
      const bool pucch_ue_res_id_test = res_idx.ue_res_id >= nof_f0_f1_res_harq_per_ue + 1 and
                                        res_idx.ue_res_id < nof_f0_f1_res_harq_per_ue + 1 + nof_f2_res_harq_per_ue;
      // Check if the PUCCH cell resourece ID is set correspondingly to the PUCCH UE resource ID.
      const bool pucch_ue_cell_res_id_test =
          res_idx.cell_res_id - (nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                 nof_f2_res_harq_per_ue * harq_cfg_idx) ==
          res_idx.ue_res_id - (nof_f0_f1_res_harq_per_ue + 1);
      // Make sure the PUCCH resource ID in the set has a corresponding resource in the PUCCH resource list.
      auto* res_it = get_pucch_resource_with_id(res_idx);
      test_result  = test_result and pucch_cell_res_id_test and pucch_ue_res_id_test and pucch_ue_cell_res_id_test and
                    res_it != pucch_cfg.pucch_res_list.end();
    }

    // Check the format correctness.
    const pucch_format format_pucch_set_0 = has_f0_res ? pucch_format::FORMAT_0 : pucch_format::FORMAT_1;
    for (const auto& res : pucch_cfg.pucch_res_list) {
      const pucch_format expected_format =
          res.res_id.cell_res_id < nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell
              ? format_pucch_set_0
              : srsran::pucch_format::FORMAT_2;
      test_result = test_result && expected_format == res.format;
    }

    // Check SR and related PUCCH resource.
    test_result                          = test_result and pucch_cfg.sr_res_list.size() == 1;
    const pucch_res_id_t sr_pucch_res_id = pucch_cfg.sr_res_list.front().pucch_res_id;
    // Check the PUCCH resouce ID for SR is correct.
    const bool sr_cell_res_id_test =
        sr_pucch_res_id.cell_res_id == (nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + sr_idx);
    const bool sr_ue_res_id_test = sr_pucch_res_id.ue_res_id == nof_f0_f1_res_harq_per_ue;
    test_result                  = test_result and sr_cell_res_id_test and sr_ue_res_id_test;
    // Check the PUCCH resouce for SR present in the list.
    auto* res_it = get_pucch_resource_with_id(pucch_cfg.sr_res_list.front().pucch_res_id);
    test_result  = test_result and res_it != pucch_cfg.pucch_res_list.end();

    // Check SR and related PUCCH resource.
    srsran_assert(serv_cell_cfg.csi_meas_cfg.has_value(), "CSI meas config is expected to be present");
    auto& csi_cfg = serv_cell_cfg.csi_meas_cfg.value();
    srsran_assert(serv_cell_cfg.csi_meas_cfg.has_value(), "CSI meas config is expected to be present");
    srsran_assert(not csi_cfg.csi_report_cfg_list.empty() and
                      std::holds_alternative<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                          csi_cfg.csi_report_cfg_list.front().report_cfg_type) and
                      not std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                              csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                              .pucch_csi_res_list.empty(),
                  "PUCCH-CSI-ResourceList has not been configured in the CSI-reportConfig");

    const pucch_res_id_t csi_res_id = std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(
                                          csi_cfg.csi_report_cfg_list.front().report_cfg_type)
                                          .pucch_csi_res_list.front()
                                          .pucch_res_id;

    // Check the PUCCH resouce ID for CSI is correct.
    const bool csi_cell_res_id_test =
        csi_res_id.cell_res_id == nof_f0_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell +
                                      nof_f2_res_harq_per_ue * nof_harq_cfg_per_ue + csi_idx;
    const bool csi_ue_res_id_test = csi_res_id.ue_res_id == nof_f0_f1_res_harq_per_ue + 1 + nof_f2_res_harq_per_ue;
    test_result                   = test_result and csi_cell_res_id_test and csi_ue_res_id_test;
    // Check the PUCCH resouce for SR present in the list.
    res_it      = get_pucch_resource_with_id(csi_res_id);
    test_result = test_result and res_it != pucch_cfg.pucch_res_list.end();

    return test_result;
  }

  // Parameters that are passed by the routing to run the tests.
  const unsigned        bwp_size{106};
  const unsigned        nof_f0_res_harq_per_ue;
  const unsigned        nof_f1_res_harq_per_ue;
  const unsigned        nof_f2_res_harq_per_ue;
  const unsigned        nof_harq_cfg_per_ue;
  const unsigned        nof_sr_res_per_cell;
  const unsigned        nof_csi_res_per_cell;
  const unsigned        nof_symbols_per_slot{14};
  const pucch_f0_params f0_params;
  const pucch_f1_params f1_params;
  const pucch_f2_params f2_params;
  serving_cell_config   serv_cell_cfg;
};

TEST_P(test_ue_pucch_config_builder, test_validator_too_many_resources)
{
  // Generate the cell list of resources with many resources.
  const unsigned              nof_f0_res = nof_f0_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell;
  const unsigned              nof_f1_res = nof_f1_res_harq_per_ue * nof_harq_cfg_per_ue + nof_sr_res_per_cell;
  const unsigned              nof_f2_res = nof_f2_res_harq_per_ue * nof_harq_cfg_per_ue + nof_csi_res_per_cell;
  std::vector<pucch_resource> res_list;

  ASSERT_FALSE(nof_f0_res_harq_per_ue != 0 and nof_f1_res_harq_per_ue != 0)
      << "PUCCH Format 0 and Format 1 resources cannot be used together";

  if (nof_f0_res_harq_per_ue != 0) {
    res_list = generate_cell_pucch_res_list(nof_f0_res, nof_f2_res, f0_params, f2_params, bwp_size);
  } else {
    res_list = generate_cell_pucch_res_list(nof_f1_res, nof_f2_res, f1_params, f2_params, bwp_size);
  }

  const unsigned harq_idx_cfg = test_rgen::uniform_int<unsigned>(0, nof_harq_cfg_per_ue - 1);
  const unsigned sr_idx_cfg   = test_rgen::uniform_int<unsigned>(0, nof_sr_res_per_cell - 1);
  const unsigned csi_idx_cfg  = test_rgen::uniform_int<unsigned>(0, nof_csi_res_per_cell - 1);

  // Update pucch_cfg with the UE list of resources (with at max 8 HARQ F1, 8 HARQ F2, 4 SR).
  ue_pucch_config_builder(serv_cell_cfg,
                          res_list,
                          harq_idx_cfg,
                          sr_idx_cfg,
                          csi_idx_cfg,
                          nof_f0_res_harq_per_ue != 0 ? nof_f0_res_harq_per_ue : nof_f1_res_harq_per_ue,
                          nof_f2_res_harq_per_ue,
                          nof_harq_cfg_per_ue,
                          nof_sr_res_per_cell,
                          nof_csi_res_per_cell);

  ASSERT_TRUE(verify_nof_res_and_idx(harq_idx_cfg, sr_idx_cfg, csi_idx_cfg));
}

INSTANTIATE_TEST_SUITE_P(ue_pucch_config_builder,
                         test_ue_pucch_config_builder,
                         // clang-format off
                         //                                   nof:  f0  |  f1  |  f2  | harq | sr | csi
                         //                                   nof:  f0  |  f1  |  f2  | cfg  | sr | csi
                         ::testing::Values(
                                           pucch_cfg_builder_params{ 0,     3,     6,     1,    2,   1 },
                                           pucch_cfg_builder_params{ 0,     7,     3,     1,    1,   1 },
                                           pucch_cfg_builder_params{ 0,     8,     8,     1,    4,   1 },
                                           pucch_cfg_builder_params{ 0,     1,     1,     1,    1,   1 },
                                           pucch_cfg_builder_params{ 0,     7,     7,     1,    3,   1 },
                                           pucch_cfg_builder_params{ 0,     8,     8,     4,    4,   4 },
                                           pucch_cfg_builder_params{ 0,     5,     2,    10,    2,   7 },
                                           pucch_cfg_builder_params{ 0,     2,     7,     3,    7,   3 },
                                           pucch_cfg_builder_params{ 0,     6,     4,     5,    6,   2 },
                                           pucch_cfg_builder_params{ 3,     0,     6,     1,    2,   1 },
                                           pucch_cfg_builder_params{ 7,     0,     3,     1,    1,   1 },
                                           pucch_cfg_builder_params{ 8,     0,     8,     1,    4,   1 },
                                           pucch_cfg_builder_params{ 1,     0,     1,     1,    1,   1 },
                                           pucch_cfg_builder_params{ 7,     0,     7,     1,    3,   1 },
                                           pucch_cfg_builder_params{ 8,     0,     8,     4,    4,   4 },
                                           pucch_cfg_builder_params{ 5,     0,     2,    10,    2,   7 },
                                           pucch_cfg_builder_params{ 2,     0,     7,     3,    7,   3 },
                                           pucch_cfg_builder_params{ 6,     0,     4,     5,    6,   2 }
)
                         // clang-format on
);
