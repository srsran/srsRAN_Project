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
#include "srsran/instrumentation/traces/ofh_traces.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder_properties.h"

using namespace srsran;
using namespace ofh;

void uplane_rx_symbol_data_flow_writer::write_to_resource_grid(unsigned                              eaxc,
                                                               const uplane_message_decoder_results& results)
{
  trace_point access_repo_tp = ofh_tracer.now();

  slot_point     slot       = results.params.slot;
  unsigned       symbol     = results.params.symbol_id;
  uplink_context ul_context = ul_context_repo->get(slot, symbol);
  if (ul_context.empty()) {
    logger.warning("Dropped received Open Fronthaul message as no uplink slot context was found for slot '{}', symbol "
                   "'{}' and eAxC '{}'",
                   results.params.slot,
                   results.params.symbol_id,
                   eaxc);

    return;
  }
  ofh_tracer << trace_event("ofh_receiver_access_repo", access_repo_tp);

  // Find resource grid port with eAxC.
  unsigned rg_port = std::distance(ul_eaxc.begin(), std::find(ul_eaxc.begin(), ul_eaxc.end(), eaxc));
  srsran_assert(rg_port < ul_eaxc.size(), "Invalid resource grid port value '{}'", rg_port);

  // The DU cell bandwidth may be narrower than the operating bandwidth of the RU.
  unsigned du_nof_prbs = ul_context.get_grid_nof_prbs();
  for (const auto& section : results.sections) {
    // Drop the whole section when all PRBs are outside the range of the DU bandwidth and the operating bandwidth of the
    // RU is larger.
    if (section.start_prb >= du_nof_prbs) {
      continue;
    }

    // At this point, we have to care about the following cases:
    //   a) The last PRB of the section falls outside the range of the DU cell bandwidth.
    //   b) The last PRB of the section falls inside the range of the DU cell bandwidth.

    // Take care of case (a), takes the first N PRBs inside the section.
    unsigned nof_prbs_to_write = du_nof_prbs - section.start_prb;
    // Take care of case (b), takes all the PRBs inside the section.
    if (section.start_prb + section.nof_prbs < du_nof_prbs) {
      nof_prbs_to_write = section.nof_prbs;
    }

    trace_point write_rg_tp = ofh_tracer.now();

    ul_context_repo->write_grid(
        slot,
        rg_port,
        symbol,
        section.start_prb * NOF_SUBCARRIERS_PER_RB,
        span<const cbf16_t>(section.iq_samples).first(nof_prbs_to_write * NOF_SUBCARRIERS_PER_RB));

    ofh_tracer << trace_event("ofh_receiver_write_rg", write_rg_tp);

    logger.debug("Written IQ data into UL resource grid PRB range [{},{}), for slot '{}', symbol '{}' and port '{}'",
                 section.start_prb,
                 section.start_prb + nof_prbs_to_write,
                 slot,
                 symbol,
                 rg_port);
  }
}
