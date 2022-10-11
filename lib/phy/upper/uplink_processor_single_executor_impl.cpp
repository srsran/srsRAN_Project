/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uplink_processor_single_executor_impl.h"
#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
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

uplink_processor_single_executor_impl::uplink_processor_single_executor_impl(
    std::unique_ptr<prach_detector>  detector_,
    std::unique_ptr<pusch_processor> pusch_proc_,
    std::unique_ptr<pucch_processor> pucch_proc_,
    task_executor&                   executor,
    upper_phy_rx_results_notifier&   results_notifier) :
  detector(std::move(detector_)),
  pusch_proc(std::move(pusch_proc_)),
  pucch_proc(std::move(pucch_proc_)),
  executor(executor),
  results_notifier(results_notifier)
{
  srsgnb_assert(detector, "A valid PRACH detector must be provided");
  srsgnb_assert(pusch_proc, "A valid PUSCH processor must be provided");
  srsgnb_assert(pucch_proc, "A valid PUCCH processor must be provided");
}

void uplink_processor_single_executor_impl::process_prach(const prach_buffer&         buffer,
                                                          const prach_buffer_context& context)
{
  executor.execute([&buffer, context, this]() {
    prach_detector::configuration configuration = get_prach_dectector_config_from_prach_context(context);

    // Data adaptation.
    ul_prach_results ul_results;
    ul_results.context = context;
    ul_results.result  = detector->detect(buffer, configuration);

    // Notify the PRACH results.
    results_notifier.on_new_prach_results(ul_results);
  });
}

void uplink_processor_single_executor_impl::process_pusch(span<uint8_t>                      data,
                                                          rx_softbuffer&                     softbuffer,
                                                          const resource_grid_reader&        grid,
                                                          const uplink_processor::pusch_pdu& pdu)
{
  executor.execute([data, &softbuffer, &grid, pdu, this]() {
    pusch_processor_result proc_result = pusch_proc->process(data, softbuffer, grid, pdu.pdu);

    ul_pusch_results result;
    result.slot = pdu.pdu.slot;
    result.csi  = proc_result.csi;

    if (proc_result.data.has_value()) {
      result.data.emplace();
      ul_pusch_results::pusch_data& results_data = result.data.value();
      results_data.harq_id                       = pdu.harq_id;
      results_data.rnti                          = to_rnti(pdu.pdu.rnti);
      results_data.decoder_result                = proc_result.data.value();
      results_data.payload                       = (proc_result.data.value().tb_crc_ok) ? data : span<uint8_t>();
    }

    // :TODO: Add the UCI to the notifier results when available.

    // Notify the PUSCH results.
    results_notifier.on_new_pusch_results(result);
  });
}

void uplink_processor_single_executor_impl::process_pucch(const resource_grid_reader&        grid,
                                                          const uplink_processor::pucch_pdu& pdu)
{
  executor.execute([&grid, pdu, this]() {
    srsgnb_assert(pdu.format_type == pucch_format::FORMAT_1, "Currently supporting PUCCH format 1 exclusively");

    pucch_processor_result proc_result;
    ul_pucch_results       result;
    // Process the PUCCH.
    switch (pdu.format_type) {
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
        srsgnb_assert(0, "Invalid PUCCH format={}", pdu.format_type);
    }

    // Write the results.
    result.rnti             = pdu.rnti;
    result.slot             = pdu.slot;
    result.processor_result = proc_result;

    // Notify the PUCCH results.
    results_notifier.on_new_pucch_results(result);
  });
}
