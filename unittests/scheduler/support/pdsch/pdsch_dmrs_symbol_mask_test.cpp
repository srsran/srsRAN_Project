/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/scheduler/support/pdsch/pdsch_dmrs_symbol_mask.h"
#include "srsgnb/support/srsgnb_test.h"
#include <map>

using namespace srsgnb;

#define L0 (UINT8_MAX)
#define L1 (UINT8_MAX + 1)

static const std::map<std::pair<unsigned, unsigned>, static_vector<unsigned, 4> >
    pdsch_dmrs_symbol_mask_mapping_type_A_single_table = {
        {{3, 0}, {L0}},  {{3, 1}, {L0}},      {{3, 2}, {L0}},         {{3, 3}, {L0}},
        {{4, 0}, {L0}},  {{4, 1}, {L0}},      {{4, 2}, {L0}},         {{4, 3}, {L0}},
        {{5, 0}, {L0}},  {{5, 1}, {L0}},      {{5, 2}, {L0}},         {{5, 3}, {L0}},
        {{6, 0}, {L0}},  {{6, 1}, {L0}},      {{6, 2}, {L0}},         {{6, 3}, {L0}},
        {{7, 0}, {L0}},  {{7, 1}, {L0}},      {{7, 2}, {L0}},         {{7, 3}, {L0}},
        {{8, 0}, {L0}},  {{8, 1}, {L0, 7}},   {{8, 2}, {L0, 7}},      {{8, 3}, {L0, 7}},
        {{9, 0}, {L0}},  {{9, 1}, {L0, 7}},   {{9, 2}, {L0, 7}},      {{9, 3}, {L0, 7}},
        {{10, 0}, {L0}}, {{10, 1}, {L0, 9}},  {{10, 2}, {L0, 6, 9}},  {{10, 3}, {L0, 6, 9}},
        {{11, 0}, {L0}}, {{11, 1}, {L0, 9}},  {{11, 2}, {L0, 6, 9}},  {{11, 3}, {L0, 6, 9}},
        {{12, 0}, {L0}}, {{12, 1}, {L0, 9}},  {{12, 2}, {L0, 6, 9}},  {{12, 3}, {L0, 5, 8, 11}},
        {{13, 0}, {L0}}, {{13, 1}, {L0, L1}}, {{13, 2}, {L0, 7, 11}}, {{13, 3}, {L0, 5, 8, 11}},
        {{14, 0}, {L0}}, {{14, 1}, {L0, L1}}, {{14, 2}, {L0, 7, 11}}, {{14, 3}, {L0, 5, 8, 11}},
};

static void pdsch_dmrs_symbol_mask_mapping_type_A_single_test()
{
  // Iterate through all possible values.
  for (unsigned typeA_pos : {2, 3}) {
    for (unsigned additional_position : {0, 1, 2, 3}) {
      for (unsigned duration = 3; duration != 15; ++duration) {
        for (bool lte_crs_match_around : {true, false}) {
          for (bool ue_capable_additional_dmrs_dl_alt : {true, false}) {
            // Prepare configuration and get the mask.
            pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration config;
            config.typeA_pos                         = typeA_pos;
            config.additional_position               = additional_position;
            config.duration                          = duration;
            config.lte_crs_match_around              = lte_crs_match_around;
            config.ue_capable_additional_dmrs_dl_alt = ue_capable_additional_dmrs_dl_alt;
            pdsch_dmrs_symbol_mask mask              = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(config);

            // Calculate expected mask.
            unsigned               l0 = typeA_pos;
            unsigned               l1 = (config.lte_crs_match_around && config.ue_capable_additional_dmrs_dl_alt &&
                           config.additional_position == 1 && l0 == 3)
                                            ? 12
                                            : 11;
            pdsch_dmrs_symbol_mask expected(14);
            TESTASSERT(pdsch_dmrs_symbol_mask_mapping_type_A_single_table.count({duration, additional_position}),
                       "Missing case for duration={} and additional_position={}.",
                       duration,
                       additional_position);
            for (unsigned symbol_index :
                 pdsch_dmrs_symbol_mask_mapping_type_A_single_table.at({duration, additional_position})) {
              if (symbol_index == L0) {
                symbol_index = l0;
              } else if (symbol_index == L1) {
                symbol_index = l1;
              }
              expected.set(symbol_index);
            }

            TESTASSERT_EQ(mask,
                          expected,
                          "typeA_pos={}; additional_position={}; duration={}; lte_crs_match_around={}; "
                          "ue_capable_additional_dmrs_dl_alt={}",
                          typeA_pos,
                          additional_position,
                          duration,
                          lte_crs_match_around,
                          ue_capable_additional_dmrs_dl_alt);
          }
        }
      }
    }
  }
}

int main()
{
  pdsch_dmrs_symbol_mask_mapping_type_A_single_test();

  return 0;
}