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

#include "ofh_data_flow_uplane_uplink_prach_impl.h"
#include "srsran/ofh/serdes/ofh_message_decoder_properties.h"

using namespace srsran;
using namespace ofh;

data_flow_uplane_uplink_prach_impl::data_flow_uplane_uplink_prach_impl(
    const data_flow_uplane_uplink_prach_impl_config&  config_,
    data_flow_uplane_uplink_prach_impl_dependencies&& dependencies) :
  logger(*dependencies.logger),
  is_prach_cplane_enabled(config_.is_prach_cplane_enabled),
  ul_cplane_context_repo_ptr(dependencies.ul_cplane_context_repo_ptr),
  ul_cplane_context_repo(*ul_cplane_context_repo_ptr),
  uplane_decoder(std::move(dependencies.uplane_decoder)),
  prach_iq_writter(config_.prach_eaxcs, *dependencies.logger, dependencies.prach_context_repo),
  notification_sender(*dependencies.logger, dependencies.prach_context_repo, dependencies.notifier)
{
  srsran_assert(ul_cplane_context_repo_ptr, "Invalid Control-Plane context repository");
  srsran_assert(uplane_decoder, "Invalid User-Plane decoder");
}

bool data_flow_uplane_uplink_prach_impl::should_uplane_packet_be_filtered(
    unsigned                              eaxc,
    const uplane_message_decoder_results& results) const
{
  if (!is_a_prach_message(results.params.filter_index)) {
    logger.info("Dropped received Open Fronthaul User-Plane packet for slot '{}' and symbol '{}' as decoded filter "
                "index value '{}' is not valid",
                results.params.slot,
                results.params.symbol_id,
                to_value(results.params.filter_index));

    return true;
  }

  // When Control-Plane message for PRACH is not configured, skip the check.
  if (!is_prach_cplane_enabled) {
    return false;
  }

  const uplane_message_params& params = results.params;
  expected<ul_cplane_context>  ex_cp_context =
      ul_cplane_context_repo.get(params.slot, params.symbol_id, params.filter_index, eaxc);

  if (!ex_cp_context) {
    logger.info(
        "Dropped received Open Fronthaul User-Plane PRACH packet as no data was expected for slot '{}', symbol '{}' "
        "and eAxC '{}'",
        params.slot,
        params.symbol_id,
        eaxc);

    return true;
  }

  // Check the PRBs.
  const ul_cplane_context& cp_context = ex_cp_context.value();
  return std::any_of(
      results.sections.begin(), results.sections.end(), [&cp_context, this](const uplane_section_params& up_section) {
        if (up_section.start_prb > MAX_NOF_PRBS - 1) {
          logger.info("Dropped received Open Fronthaul User-Plane packet as the first PRB index '{}' is not valid",
                      up_section.start_prb);

          return true;
        }

        if (up_section.start_prb + up_section.nof_prbs > MAX_NOF_PRBS) {
          logger.info("Dropped received Open Fronthaul User-Plane packet as the last PRB index '{}' is not valid",
                      up_section.start_prb + up_section.nof_prbs);

          return true;
        }

        if (!up_section.is_every_rb_used) {
          logger.info("Dropped received Open Fronthaul User-Plane packet as 'every other resource block is used' mode "
                      "is not supported");

          return true;
        }

        if (!up_section.use_current_symbol_number) {
          logger.info("Dropped received Open Fronthaul User-Plane packet as 'increment the current symbol number and "
                      "use that' mode is not supported");

          return true;
        }

        bool is_up_section_not_found_in_cp_section =
            up_section.start_prb < cp_context.prb_start ||
            (up_section.start_prb + up_section.nof_prbs) > (cp_context.prb_start + cp_context.nof_prb);

        if (is_up_section_not_found_in_cp_section) {
          logger.info("Dropped received Open Fronthaul User-Plane packet as PRB index range '{}:{}' does not match the "
                      "expected range '{}:{}'",
                      up_section.start_prb,
                      up_section.nof_prbs,
                      cp_context.prb_start,
                      cp_context.nof_prb);
        }
        return is_up_section_not_found_in_cp_section;
      });
}

void data_flow_uplane_uplink_prach_impl::decode_type1_message(unsigned eaxc, span<const uint8_t> message)
{
  uplane_message_decoder_results results;
  if (!uplane_decoder->decode(results, message)) {
    return;
  }

  if (should_uplane_packet_be_filtered(eaxc, results)) {
    return;
  }

  prach_iq_writter.write_to_prach_buffer(eaxc, results);
  notification_sender.notify_prach(results.params.slot);
}
