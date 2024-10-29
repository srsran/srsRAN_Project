/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Helper functions for PUCCH Format 3 and PUCCH Format 4.

#pragma once

#include "srsran/adt/bounded_integer.h"
#include "srsran/phy/support/mask_types.h"

namespace srsran {

/// \brief DM-RS symbol mask for PUCCH Format 3 and PUCCH Format 4.
///
/// \param[in] nof_symbols       Number of symbols assigned to PUCCH resource.
/// \param[in] frequency_hopping True if intra-slot frequency hopping is enabled for the PUCCH resource, false
/// otherwise. \param[in] additional_dmrs   Whether \e additionalDMRS parameter is set for the PUCCH resource. \returns
/// The symbol mask for symbols containing DM-RS for that configuration, as per TS38.211 Section 6.4.1.3.3.2-1.
inline symbol_slot_mask get_pucch_formats_3_4_dmrs_symbol_mask(bounded_integer<unsigned, 4, 14> nof_symbols,
                                                               bool                             frequency_hopping,
                                                               bool                             additional_dmrs)
{
  symbol_slot_mask mask(nof_symbols.value());

  switch (nof_symbols.value()) {
    case 4:
      if (frequency_hopping) {
        mask.set(0);
        mask.set(2);
      } else {
        mask.set(1);
      }
      break;
    case 5:
      mask.set(0);
      mask.set(3);
      break;
    case 6:
    case 7:
      mask.set(1);
      mask.set(4);
      break;
    case 8:
      mask.set(1);
      mask.set(5);
      break;
    case 9:
      mask.set(1);
      mask.set(6);
      break;
    case 10:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(3);
        mask.set(6);
        mask.set(8);
      } else {
        mask.set(2);
        mask.set(7);
      }
      break;
    case 11:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(3);
        mask.set(6);
        mask.set(9);
      } else {
        mask.set(2);
        mask.set(7);
      }
      break;
    case 12:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(4);
        mask.set(7);
        mask.set(10);
      } else {
        mask.set(2);
        mask.set(8);
      }
      break;
    case 13:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(4);
        mask.set(7);
        mask.set(11);
      } else {
        mask.set(2);
        mask.set(9);
      }
      break;
    case 14:
      if (additional_dmrs) {
        mask.set(1);
        mask.set(5);
        mask.set(8);
        mask.set(12);
      } else {
        mask.set(3);
        mask.set(10);
      }
      break;
  }

  return mask;
}

} // namespace srsran
