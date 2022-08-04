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

uplink_processor_single_executor_impl::uplink_processor_single_executor_impl(
    std::unique_ptr<prach_detector> detector_,
    task_executor&                  executor,
    upper_phy_rx_results_notifier&  results_notifier) :
  detector(std::move(detector_)), executor(executor), results_notifier(results_notifier)
{
  srsgnb_assert(detector, "A valid PRACH detector must be provided");
}

void uplink_processor_single_executor_impl::process_prach(const prach_buffer&                       buffer,
                                                          const prach_buffer_context&               context,
                                                          const prach_detector::slot_configuration& configuration)
{
  // :TODO: not sure what data adaptatios goes here. Please review it when the PRACH_detector is updated, to work in
  // frequency domain.
  executor.execute([samples = buffer.get_symbol(context.start_symbol), configuration, this]() {
    const prach_detector::detection_result& result = detector->detect(samples, configuration);

    // Notify the PRACH results.
    results_notifier.on_new_prach_results(result);
  });
}
