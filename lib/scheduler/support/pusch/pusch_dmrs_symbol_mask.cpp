/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_dmrs_symbol_mask.h"

using namespace srsgnb;

dmrs_symbol_mask srsgnb::pusch_dmrs_symbol_mask_mapping_type_A_single_get(
    const pusch_dmrs_symbol_mask_mapping_type_A_single_configuration& config)
{
  srsgnb_assert(config.typeA_pos == dmrs_typeA_position::pos2 || config.typeA_pos == dmrs_typeA_position::pos3,
                "Invalid TypeA position {}",
                config.typeA_pos);
  srsgnb_assert(config.duration > 0 && config.duration <= 14, "Invalid transmission duration {}", config.duration);
  unsigned l0 = static_cast<unsigned>(config.typeA_pos);

  dmrs_symbol_mask mask(14);
  mask.set(l0);

  if (config.duration < 8 || config.additional_position == dmrs_additional_positions::pos0) {
    return mask;
  }

  if (config.duration < 10) {
    mask.set(7);
    return mask;
  }

  if (config.duration < 13 && (config.duration != 12 || config.additional_position < dmrs_additional_positions::pos3)) {
    mask.set(9);
    if (config.additional_position >= dmrs_additional_positions::pos2) {
      mask.set(6);
    }
    return mask;
  }

  mask.set(11);
  if (config.additional_position == dmrs_additional_positions::pos2) {
    mask.set(7);
  } else if (config.additional_position == dmrs_additional_positions::pos3) {
    mask.set(5);
    mask.set(8);
  }

  return mask;
}