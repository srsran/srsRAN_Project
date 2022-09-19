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
    task_executor&                   executor,
    upper_phy_rx_results_notifier&   results_notifier) :
  detector(std::move(detector_)),
  pusch_proc(std::move(pusch_proc_)),
  executor(executor),
  results_notifier(results_notifier)
{
  srsgnb_assert(detector, "A valid PRACH detector must be provided");
  srsgnb_assert(pusch_proc, "A valid PUSCH processor must be provided");
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
      results_data.payload                       = data;
    }

    // :TODO: Add the UCI to the notifier results when available.

    // Notify the PUSCH results.
    results_notifier.on_new_pusch_results(result);
  });
}
