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
  if (results.params.filter_index == filter_index_type::ul_prach_preamble_1p25khz) {
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
  // NOTE: RU notifies the PRACH in slot 1, but MAC asked for that in slot 0.
  slot_point slot(uplane_results.params.slot.numerology(),
                  uplane_results.params.slot.sfn(),
                  uplane_results.params.slot.subframe_index(),
                  0);

  ul_prach_context prach_context = prach_repo.get(slot);
  if (prach_context.empty()) {
    return;
  }

  srsran_assert(prach_context.context.nof_fd_occasions == 1, "Only supporting one frequency domain occasion");
  srsran_assert(prach_context.context.nof_td_occasions == 1, "Only supporting one time domain occasion");

  span<cf_t> buffer = prach_context.buffer->get_symbol(prach_context.context.port,
                                                       prach_context.context.nof_fd_occasions - 1,
                                                       prach_context.context.nof_td_occasions - 1,
                                                       uplane_results.params.symbol_id);

  // Note assuming all PRBs in 1 packet.
  const uplane_section_params& sect_params        = uplane_results.sections.front();
  unsigned                     prach_length_in_re = (is_long_preamble(prach_context.context.format))
                                                        ? prach_constants::LONG_SEQUENCE_LENGTH
                                                        : prach_constants::SHORT_SEQUENCE_LENGTH;

  srsran_assert(is_long_preamble(prach_context.context.format), "SHORT PRACH format not supported");

  const prach_preamble_information& prem_info = get_prach_preamble_long_info(prach_context.context.format);
  unsigned nof_re_to_prach_data = prach_frequency_mapping_get(prem_info.scs, prach_context.context.pusch_scs).k_bar;

  if (sect_params.nof_prbs * NOF_SUBCARRIERS_PER_RB < nof_re_to_prach_data + prach_length_in_re) {
    logger.error("PRACH message segmentation not supported");

    return;
  }

  // Grab the data.
  span<const cf_t> prach_data =
      span<const cf_t>(sect_params.iq_samples).subspan(nof_re_to_prach_data, prach_length_in_re);

  std::copy(prach_data.begin(), prach_data.end(), buffer.begin());

  notifier.on_new_prach_window_data(prach_context.context, *(prach_context.buffer));
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
