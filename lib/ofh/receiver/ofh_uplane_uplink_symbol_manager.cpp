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

#include "ofh_uplane_uplink_symbol_manager.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/ran/prach/prach_constants.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/resource_block.h"
#include "srsran/support/format_utils.h"

using namespace srsran;
using namespace ofh;

uplane_uplink_symbol_manager::uplane_uplink_symbol_manager(const uplane_uplink_symbol_manager_config& config) :
  logger(config.logger),
  ul_eaxc(config.ul_eaxc),
  prach_eaxc(config.prach_eaxc),
  notifier(config.notifier),
  packet_handler(config.packet_handler),
  prach_repo(config.prach_repo),
  ul_slot_repo(config.ul_slot_repo)
{
}

void uplane_uplink_symbol_manager::on_new_frame(span<const uint8_t> payload)
{
  expected<message_decoder_results> decoding_results = packet_handler.decode_packet(payload);

  // Do nothing on decoding error.
  if (decoding_results.is_error()) {
    return;
  }

  const uplane_message_decoder_results& results = decoding_results.value().uplane_results;

  // Copy the PRBs into the PRACH buffer.
  if (is_a_prach_message(results.params.filter_index)) {
    handle_prach_prbs(decoding_results.value());

    return;
  }

  // Copy the PRBs into the resource grid.
  if (results.params.filter_index == filter_index_type::standard_channel_filter) {
    handle_grid_prbs(decoding_results.value());

    return;
  }
}

void uplane_uplink_symbol_manager::handle_prach_prbs(const message_decoder_results& results)
{
  const uplane_message_decoder_results& uplane_results = results.uplane_results;
  slot_point                            slot           = uplane_results.params.slot;

  ul_prach_context prach_context = prach_repo.get(slot);
  if (prach_context.empty()) {
    logger.debug("Dropping Open Fronthaul message as no uplink PRACH context was found for slot={}, symbol={}",
                 slot,
                 uplane_results.params.symbol_id);
    return;
  }
  const uplane_section_params& sect_params = uplane_results.sections.front();

  if (sect_params.nof_prbs * NOF_SUBCARRIERS_PER_RB < prach_context.get_prach_nof_re()) {
    logger.error("PRACH message segmentation not supported");

    return;
  }

  // Find resource grid port with eAxC.
  unsigned port = std::distance(prach_eaxc.begin(), std::find(prach_eaxc.begin(), prach_eaxc.end(), results.eaxc));
  logger.debug("Handling PRACH in slot {}: port={}, symbol={}", slot, port, uplane_results.params.symbol_id);

  bool notified = prach_repo.update_buffer_and_notify(
      slot, port, uplane_results.params.symbol_id, sect_params.iq_samples, notifier);
  if (notified) {
    logger.debug("Finished PRACH reception in slot {}", slot);
  }
}

void uplane_uplink_symbol_manager::handle_grid_prbs(const message_decoder_results& results)
{
  const uplane_message_decoder_results& uplane_results  = results.uplane_results;
  const slot_point                      slot            = uplane_results.params.slot;
  ul_slot_context                       ul_data_context = ul_slot_repo.get(slot);
  if (ul_data_context.empty()) {
    logger.info("Dropping Open Fronthaul message as no uplink slot context was found for slot={}, symbol={}",
                uplane_results.params.slot,
                uplane_results.params.symbol_id);

    return;
  }

  // Find resource grid port with eAxC.
  unsigned rg_port = std::distance(ul_eaxc.begin(), std::find(ul_eaxc.begin(), ul_eaxc.end(), results.eaxc));

  const unsigned du_ul_nof_prbs = ul_data_context.get_grid_nof_prbs();
  const unsigned symbol         = uplane_results.params.symbol_id;
  for (const auto& sect : uplane_results.sections) {
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

    srsran_assert(rg_port < ul_eaxc.size(), "Invalid resource grid port={}", rg_port);

    ul_slot_repo.update_grid_and_notify(
        slot,
        rg_port,
        symbol,
        sect.start_prb * NOF_SUBCARRIERS_PER_RB,
        span<const cf_t>(sect.iq_samples)
            .subspan(sect.start_prb * NOF_SUBCARRIERS_PER_RB, nof_prbs_to_write * NOF_SUBCARRIERS_PER_RB),
        notifier);
  }
}
