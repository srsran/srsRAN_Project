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

#pragma once

#include "prach_processor_worker.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_baseband.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_notifier.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_request_handler.h"
#include <atomic>

namespace srsran {

/// \brief PRACH processor implementation.
///
/// Acts as a PRACH processor worker pool which assigns the PRACH occasion requests to the first available worker.
class prach_processor_impl : public prach_processor,
                             private prach_processor_baseband,
                             private prach_processor_request_handler
{
private:
  std::vector<std::unique_ptr<prach_processor_worker>> workers;
  prach_processor_notifier*                            notifier = nullptr;

  // See prach_processor_request_handler for documentation.
  void handle_request(prach_buffer& buffer, const prach_buffer_context& context) override
  {
    srsran_assert(notifier, "Notifier has not been connected.");

    // Iterate all workers...
    for (std::unique_ptr<prach_processor_worker>& worker : workers) {
      // Select the first worker available.
      if (worker->is_available()) {
        // Set request.
        worker->handle_request(buffer, context);

        // Stop iterating.
        return;
      }
    }

    // If no available workers, notify error and the buffer is discarded.
    notifier->on_prach_request_overflow(context);
  }

  // See prach_processor_baseband for documentation.
  void process_symbol(const baseband_gateway_buffer_reader& samples, const symbol_context& context) override
  {
    // Iterate all workers...
    for (std::unique_ptr<prach_processor_worker>& worker : workers) {
      // Process baseband.
      worker->process_symbol(samples, context);
    }
  }

public:
  /// Creates a PRACH processor containing workers.
  explicit prach_processor_impl(std::vector<std::unique_ptr<prach_processor_worker>>&& workers_) :
    workers(std::move(workers_))
  {
    srsran_assert(!workers.empty(), "No workers are available.");
    for (const std::unique_ptr<prach_processor_worker>& worker : workers) {
      srsran_assert(worker, "Invalid worker.");
    }
  }

  // See prach_processor for documentation.
  void connect(prach_processor_notifier& notifier_) override
  {
    for (std::unique_ptr<prach_processor_worker>& worker : workers) {
      worker->connect(notifier_);
    }
    notifier = &notifier_;
  }

  // See prach_processor for documentation.
  prach_processor_request_handler& get_request_handler() override { return *this; }

  // See prach_processor for documentation.
  prach_processor_baseband& get_baseband() override { return *this; }
};

} // namespace srsran
