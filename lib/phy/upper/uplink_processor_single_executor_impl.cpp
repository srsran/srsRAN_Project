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
    std::unique_ptr<prach_detector> detector_,
    task_executor&                  executor,
    upper_phy_rx_results_notifier&  results_notifier) :
  detector(std::move(detector_)), executor(executor), results_notifier(results_notifier)
{
  srsgnb_assert(detector, "A valid PRACH detector must be provided");
}

void uplink_processor_single_executor_impl::process_prach(const prach_buffer&         buffer,
                                                          const prach_buffer_context& context)
{
  // :TODO: not sure what data adaptation goes here. Please review it when the PRACH_detector is updated.
  executor.execute([&buffer, context, this]() {
    prach_detector::configuration           configuration = get_prach_dectector_config_from_prach_context(context);
    const prach_detector::detection_result& result        = detector->detect(buffer, configuration);

    // Notify the PRACH results.
    results_notifier.on_new_prach_results(result);
  });
}
