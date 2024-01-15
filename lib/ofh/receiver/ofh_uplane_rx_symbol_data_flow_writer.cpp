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

#include "ofh_uplane_rx_symbol_data_flow_writer.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder_properties.h"

using namespace srsran;
using namespace ofh;

void uplane_rx_symbol_data_flow_writer::write_to_resource_grid(unsigned                              eaxc,
                                                               const uplane_message_decoder_results& results)
{
  slot_point     slot       = results.params.slot;
  unsigned       symbol     = results.params.symbol_id;
  uplink_context ul_context = ul_context_repo.get(slot, symbol);
  if (ul_context.empty()) {
    logger.warning("Dropped received Open Fronthaul message as no uplink slot context was found for slot '{}', symbol "
                   "'{}' and eAxC '{}'",
                   results.params.slot,
                   results.params.symbol_id,
                   eaxc);

    return;
  }

  // Find resource grid port with eAxC.
  unsigned rg_port = std::distance(ul_eaxc.begin(), std::find(ul_eaxc.begin(), ul_eaxc.end(), eaxc));
  srsran_assert(rg_port < ul_eaxc.size(), "Invalid resource grid port={}", rg_port);

  unsigned du_ul_nof_prbs = ul_context.get_grid_nof_prbs();
  for (const auto& sect : results.sections) {
    // Section PRBs are above the last PRB of the DU. Do not copy.
    if (sect.start_prb >= du_ul_nof_prbs) {
      continue;
    }

    // By default, try to copy all the expected PRBs.
    unsigned nof_prbs_to_write = du_ul_nof_prbs - sect.start_prb;

    // Section contains less PRBs than the grid. Copy the whole section.
    if (sect.start_prb + sect.nof_prbs < du_ul_nof_prbs) {
      nof_prbs_to_write = sect.nof_prbs;
    }

    ul_context_repo.write_grid(
        slot,
        rg_port,
        symbol,
        sect.start_prb * NOF_SUBCARRIERS_PER_RB,
        span<const cf_t>(sect.iq_samples)
            .subspan(sect.start_prb * NOF_SUBCARRIERS_PER_RB, nof_prbs_to_write * NOF_SUBCARRIERS_PER_RB));

    logger.debug("Written IQ data into UL resource grid PRB range [{},{}), for slot '{}', symbol '{}' and port '{}'",
                 sect.start_prb,
                 sect.start_prb + nof_prbs_to_write,
                 slot,
                 symbol,
                 rg_port);
  }
}
