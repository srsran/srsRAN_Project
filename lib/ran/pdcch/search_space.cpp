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

#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/pdcch/aggregation_level.h"
#include "srsran/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsran/ran/pdcch/pdcch_type0_css_occasions.h"

using namespace srsran;

search_space_configuration::search_space_configuration(nr_band            band,
                                                       subcarrier_spacing common_scs,
                                                       subcarrier_spacing ssb_scs,
                                                       unsigned           coreset0_index,
                                                       unsigned           search_space0_index) :
  id(to_search_space_id(0)), cs_id(to_coreset_id(0)), dci_fmt(common_dci_format{.f0_0_and_f1_0 = true})
{
  const pdcch_type0_css_coreset_description cset0_desc =
      pdcch_type0_css_coreset_get(band, ssb_scs, common_scs, coreset0_index, 0);
  srsran_assert(cset0_desc.pattern != PDCCH_TYPE0_CSS_CORESET_RESERVED.pattern, "Invalid CORESET#0 index value");
  const pdcch_type0_css_occasion_pattern1_description ss0_occasion = pdcch_type0_css_occasions_get_pattern1(
      pdcch_type0_css_occasion_pattern1_configuration{.is_fr2           = false,
                                                      .ss_zero_index    = static_cast<uint8_t>(search_space0_index),
                                                      .nof_symb_coreset = cset0_desc.nof_symb_coreset});

  // TS 38.213, Table 10.1-1 - CCE aggregation levels and maximum number of PDCCH candidates per CCE aggregation level
  // for CSS sets configured by searchSpace-SIB1.
  const unsigned cset0_nof_cces =
      cset0_desc.nof_rb_coreset * cset0_desc.nof_symb_coreset / pdcch_constants::NOF_REG_PER_CCE;
  nof_candidates = {0, 0, 4, 2, 1};
  for (unsigned lidx = 0; lidx != NOF_AGGREGATION_LEVELS; ++lidx) {
    nof_candidates[lidx] = to_nof_cces(aggregation_index_to_level(lidx)) > cset0_nof_cces ? 0U : nof_candidates[lidx];
  }

  // NOTE: Currently, we support only SS/PBCH and CORESET multiplexing pattern 1 where the periodicity of
  // SearchSpace#0 is 20 ms.
  // TODO: Revise the below assignment when other multiplexing patterns are supported.
  monitoring_slot_periodicity = get_nof_slots_per_subframe(common_scs) * 20;

  // NOTE: Currently, we support only SS/PBCH and CORESET multiplexing pattern 1 where the duration of SearchSpace#0
  // is 2 consecutive slots.
  // TODO: Revise the below assignment when other multiplexing patterns are supported.
  duration = 2;

  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ++ssb_idx) {
    monitoring_slot_offset.push_back(srsran::get_type0_pdcch_css_n0(
        static_cast<unsigned>(ss0_occasion.offset), ss0_occasion.M, common_scs, ssb_idx));
  }

  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ++ssb_idx) {
    search_space_configuration::monitoring_symbols_within_slot_t symbols_within_slot(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP);
    symbols_within_slot.set(ss0_occasion.start_symbol[ssb_idx % ss0_occasion.start_symbol.size()], true);
    monitoring_symbols_within_slot.push_back(symbols_within_slot);
  }
}

search_space_configuration::search_space_configuration(
    search_space_id                                    id_,
    coreset_id                                         cs_id_,
    std::array<uint8_t, 5>                             nof_candidates_,
    variant<common_dci_format, ue_specific_dci_format> dci_fmt_,
    unsigned                                           monitoring_slot_periodicity_,
    unsigned                                           monitoring_slot_offset_,
    subcarrier_spacing                                 scs_common,
    unsigned                                           duration_,
    monitoring_symbols_within_slot_t                   monitoring_symbols_within_slot_) :
  id(id_),
  cs_id(cs_id_),
  nof_candidates(nof_candidates_),
  dci_fmt(dci_fmt_),
  monitoring_slot_periodicity(monitoring_slot_periodicity_),
  duration(duration_)
{
  srsran_assert(not is_search_space0(), "Constructor cannot be used for SearchSpace#0");

  set_non_ss0_monitoring_slot_offset(monitoring_slot_offset_, scs_common);
  set_non_ss0_monitoring_symbols_within_slot(monitoring_symbols_within_slot_);
}

void search_space_configuration::set_non_ss0_monitoring_slot_offset(unsigned slot_offset, subcarrier_spacing scs_common)
{
  srsran_assert(not is_search_space0(), "Invalid access to monitoring slot offset of SearchSpace#0");
  const uint8_t numerology_mu = to_numerology_value(scs_common);
  monitoring_slot_offset.clear();
  monitoring_slot_offset.push_back({numerology_mu, slot_offset});
}

void search_space_configuration::set_non_ss0_monitoring_symbols_within_slot(
    monitoring_symbols_within_slot_t symbols_within_slot)
{
  srsran_assert(not is_search_space0(), "Invalid access to monitoring symbols within slot of SearchSpace#0");
  srsran_assert(not symbols_within_slot.none(),
                "None of the symbols are set in monitoring symbols within a slot when configuring SearchSpace#{}",
                id);
  monitoring_symbols_within_slot.clear();
  monitoring_symbols_within_slot.push_back(symbols_within_slot);
}
