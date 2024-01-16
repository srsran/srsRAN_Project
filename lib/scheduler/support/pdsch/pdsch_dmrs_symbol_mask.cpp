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

#include "pdsch_dmrs_symbol_mask.h"

using namespace srsran;

dmrs_symbol_mask srsran::pdsch_dmrs_symbol_mask_mapping_type_A_single_get(
    const pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration& config)
{
  unsigned l0 = static_cast<unsigned>(config.typeA_pos);
  unsigned l1 =
      (config.lte_crs_match_around && (config.additional_position == dmrs_additional_positions::pos1 && l0 == 3) &&
       config.ue_capable_additional_dmrs_dl_alt)
          ? 12
          : 11;

  dmrs_symbol_mask mask(14);
  mask.set(l0);

  if (config.last_symbol < 8 || config.additional_position == dmrs_additional_positions::pos0) {
    return mask;
  }

  if (config.last_symbol < 10) {
    mask.set(7);
    return mask;
  }

  if (config.additional_position == dmrs_additional_positions::pos1) {
    if (config.last_symbol < 13) {
      mask.set(9);
      return mask;
    }
    mask.set(l1);
    return mask;
  }

  if (config.additional_position == dmrs_additional_positions::pos2) {
    if (config.last_symbol < 13) {
      mask.set(6);
      mask.set(9);
      return mask;
    }

    mask.set(7);
    mask.set(11);
    return mask;
  }

  if (config.last_symbol < 12) {
    mask.set(6);
    mask.set(9);
    return mask;
  }

  mask.set(5);
  mask.set(8);
  mask.set(11);

  return mask;
}