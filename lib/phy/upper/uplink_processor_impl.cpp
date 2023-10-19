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

#include "uplink_processor_impl.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"

using namespace srsran;

/// \brief Returns a PRACH detector slot configuration using the given PRACH buffer context.
static prach_detector::configuration get_prach_dectector_config_from_prach_context(const prach_buffer_context& context)
{
  prach_detector::configuration config;
  config.root_sequence_index   = context.root_sequence_index;
  config.format                = context.format;
  config.restricted_set        = context.restricted_set;
  config.zero_correlation_zone = context.zero_correlation_zone;
  config.start_preamble_index  = context.start_preamble_index;
  config.nof_preamble_indices  = context.nof_preamble_indices;
  config.ra_scs                = to_ra_subcarrier_spacing(context.pusch_scs);
  config.nof_rx_ports          = 1;

  return config;
}

uplink_processor_impl::uplink_processor_impl(std::unique_ptr<prach_detector>  prach_,
                                             std::unique_ptr<pusch_processor> pusch_proc_,
                                             std::unique_ptr<pucch_processor> pucch_proc_) :
  free_pusch_adaptors(max_nof_pusch_notifier_adaptors),
  prach(std::move(prach_)),
  pusch_proc(std::move(pusch_proc_)),
  pucch_proc(std::move(pucch_proc_)),
  logger(srslog::fetch_basic_logger("PHY", true))
{
  srsran_assert(prach, "A valid PRACH detector must be provided");
  srsran_assert(pusch_proc, "A valid PUSCH processor must be provided");
  srsran_assert(pucch_proc, "A valid PUCCH processor must be provided");

  for (unsigned i = 0; i != max_nof_pusch_notifier_adaptors; ++i) {
    pusch_adaptors.emplace_back(detail::pusch_processor_result_notifier_adaptor(free_pusch_adaptors));
  }
}

void uplink_processor_impl::process_prach(upper_phy_rx_results_notifier& notifier,
                                          const prach_buffer&            buffer,
                                          const prach_buffer_context&    context)
{
  ul_prach_results ul_results;
  ul_results.context = context;
  ul_results.result  = prach->detect(buffer, get_prach_dectector_config_from_prach_context(context));

  // Notify the PRACH results.
  notifier.on_new_prach_results(ul_results);
}

void uplink_processor_impl::process_pusch(span<uint8_t>                      data,
                                          unique_rx_softbuffer               softbuffer,
                                          upper_phy_rx_results_notifier&     notifier,
                                          const resource_grid_reader&        grid,
                                          const uplink_processor::pusch_pdu& pdu)
{
  // Pop an adaptor identifier.
  optional<unsigned> adaptor_id = free_pusch_adaptors.try_pop();
  if (!adaptor_id.has_value()) {
    logger.set_context(pdu.pdu.slot.sfn(), pdu.pdu.slot.slot_index());
    logger.warning("UL rnti={:#x} h_id={}: insufficient number of PUSCH notifier adaptor. Dropping PDU.",
                   pdu.pdu.rnti,
                   pdu.harq_id);
    return;
  }

  // Configure adaptor.
  pusch_processor_result_notifier& processor_notifier = pusch_adaptors[adaptor_id.value()].configure(
      notifier, to_rnti(pdu.pdu.rnti), pdu.pdu.slot, to_harq_id(pdu.harq_id), data);

  // Process PUSCH.
  pusch_proc->process(data, std::move(softbuffer), processor_notifier, grid, pdu.pdu);
}

void uplink_processor_impl::process_pucch(upper_phy_rx_results_notifier&     notifier,
                                          const resource_grid_reader&        grid,
                                          const uplink_processor::pucch_pdu& pdu)
{
  srsran_assert(pdu.context.format == pucch_format::FORMAT_1 || pdu.context.format == pucch_format::FORMAT_2,
                "Currently supporting PUCCH Format 1 and 2 only.");

  pucch_processor_result proc_result;
  // Process the PUCCH.
  switch (pdu.context.format) {
    case pucch_format::FORMAT_0:
      proc_result = pucch_proc->process(grid, pdu.format0);
      break;
    case pucch_format::FORMAT_1:
      proc_result = pucch_proc->process(grid, pdu.format1);
      break;
    case pucch_format::FORMAT_2:
      proc_result = pucch_proc->process(grid, pdu.format2);
      break;
    case pucch_format::FORMAT_3:
      proc_result = pucch_proc->process(grid, pdu.format3);
      break;
    case pucch_format::FORMAT_4:
      proc_result = pucch_proc->process(grid, pdu.format4);
      break;
    default:
      srsran_assert(0, "Invalid PUCCH format={}", pdu.context.format);
  }

  // Write the results.
  ul_pucch_results result;
  result.context          = pdu.context;
  result.processor_result = proc_result;

  // Notify the PUCCH results.
  notifier.on_new_pucch_results(result);
}
