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

#pragma once

#include "../support/uplink_cplane_context_repository.h"
#include "srsran/ofh/serdes/ofh_message_decoder_properties.h"
#include "srsran/srslog/logger.h"
#include <algorithm>

namespace srsran {
namespace ofh {

/// Returns true if an OFDM symbol index is valid for the given uplink Control-Plane context.
constexpr bool is_symbol_index_in_cplane_valid(const ul_cplane_context& cp_context, unsigned start_symbol)
{
  return (start_symbol >= cp_context.start_symbol) &&
         (start_symbol < (cp_context.start_symbol + cp_context.nof_symbols));
}

/// Returns true if all of the Open Fronthaul User-Plane per section PRB fields are valid, otherwise returns false.
inline bool are_uplane_prb_fields_valid(const uplane_message_decoder_results& results,
                                        const ul_cplane_context&              cp_context,
                                        unsigned                              sector_id,
                                        srslog::basic_logger&                 logger)
{
  return std::none_of(
      results.sections.begin(),
      results.sections.end(),
      [cp_context, sector_id, &logger](const uplane_section_params& up_section) {
        if (SRSRAN_UNLIKELY(up_section.start_prb > MAX_NOF_PRBS - 1)) {
          logger.info(
              "Sector#{}: dropped received Open Fronthaul User-Plane packet as the first PRB index '{}' is not valid",
              sector_id,
              up_section.start_prb);

          return true;
        }

        if (SRSRAN_UNLIKELY(up_section.start_prb + up_section.nof_prbs > MAX_NOF_PRBS)) {
          logger.info(
              "Sector#{}: dropped received Open Fronthaul User-Plane packet as the last PRB index '{}' is not valid",
              sector_id,
              up_section.start_prb + up_section.nof_prbs);

          return true;
        }

        if (SRSRAN_UNLIKELY(!up_section.is_every_rb_used)) {
          logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet as 'every other resource block is "
                      "used' mode is not supported",
                      sector_id);

          return true;
        }

        if (SRSRAN_UNLIKELY(!up_section.use_current_symbol_number)) {
          logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet as 'increment the current symbol "
                      "number and use that' mode is not supported",
                      sector_id);

          return true;
        }

        bool is_up_section_not_found_in_cp_section =
            up_section.start_prb < cp_context.prb_start ||
            (up_section.start_prb + up_section.nof_prbs) > (cp_context.prb_start + cp_context.nof_prb);

        if (SRSRAN_UNLIKELY(is_up_section_not_found_in_cp_section)) {
          logger.info("Sector#{}: dropped received Open Fronthaul User-Plane packet as PRB index range '{}:{}' does "
                      "not match the expected range '{}:{}'",
                      sector_id,
                      up_section.start_prb,
                      up_section.nof_prbs,
                      cp_context.prb_start,
                      cp_context.nof_prb);
        }
        return is_up_section_not_found_in_cp_section;
      });
}

} // namespace ofh
} // namespace srsran
