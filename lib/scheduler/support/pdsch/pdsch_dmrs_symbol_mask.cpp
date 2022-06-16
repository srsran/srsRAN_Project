/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/scheduler/support/pdsch/pdsch_dmrs_symbol_mask.h"

using namespace srsgnb;

pdsch_dmrs_symbol_mask srsgnb::pdsch_dmrs_symbol_mask_mapping_type_A_single_get(
    const pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration& config)
{
  srsran_assert(config.typeA_pos == 2 || config.typeA_pos == 3, "Invalid TypeA position {}.", config.typeA_pos);
  srsran_assert(config.duration > 2 && config.duration <= 14, "Invalid transmission duration {}.", config.duration);
  unsigned l0 = config.typeA_pos;
  unsigned l1 = (config.lte_crs_match_around && (config.additional_position == 1 && l0 == 3) &&
                 config.ue_capable_additional_dmrs_dl_alt)
                    ? 12
                    : 11;

  pdsch_dmrs_symbol_mask mask(14);
  mask.set(l0);

  if (config.duration < 8 || config.additional_position == 0) {
    return mask;
  }

  if (config.duration < 10) {
    mask.set(7);
    return mask;
  }

  if (config.additional_position == 1) {
    if (config.duration < 13) {
      mask.set(9);
      return mask;
    }
    mask.set(l1);
    return mask;
  }

  if (config.additional_position == 2) {
    if (config.duration < 13) {
      mask.set(6);
      mask.set(9);
      return mask;
    }

    mask.set(7);
    mask.set(11);
    return mask;
  }

  if (config.duration < 12) {
    mask.set(6);
    mask.set(9);
    return mask;
  }

  mask.set(5);
  mask.set(8);
  mask.set(11);

  return mask;
}