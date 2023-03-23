/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/du_manager/ran_resource_management/pucch_resource_generator.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;

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
        for (unsigned rb_idx = res.starting_prb; rb_idx < res_f2.nof_prbs; ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx; sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols / 2;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
        // Second hop.
        for (unsigned rb_idx = res.second_hop_prb.value(); rb_idx < res_f2.nof_prbs; ++rb_idx) {
          for (unsigned sym_idx = res_f2.starting_sym_idx + res_f2.nof_symbols / 2;
               sym_idx < res_f2.starting_sym_idx + res_f2.nof_symbols;
               ++sym_idx) {
            auto& grid_elem        = grid[sym_idx + nof_symbols * rb_idx];
            grid_elem.element_used = true;
            grid_elem.format       = pucch_format::FORMAT_2;
          }
        }
      } else {
        for (unsigned rb_idx = res.starting_prb; rb_idx < res_f2.nof_prbs; ++rb_idx) {
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

  bool verify_collision(const cell_pucch_resource& res)
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
        for (unsigned rb_idx = res.second_hop_prb.value(); rb_idx < res.starting_prb + res_f2.nof_prbs; ++rb_idx) {
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
  static const unsigned max_nof_occ    = 7;
  static const unsigned max_nof_cs     = 12;
  static const unsigned max_nof_occ_cs = max_nof_occ * max_nof_cs;
  struct grid_element {
    bool                             element_used;
    pucch_format                     format;
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

// TEST_F(test_pucch_res_generator, asd)
//{
//   pucch_params_option2                                params_opt2{.nof_res_f1 = 10, .nof_res_f2 = 12};
//   variant<pucch_params_option1, pucch_params_option2> params;
//   params.emplace<pucch_params_option2>(params_opt2);
//   std::vector<cell_pucch_resource> res_list = generate_pucch_resource_list(params, 106);
//
//   ASSERT_TRUE(res_list.size() > 0);
//   ASSERT_EQ(params_opt2.nof_res_f1 + params_opt2.nof_res_f2, res_list.size());
//
//   printf("New test\n");
//   for (const auto& pucch_res : res_list) {
//     printf("REs Id. %d ", pucch_res.cell_res_id);
//     if (pucch_res.format == srsran::pucch_format::FORMAT_1) {
//       const auto& f1 = variant_get<pucch_format_1_cfg>(pucch_res.format_params);
//       printf("Format 1 - PRBs [%d, %d), Symbols [%d, %d), CS: %d, OCC: %d \n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + 1,
//              f1.starting_sym_idx,
//              f1.starting_sym_idx + f1.nof_symbols,
//              f1.initial_cyclic_shift,
//              f1.time_domain_occ);
//     } else if (pucch_res.format == srsran::pucch_format::FORMAT_2) {
//       const auto& f2 = variant_get<pucch_format_2_3_cfg>(pucch_res.format_params);
//       printf("Format 2 - PRBs [%d, %d), Symbols [%d, %d)\n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + f2.nof_prbs,
//              f2.starting_sym_idx,
//              f2.starting_sym_idx + f2.nof_symbols);
//     }
//
//     ASSERT_FALSE(grid.verify_collision(pucch_res));
//     grid.add_resource(pucch_res);
//   }
// }
//
// TEST_F(test_pucch_res_generator, asd1)
//{
//   pucch_params_option2 params_opt2{
//       .nof_res_f1 = 10, .nof_res_f2 = 26, .max_f2_code_rate = srsran::max_pucch_code_rate::dot_15};
//   variant<pucch_params_option1, pucch_params_option2> params;
//   params.emplace<pucch_params_option2>(params_opt2);
//   std::vector<cell_pucch_resource> res_list = generate_pucch_resource_list(params, 106);
//
//   ASSERT_TRUE(res_list.size() > 0);
//   ASSERT_EQ(params_opt2.nof_res_f1 + params_opt2.nof_res_f2, res_list.size());
//
//   printf("New test\n");
//   for (const auto& pucch_res : res_list) {
//     printf("REs Id. %d ", pucch_res.cell_res_id);
//     if (pucch_res.format == srsran::pucch_format::FORMAT_1) {
//       const auto& f1 = variant_get<pucch_format_1_cfg>(pucch_res.format_params);
//       printf("Format 1 - PRBs [%d, %d), Symbols [%d, %d), CS: %d, OCC: %d \n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + 1,
//              f1.starting_sym_idx,
//              f1.starting_sym_idx + f1.nof_symbols,
//              f1.initial_cyclic_shift,
//              f1.time_domain_occ);
//     } else if (pucch_res.format == srsran::pucch_format::FORMAT_2) {
//       const auto& f2 = variant_get<pucch_format_2_3_cfg>(pucch_res.format_params);
//       printf("Format 2 - PRBs [%d, %d), Symbols [%d, %d)\n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + f2.nof_prbs,
//              f2.starting_sym_idx,
//              f2.starting_sym_idx + f2.nof_symbols);
//     }
//
//     ASSERT_FALSE(grid.verify_collision(pucch_res));
//     grid.add_resource(pucch_res);
//   }
// }

// TEST_F(test_pucch_res_generator, asd2)
//{
//   pucch_params_option2 params_opt2{
//       .nof_res_f1 = 10, .nof_res_f2 = 26, .nof_f1_symbols = 5, .max_f2_code_rate =
//       srsran::max_pucch_code_rate::dot_15};
//   variant<pucch_params_option1, pucch_params_option2> params;
//   params.emplace<pucch_params_option2>(params_opt2);
//   std::vector<cell_pucch_resource> res_list = generate_pucch_resource_list(params, 106);
//
//   ASSERT_TRUE(res_list.size() > 0);
//   ASSERT_EQ(params_opt2.nof_res_f1 + params_opt2.nof_res_f2, res_list.size());
//
//   printf("New test\n");
//   for (const auto& pucch_res : res_list) {
//     printf("REs Id. %d ", pucch_res.cell_res_id);
//     if (pucch_res.format == srsran::pucch_format::FORMAT_1) {
//       const auto& f1 = variant_get<pucch_format_1_cfg>(pucch_res.format_params);
//       printf("Format 1 - PRBs [%d, %d), Symbols [%d, %d), CS: %d, OCC: %d \n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + 1,
//              f1.starting_sym_idx,
//              f1.starting_sym_idx + f1.nof_symbols,
//              f1.initial_cyclic_shift,
//              f1.time_domain_occ);
//     } else if (pucch_res.format == srsran::pucch_format::FORMAT_2) {
//       const auto& f2 = variant_get<pucch_format_2_3_cfg>(pucch_res.format_params);
//       printf("Format 2 - PRBs [%d, %d), Symbols [%d, %d)\n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + f2.nof_prbs,
//              f2.starting_sym_idx,
//              f2.starting_sym_idx + f2.nof_symbols);
//     }
//
//     ASSERT_FALSE(grid.verify_collision(pucch_res));
//     grid.add_resource(pucch_res);
//   }
// }

// TEST_F(test_pucch_res_generator, asd3)
//{
//   pucch_params_option2 params_opt2{
//       .nof_res_f1 = 25, .nof_res_f2 = 10, .nof_f1_symbols = 5, .max_f2_code_rate =
//       srsran::max_pucch_code_rate::dot_15};
//   variant<pucch_params_option1, pucch_params_option2> params;
//   params.emplace<pucch_params_option2>(params_opt2);
//   std::vector<cell_pucch_resource> res_list = generate_pucch_resource_list(params, 106);
//
//   ASSERT_TRUE(res_list.size() > 0);
//   ASSERT_EQ(params_opt2.nof_res_f1 + params_opt2.nof_res_f2, res_list.size());
//
//   printf("New test\n");
//   for (const auto& pucch_res : res_list) {
//     printf("REs Id. %d ", pucch_res.cell_res_id);
//     if (pucch_res.format == srsran::pucch_format::FORMAT_1) {
//       const auto& f1 = variant_get<pucch_format_1_cfg>(pucch_res.format_params);
//       printf("Format 1 - PRBs [%d, %d), Symbols [%d, %d), CS: %d, OCC: %d \n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + 1,
//              f1.starting_sym_idx,
//              f1.starting_sym_idx + f1.nof_symbols,
//              f1.initial_cyclic_shift,
//              f1.time_domain_occ);
//     } else if (pucch_res.format == srsran::pucch_format::FORMAT_2) {
//       const auto& f2 = variant_get<pucch_format_2_3_cfg>(pucch_res.format_params);
//       printf("Format 2 - PRBs [%d, %d), Symbols [%d, %d)\n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + f2.nof_prbs,
//              f2.starting_sym_idx,
//              f2.starting_sym_idx + f2.nof_symbols);
//     }
//
//     ASSERT_FALSE(grid.verify_collision(pucch_res));
//     grid.add_resource(pucch_res);
//   }
// }

// TEST_F(test_pucch_res_generator, asd3)
//{
//   pucch_params_option2 params_opt2{.nof_res_f1       = 100,
//                                    .nof_res_f2       = 10,
//                                    .format1_cp       = srsran::srs_du::nof_cyclic_shifts::twelve,
//                                    .f1_occ_supported = true,
//                                    .nof_f1_symbols   = 7,
//                                    .max_f2_code_rate = srsran::max_pucch_code_rate::dot_15};
//
//   variant<pucch_params_option1, pucch_params_option2> params;
//   params.emplace<pucch_params_option2>(params_opt2);
//   std::vector<cell_pucch_resource> res_list = generate_pucch_resource_list(params, 106);
//
//   ASSERT_TRUE(res_list.size() > 0);
//   ASSERT_EQ(params_opt2.nof_res_f1 + params_opt2.nof_res_f2, res_list.size());
//
//   printf("New test\n");
//   for (const auto& pucch_res : res_list) {
//     printf("REs Id. %d ", pucch_res.cell_res_id);
//     if (pucch_res.format == srsran::pucch_format::FORMAT_1) {
//       const auto& f1 = variant_get<pucch_format_1_cfg>(pucch_res.format_params);
//       printf("Format 1 - PRBs [%d, %d), Symbols [%d, %d), CS: %d, OCC: %d \n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + 1,
//              f1.starting_sym_idx,
//              f1.starting_sym_idx + f1.nof_symbols,
//              f1.initial_cyclic_shift,
//              f1.time_domain_occ);
//     } else if (pucch_res.format == srsran::pucch_format::FORMAT_2) {
//       const auto& f2 = variant_get<pucch_format_2_3_cfg>(pucch_res.format_params);
//       printf("Format 2 - PRBs [%d, %d), Symbols [%d, %d)\n",
//              pucch_res.starting_prb,
//              pucch_res.starting_prb + f2.nof_prbs,
//              f2.starting_sym_idx,
//              f2.starting_sym_idx + f2.nof_symbols);
//     }
//
//     ASSERT_FALSE(grid.verify_collision(pucch_res));
//     grid.add_resource(pucch_res);
//   }
// }

TEST_F(test_pucch_res_generator, asd5)
{
  pucch_params_option1 params_opt1{.max_pucch_f1_rbs          = 5,
                                   .max_pucch_f2_rbs          = 8,
                                   .format1_cp                = srsran::srs_du::nof_cyclic_shifts::no_cyclic_shift,
                                   .f1_occ_supported          = false,
                                   .nof_f1_symbols            = 7,
                                   .f1_intraslot_freq_hopping = false,
                                   .max_f2_payload_bits       = 8,
                                   .max_f2_code_rate          = srsran::max_pucch_code_rate::dot_15,
                                   .f2_intraslot_freq_hopping = true};

  variant<pucch_params_option1, pucch_params_option2> params;
  params.emplace<pucch_params_option1>(params_opt1);
  const std::vector<cell_pucch_resource> res_list = generate_pucch_resource_list(params, bwp_size);

  ASSERT_FALSE(res_list.empty());

  printf("New test\n");
  for (const auto& pucch_res : res_list) {
    printf("REs Id. %d ", pucch_res.cell_res_id);
    if (pucch_res.format == srsran::pucch_format::FORMAT_1) {
      const auto& f1 = variant_get<pucch_format_1_cfg>(pucch_res.format_params);
      if (pucch_res.second_hop_prb.has_value()) {
        printf("Format 1 - PRBs [%d, %d), Symbols [%d, %d), 2nd-PRBs [%d, %d), 2nd-Symbols [%d, %d) CS: %d, OCC: %d \n",
               pucch_res.starting_prb,
               pucch_res.starting_prb + 1,
               f1.starting_sym_idx,
               f1.starting_sym_idx + f1.nof_symbols / 2,
               pucch_res.second_hop_prb.value(),
               pucch_res.second_hop_prb.value() + 1,
               f1.starting_sym_idx + f1.nof_symbols / 2,
               f1.starting_sym_idx + f1.nof_symbols,
               f1.initial_cyclic_shift,
               f1.time_domain_occ);
      } else {
        printf("Format 1 - PRBs [%d, %d), Symbols [%d, %d), CS: %d, OCC: %d \n",
               pucch_res.starting_prb,
               pucch_res.starting_prb + 1,
               f1.starting_sym_idx,
               f1.starting_sym_idx + f1.nof_symbols,
               f1.initial_cyclic_shift,
               f1.time_domain_occ);
      }
    } else if (pucch_res.format == srsran::pucch_format::FORMAT_2) {
      const auto& f2 = variant_get<pucch_format_2_3_cfg>(pucch_res.format_params);
      if (pucch_res.second_hop_prb.has_value()) {
        printf("Format 2 - PRBs [%d, %d), Symbols [%d, %d), 2nd-PRBs [%d, %d), 2nd-Symbols [%d, %d) \n",
               pucch_res.starting_prb,
               pucch_res.starting_prb + f2.nof_prbs,
               f2.starting_sym_idx,
               f2.starting_sym_idx + f2.nof_symbols / 2,
               pucch_res.second_hop_prb.value(),
               pucch_res.second_hop_prb.value() + f2.nof_prbs,
               f2.starting_sym_idx + f2.nof_symbols / 2,
               f2.starting_sym_idx + f2.nof_symbols);
      } else {
        printf("Format 2 - PRBs [%d, %d), Symbols [%d, %d)\n",
               pucch_res.starting_prb,
               pucch_res.starting_prb + f2.nof_prbs,
               f2.starting_sym_idx,
               f2.starting_sym_idx + f2.nof_symbols);
      }
    }

    ASSERT_FALSE(grid.verify_collision(pucch_res));
    grid.add_resource(pucch_res);
  }
}
