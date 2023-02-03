/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uplink_processor_impl.h"
#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/upper/unique_rx_softbuffer.h"
#include "srsgnb/phy/upper/upper_phy_rx_results_notifier.h"
#include "srsgnb/support/executors/task_executor.h"

using namespace srsgnb;

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

  return config;
}

uplink_processor_impl::uplink_processor_impl(std::unique_ptr<prach_detector>  prach_,
                                             std::unique_ptr<pusch_processor> pusch_proc_,
                                             std::unique_ptr<pucch_processor> pucch_proc_) :
  prach(std::move(prach_)), pusch_proc(std::move(pusch_proc_)), pucch_proc(std::move(pucch_proc_))
{
  srsgnb_assert(prach, "A valid PRACH detector must be provided");
  srsgnb_assert(pusch_proc, "A valid PUSCH processor must be provided");
  srsgnb_assert(pucch_proc, "A valid PUCCH processor must be provided");
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
  pusch_processor_result proc_result = pusch_proc->process(data, std::move(softbuffer), grid, pdu.pdu);

  ul_pusch_results result;
  result.slot = pdu.pdu.slot;
  result.csi  = proc_result.csi;
  result.rnti = to_rnti(pdu.pdu.rnti);

  if (proc_result.data.has_value()) {
    ul_pusch_results::pusch_data& results_data = result.data.emplace();

    results_data.harq_id        = pdu.harq_id;
    results_data.decoder_result = *proc_result.data;
    results_data.payload        = (proc_result.data->tb_crc_ok) ? data : span<const uint8_t>();
  }

  // UCI HARQ information.
  if (!proc_result.harq_ack.payload.empty()) {
    ul_pusch_results::pusch_uci& results_uci = result.uci.emplace();

    results_uci.harq_ack.emplace(proc_result.harq_ack);
  }

  // UCI CSI1 information.
  if (!proc_result.csi_part1.payload.empty()) {
    ul_pusch_results::pusch_uci& results_uci = result.uci.has_value() ? result.uci.value() : result.uci.emplace();

    results_uci.csi1.emplace(proc_result.csi_part1);
  }

  // UCI CSI2 information.
  if (!proc_result.csi_part2.payload.empty()) {
    ul_pusch_results::pusch_uci& results_uci = result.uci.has_value() ? result.uci.value() : result.uci.emplace();

    results_uci.csi2.emplace(proc_result.csi_part2);
  }

  // Notify the PUSCH results.
  notifier.on_new_pusch_results(result);
}

void uplink_processor_impl::process_pucch(upper_phy_rx_results_notifier&     notifier,
                                          const resource_grid_reader&        grid,
                                          const uplink_processor::pucch_pdu& pdu)
{
  srsgnb_assert(pdu.context.format == pucch_format::FORMAT_1, "Currently supporting PUCCH format 1 exclusively");

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
      srsgnb_assert(0, "Invalid PUCCH format={}", pdu.context.format);
  }

  // Write the results.
  ul_pucch_results result;
  result.context          = pdu.context;
  result.processor_result = proc_result;

  // Notify the PUCCH results.
  notifier.on_new_pucch_results(result);
}
