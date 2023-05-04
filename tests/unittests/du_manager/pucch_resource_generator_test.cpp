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

#include "lib/du_manager/ran_resource_management/pucch_resource_generator.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

namespace du_pucch_gen {
// Test struct.
struct pucch_gen_params_opt1 {
  unsigned                         nof_res_f1;
  unsigned                         nof_res_f2;
  nof_cyclic_shifts                nof_cyc_shifts{nof_cyclic_shifts::no_cyclic_shift};
  bool                             occ_supported{false};
  bounded_integer<unsigned, 4, 14> f1_nof_symbols{14};
  bool                             f1_intraslot_freq_hopping{false};
  bounded_integer<unsigned, 1, 2>  f2_nof_symbols{1};
  unsigned                         max_nof_rbs{1};
  optional<unsigned>               max_payload_bits;
  max_pucch_code_rate              max_code_rate{max_pucch_code_rate::dot_25};
  bool                             f2_intraslot_freq_hopping{false};
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const pucch_gen_params_opt1& value, ::std::ostream* os)
{
  return;
}

// Test struct.
struct pucch_gen_params_opt2 {
  unsigned                         max_nof_rbs_f1;
  unsigned                         max_nof_rbs_f2;
  nof_cyclic_shifts                nof_cyc_shifts{nof_cyclic_shifts::no_cyclic_shift};
  bool                             occ_supported{false};
  bounded_integer<unsigned, 4, 14> f1_nof_symbols{14};
  bool                             f1_intraslot_freq_hopping{false};
  bounded_integer<unsigned, 1, 2>  f2_nof_symbols{1};
  unsigned                         max_nof_rbs{1};
  optional<unsigned>               max_payload_bits;
  max_pucch_code_rate              max_code_rate{max_pucch_code_rate::dot_25};
  bool                             f2_intraslot_freq_hopping{false};
};

// Dummy function overload of template <typename T> void testing::internal::PrintTo(const T& value, ::std::ostream* os).
// This prevents valgrind from complaining about uninitialized variables.
void PrintTo(const pucch_gen_params_opt2& value, ::std::ostream* os)
{
  return;
}

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
  void add_resource(const cell_pucch_resource& res)
  {
    if (res.format == pucch_format::FORMAT_1) {
      srsran_assert(variant_holds_alternative<pucch_format_1_cfg>(res.format_params), "Expected PUCCH Format 1");
      const auto& res_f1 = variant_get<pucch_format_1_cfg>(res.format_params);

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
      srsran_assert(variant_holds_alternative<pucch_format_2_3_cfg>(res.format_params), "Expected PUCCH Format 2");
      const auto& res_f2 = variant_get<pucch_format_2_3_cfg>(res.format_params);

      if (res.second_hop_prb.has_value()) {
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
  bool verify_collision(const cell_pucch_resource& res) const
  {
    if (res.format == pucch_format::FORMAT_1) {
      srsran_assert(variant_holds_alternative<pucch_format_1_cfg>(res.format_params), "Expected PUCCH Format 1");
      const auto& res_f1 = variant_get<pucch_format_1_cfg>(res.format_params);
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
      srsran_assert(variant_holds_alternative<pucch_format_2_3_cfg>(res.format_params), "Expected PUCCH Format 2");
      const auto& res_f2 = variant_get<pucch_format_2_3_cfg>(res.format_params);

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

  // Compute the number of RBs used for PUCCH resources.
  unsigned get_tot_nof_used_rbs() const
  {
    unsigned used_rbs_cnt = 0;
    for (unsigned rb_idx = 0; rb_idx < nof_rbs; ++rb_idx) {
      for (unsigned sym_idx = 0; sym_idx < nof_symbols; ++sym_idx) {
        if (grid[sym_idx + nof_symbols * rb_idx].element_used) {
          ++used_rbs_cnt;
          break;
        }
      }
    }

    return used_rbs_cnt;
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

TEST_F(test_pucch_res_generator, test_validator_too_many_resources)
{
  // Request an insane amount of resources.
  const unsigned  nof_res_f1 = 10000;
  const unsigned  nof_res_f2 = 10000;
  pucch_f1_params params_f1{.nof_cyc_shifts         = srsran::srs_du::nof_cyclic_shifts::three,
                            .occ_supported          = false,
                            .nof_symbols            = 7,
                            .intraslot_freq_hopping = true};

  pucch_f2_params params_f2{.nof_symbols            = 2,
                            .max_nof_rbs            = 2,
                            .max_code_rate          = srsran::max_pucch_code_rate::dot_25,
                            .intraslot_freq_hopping = false};

  std::vector<cell_pucch_resource> res_list =
      generate_pucch_res_list_given_number(nof_res_f1, nof_res_f2, params_f1, params_f2, bwp_size);

  // Due to the many resources requested, the function cannot accommodate the request and returns an empty list.
  ASSERT_TRUE(res_list.size() == 0);
}

TEST_F(test_pucch_res_generator, test_validator_f2_1_symbol_with_freq_hop)
{
  const unsigned  nof_res_f1 = 10;
  const unsigned  nof_res_f2 = 1;
  pucch_f1_params params_f1{.nof_cyc_shifts         = srsran::srs_du::nof_cyclic_shifts::three,
                            .occ_supported          = false,
                            .nof_symbols            = 7,
                            .intraslot_freq_hopping = true};

  pucch_f2_params params_f2{.nof_symbols            = 1,
                            .max_nof_rbs            = 2,
                            .max_code_rate          = srsran::max_pucch_code_rate::dot_25,
                            .intraslot_freq_hopping = true};

  std::vector<cell_pucch_resource> res_list =
      generate_pucch_res_list_given_number(nof_res_f1, nof_res_f2, params_f1, params_f2, bwp_size);

  // Due to requested intraslot_freq_hopping with 1 symbol, the function returns an empty list.
  ASSERT_TRUE(res_list.size() == 0);
}

class test_pucch_res_generator_params : public ::testing::TestWithParam<pucch_gen_params_opt1>
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
  const unsigned  nof_res_f1 = GetParam().nof_res_f1;
  const unsigned  nof_res_f2 = GetParam().nof_res_f2;
  pucch_f1_params params_f1{.nof_cyc_shifts         = GetParam().nof_cyc_shifts,
                            .occ_supported          = GetParam().occ_supported,
                            .nof_symbols            = GetParam().f1_nof_symbols,
                            .intraslot_freq_hopping = GetParam().f1_intraslot_freq_hopping};

  pucch_f2_params params_f2{.nof_symbols            = GetParam().f2_nof_symbols,
                            .max_nof_rbs            = GetParam().max_nof_rbs,
                            .max_payload_bits       = GetParam().max_payload_bits,
                            .max_code_rate          = GetParam().max_code_rate,
                            .intraslot_freq_hopping = GetParam().f2_intraslot_freq_hopping};

  std::vector<cell_pucch_resource> res_list =
      generate_pucch_res_list_given_number(nof_res_f1, nof_res_f2, params_f1, params_f2, bwp_size);

  ASSERT_TRUE(res_list.size() > 0);
  ASSERT_EQ(nof_res_f1 + nof_res_f2, res_list.size());

  for (const auto& pucch_res : res_list) {
    ASSERT_FALSE(grid.verify_collision(pucch_res));
    grid.add_resource(pucch_res);
  }
}

INSTANTIATE_TEST_SUITE_P(
    test_res_generation_given_number,
    test_pucch_res_generator_params,
    ::testing::Values(pucch_gen_params_opt1{.nof_res_f1                = 15,
                                            .nof_res_f2                = 10,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::twelve,
                                            .occ_supported             = false,
                                            .f1_nof_symbols            = 7,
                                            .f1_intraslot_freq_hopping = true,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 2,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                            .f2_intraslot_freq_hopping = false},
                      pucch_gen_params_opt1{.nof_res_f1                = 39,
                                            .nof_res_f2                = 19,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::twelve,
                                            .occ_supported             = false,
                                            .f1_nof_symbols            = 7,
                                            .f1_intraslot_freq_hopping = true,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 2,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                            .f2_intraslot_freq_hopping = true},
                      pucch_gen_params_opt1{.nof_res_f1     = 39,
                                            .nof_res_f2     = 19,
                                            .nof_cyc_shifts = srsran::srs_du::nof_cyclic_shifts::no_cyclic_shift,
                                            .occ_supported  = true,
                                            .f1_nof_symbols = 7,
                                            .f1_intraslot_freq_hopping = true,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 2,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                            .f2_intraslot_freq_hopping = true},
                      pucch_gen_params_opt1{.nof_res_f1     = 39,
                                            .nof_res_f2     = 19,
                                            .nof_cyc_shifts = srsran::srs_du::nof_cyclic_shifts::no_cyclic_shift,
                                            .occ_supported  = true,
                                            .f1_nof_symbols = 11,
                                            .f1_intraslot_freq_hopping = true,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 1,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                            .f2_intraslot_freq_hopping = true},
                      pucch_gen_params_opt1{.nof_res_f1                = 137,
                                            .nof_res_f2                = 25,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::twelve,
                                            .occ_supported             = true,
                                            .f1_nof_symbols            = 14,
                                            .f1_intraslot_freq_hopping = true,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 1,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                            .f2_intraslot_freq_hopping = false},
                      pucch_gen_params_opt1{.nof_res_f1                = 36,
                                            .nof_res_f2                = 27,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::three,
                                            .occ_supported             = true,
                                            .f1_nof_symbols            = 9,
                                            .f1_intraslot_freq_hopping = false,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 7,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                            .f2_intraslot_freq_hopping = true},
                      pucch_gen_params_opt1{.nof_res_f1                = 34,
                                            .nof_res_f2                = 10,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::two,
                                            .occ_supported             = false,
                                            .f1_nof_symbols            = 9,
                                            .f1_intraslot_freq_hopping = false,
                                            .f2_nof_symbols            = 1,
                                            .max_nof_rbs               = 7,
                                            .max_payload_bits          = 11,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                            .f2_intraslot_freq_hopping = false},
                      pucch_gen_params_opt1{.nof_res_f1     = 15,
                                            .nof_res_f2     = 10,
                                            .nof_cyc_shifts = srsran::srs_du::nof_cyclic_shifts::no_cyclic_shift,
                                            .occ_supported  = false,
                                            .f1_nof_symbols = 9,
                                            .f1_intraslot_freq_hopping = false,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 7,
                                            .max_payload_bits          = 11,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                            .f2_intraslot_freq_hopping = true}));

class test_pucch_res_generator_max_nof_rbs : public ::testing::TestWithParam<pucch_gen_params_opt2>
{
public:
  test_pucch_res_generator_max_nof_rbs() : grid(bwp_size, nof_symbols_per_slot){};

protected:
  // Parameters that are passed by the routing to run the tests.
  const unsigned bwp_size{106};
  const unsigned nof_symbols_per_slot{14};
  pucch_grid     grid;
};

TEST_P(test_pucch_res_generator_max_nof_rbs, test_pucch_res_given_max_rbs)
{
  const unsigned  max_nof_rbs_f1 = GetParam().max_nof_rbs_f1;
  const unsigned  max_nof_rbs_f2 = GetParam().max_nof_rbs_f2;
  pucch_f1_params params_f1{.nof_cyc_shifts         = GetParam().nof_cyc_shifts,
                            .occ_supported          = GetParam().occ_supported,
                            .nof_symbols            = GetParam().f1_nof_symbols,
                            .intraslot_freq_hopping = GetParam().f1_intraslot_freq_hopping};

  pucch_f2_params params_f2{.nof_symbols            = GetParam().f2_nof_symbols,
                            .max_nof_rbs            = GetParam().max_nof_rbs,
                            .max_payload_bits       = GetParam().max_payload_bits,
                            .max_code_rate          = GetParam().max_code_rate,
                            .intraslot_freq_hopping = GetParam().f2_intraslot_freq_hopping};

  std::vector<cell_pucch_resource> res_list =
      generate_pucch_res_list_given_rbs(max_nof_rbs_f1, max_nof_rbs_f2, params_f1, params_f2, bwp_size);

  ASSERT_TRUE(res_list.size() > 0);

  // Allocate the resources on the test grid and verify they do not collide.
  for (const auto& pucch_res : res_list) {
    ASSERT_FALSE(grid.verify_collision(pucch_res));
    grid.add_resource(pucch_res);
  }

  // Verify the RBs allocated to PUCCH resources do not exceed threshold given in the input.
  ASSERT_TRUE(grid.get_tot_nof_used_rbs() <= max_nof_rbs_f1 + max_nof_rbs_f2);
  ASSERT_TRUE(grid.get_tot_nof_used_rbs() > 0);
}

INSTANTIATE_TEST_SUITE_P(
    test_res_generation_given_rbs,
    test_pucch_res_generator_max_nof_rbs,
    ::testing::Values(pucch_gen_params_opt2{.max_nof_rbs_f1            = 5,
                                            .max_nof_rbs_f2            = 10,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::twelve,
                                            .occ_supported             = false,
                                            .f1_nof_symbols            = 7,
                                            .f1_intraslot_freq_hopping = true,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 2,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                            .f2_intraslot_freq_hopping = false},
                      pucch_gen_params_opt2{.max_nof_rbs_f1            = 12,
                                            .max_nof_rbs_f2            = 7,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::twelve,
                                            .occ_supported             = true,
                                            .f1_nof_symbols            = 14,
                                            .f1_intraslot_freq_hopping = true,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 1,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                            .f2_intraslot_freq_hopping = false},
                      pucch_gen_params_opt2{.max_nof_rbs_f1            = 5,
                                            .max_nof_rbs_f2            = 16,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::three,
                                            .occ_supported             = true,
                                            .f1_nof_symbols            = 9,
                                            .f1_intraslot_freq_hopping = false,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 7,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_15,
                                            .f2_intraslot_freq_hopping = true},
                      pucch_gen_params_opt2{.max_nof_rbs_f1            = 7,
                                            .max_nof_rbs_f2            = 15,
                                            .nof_cyc_shifts            = srsran::srs_du::nof_cyclic_shifts::two,
                                            .occ_supported             = false,
                                            .f1_nof_symbols            = 9,
                                            .f1_intraslot_freq_hopping = false,
                                            .f2_nof_symbols            = 1,
                                            .max_nof_rbs               = 7,
                                            .max_payload_bits          = 11,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_08,
                                            .f2_intraslot_freq_hopping = false},
                      pucch_gen_params_opt2{.max_nof_rbs_f1 = 5,
                                            .max_nof_rbs_f2 = 10,
                                            .nof_cyc_shifts = srsran::srs_du::nof_cyclic_shifts::no_cyclic_shift,
                                            .occ_supported  = false,
                                            .f1_nof_symbols = 9,
                                            .f1_intraslot_freq_hopping = false,
                                            .f2_nof_symbols            = 2,
                                            .max_nof_rbs               = 7,
                                            .max_payload_bits          = 11,
                                            .max_code_rate             = srsran::max_pucch_code_rate::dot_25,
                                            .f2_intraslot_freq_hopping = true}));
