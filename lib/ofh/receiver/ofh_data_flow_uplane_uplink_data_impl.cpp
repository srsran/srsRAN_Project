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

#include "ofh_data_flow_uplane_uplink_data_impl.h"
#include "srsran/ofh/serdes/ofh_message_decoder_properties.h"

using namespace srsran;
using namespace ofh;

data_flow_uplane_uplink_data_impl::data_flow_uplane_uplink_data_impl(
    const data_flow_uplane_uplink_data_impl_config&  config,
    data_flow_uplane_uplink_data_impl_dependencies&& dependencies) :
  logger(*dependencies.logger),
  ul_cplane_context_repo_ptr(dependencies.ul_cplane_context_repo_ptr),
  ul_cplane_context_repo(*ul_cplane_context_repo_ptr),
  uplane_decoder(std::move(dependencies.uplane_decoder)),
  rx_symbol_writer(config.ul_eaxc, *dependencies.logger, dependencies.ul_context_repo),
  notification_sender(*dependencies.logger, dependencies.ul_context_repo, *dependencies.notifier)
{
  srsran_assert(ul_cplane_context_repo_ptr, "Invalid control plane repository");
  srsran_assert(uplane_decoder, "Invalid User-Plane decoder");
}

void data_flow_uplane_uplink_data_impl::decode_type1_message(unsigned eaxc, span<const uint8_t> message)
{
  uplane_message_decoder_results results;
  if (!uplane_decoder->decode(results, message)) {
    return;
  }

  if (should_uplane_packet_be_filtered(eaxc, results)) {
    return;
  }

  rx_symbol_writer.write_to_resource_grid(eaxc, results);
  notification_sender.notify_received_symbol(results.params.slot, results.params.symbol_id);
}

bool data_flow_uplane_uplink_data_impl::should_uplane_packet_be_filtered(
    unsigned                              eaxc,
    const uplane_message_decoder_results& results) const
{
  if (results.params.filter_index == filter_index_type::reserved || is_a_prach_message(results.params.filter_index)) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as decoded filter index={} for slot={}, symbol={}",
                 to_value(results.params.filter_index),
                 results.params.slot,
                 results.params.symbol_id);

    return true;
  }

  const uplane_message_params& params = results.params;
  expected<ul_cplane_context>  ex_cp_context =
      ul_cplane_context_repo.get(params.slot, params.symbol_id, params.filter_index, eaxc);

  if (!ex_cp_context) {
    logger.debug("Dropping Open Fronthaul User-Plane packet as no Control-Packet associated was found for slot={}, "
                 "symbol={}, eAxC={}",
                 params.slot,
                 params.symbol_id,
                 eaxc);

    return true;
  }

  // Check the PRBs.
  const ul_cplane_context& cp_context = ex_cp_context.value();

  return std::any_of(
      results.sections.begin(), results.sections.end(), [&cp_context, this](const uplane_section_params& up_section) {
        if (!up_section.is_every_rb_used) {
          logger.debug("Dropping Open Fronthaul User-Plane packet as 'every other resource block is used' mode is not "
                       "supported");

          return true;
        }

        if (!up_section.use_current_symbol_number) {
          logger.debug("Dropping Open Fronthaul User-Plane packet as 'increment the current symbol number and use "
                       "that' mode is not supported");

          return true;
        }

        bool is_up_section_not_found_in_cp_section =
            (up_section.start_prb < cp_context.prb_start ||
             (up_section.start_prb + up_section.nof_prbs) > (cp_context.prb_start + cp_context.nof_prb));

        if (is_up_section_not_found_in_cp_section) {
          logger.debug(
              "Dropping Open Fronthaul User-Plane packet as PRB indexes {}:{} does not match Control-Packet {}:{}.",
              up_section.start_prb,
              up_section.nof_prbs,
              cp_context.prb_start,
              cp_context.nof_prb);
        }

        return is_up_section_not_found_in_cp_section;
      });
}
