/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ofh_uplane_prach_symbol_data_flow_writer.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder_properties.h"

using namespace srsran;
using namespace ofh;

void uplane_prach_symbol_data_flow_writer::write_to_prach_buffer(unsigned                              eaxc,
                                                                 const uplane_message_decoder_results& results)
{
  slot_point slot = results.params.slot;

  prach_context prach_context = prach_context_repo.get(slot);
  if (prach_context.empty()) {
    logger.debug("Dropping Open Fronthaul message as no uplink PRACH context was found for slot={}",
                 slot,
                 results.params.symbol_id);
    return;
  }

  // Find resource grid port with eAxC.
  unsigned port = std::distance(prach_eaxc.begin(), std::find(prach_eaxc.begin(), prach_eaxc.end(), eaxc));
  if (port >= prach_context.get_max_nof_ports()) {
    logger.debug(
        "Skipping port {} as stored PRACH buffer supports up to {} ports", eaxc, prach_context.get_max_nof_ports());

    return;
  }

  unsigned prach_nof_res           = prach_context.get_prach_nof_re();
  unsigned nof_re_to_prach_data    = prach_context.get_prach_offset_to_first_re();
  unsigned prach_nof_prbs          = std::ceil(float(prach_nof_res + nof_re_to_prach_data) / NOF_SUBCARRIERS_PER_RB);
  unsigned nof_re_after_prach_data = prach_nof_prbs * NOF_SUBCARRIERS_PER_RB - (prach_nof_res + nof_re_to_prach_data);
  unsigned prach_data_start_prb    = nof_re_to_prach_data / NOF_SUBCARRIERS_PER_RB;

  for (const auto& section : results.sections) {
    // Section PRBs are above the last PRB of the PRACH. Do not copy.
    if (section.start_prb >= prach_nof_prbs) {
      continue;
    }

    // Section PRBs are below the first PRB of the PRACH. Do not copy.
    if (section.start_prb + section.nof_prbs <= prach_data_start_prb) {
      continue;
    }

    // By default, try to copy all the expected PRBs.
    unsigned nof_prbs_to_write = prach_nof_prbs - section.start_prb;

    // Section contains less PRBs than the PRACH buffer. Copy the whole section.
    if (section.start_prb + section.nof_prbs < prach_nof_prbs) {
      nof_prbs_to_write = section.nof_prbs;
    }

    // Set the start resource element for the PRACH buffer.
    unsigned start_re = std::max<int>(0, (section.start_prb * NOF_SUBCARRIERS_PER_RB - nof_re_to_prach_data));

    // Section RE start and number of REs.
    unsigned section_start_re = section.start_prb * NOF_SUBCARRIERS_PER_RB;
    unsigned section_nof_re   = nof_prbs_to_write * NOF_SUBCARRIERS_PER_RB;

    // Remove the unused REs from the last PRB.
    if (section.start_prb + section.nof_prbs >= prach_nof_prbs) {
      section_nof_re -= nof_re_after_prach_data;
    }

    // Start RE in the section IQ data.
    unsigned iq_start_re = 0U;
    // If the first RE of the section is smaller than the offset to the first RE that contains data, adjust the first RE
    // that has data in the IQ data.
    if (section_start_re < nof_re_to_prach_data) {
      iq_start_re = nof_re_to_prach_data - section_start_re;
      // Remove the offset REs from the number of REs of the section.
      section_nof_re -= iq_start_re;
    }

    // Get the number of REs to copy from the IQ data.
    unsigned iq_size_re = std::min(section_nof_re, prach_nof_res);

    // Grab the data.
    span<const cf_t> prach_in_data = span<const cf_t>(section.iq_samples).subspan(iq_start_re, iq_size_re);

    // Copy the data in the buffer.
    prach_context_repo.write_iq(slot, port, results.params.symbol_id, start_re, prach_in_data);

    logger.debug("Handling PRACH in slot {}: port={}, symbol={}", slot, port, results.params.symbol_id);
  }
}
