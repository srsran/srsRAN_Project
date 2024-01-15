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

#include "srsran/phy/lower/processors/uplink/prach/prach_processor_factories.h"
#include "prach_processor_impl.h"
#include "prach_processor_worker.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/phy/lower/sampling_rate.h"

using namespace srsran;

namespace {

class prach_processor_factory_sw : public prach_processor_factory
{
private:
  std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory;
  task_executor&                                  async_task_executor;
  sampling_rate                                   srate;
  unsigned                                        max_nof_ports;
  unsigned                                        max_nof_concurrent_requests;

public:
  prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory>& ofdm_prach_factory_,
                             task_executor&                                   async_task_executor_,
                             sampling_rate                                    srate_,
                             unsigned                                         max_nof_ports_,
                             unsigned                                         max_nof_concurrent_requests_) :
    ofdm_prach_factory(ofdm_prach_factory_),
    async_task_executor(async_task_executor_),
    srate(srate_),
    max_nof_ports(max_nof_ports_),
    max_nof_concurrent_requests(max_nof_concurrent_requests_)
  {
    srsran_assert(ofdm_prach_factory, "Invalid OFDM PRACH factory.");
    srsran_assert(max_nof_concurrent_requests, "Zero is an invalid maximum number of concurrent reqs.");
  }

  std::unique_ptr<prach_processor> create() override
  {
    std::vector<std::unique_ptr<prach_processor_worker>> workers;
    workers.reserve(max_nof_concurrent_requests);
    for (unsigned count = 0; count != max_nof_concurrent_requests; ++count) {
      workers.push_back(std::make_unique<prach_processor_worker>(
          ofdm_prach_factory->create(), async_task_executor, srate, max_nof_ports));
    }

    return std::make_unique<prach_processor_impl>(std::move(workers));
  }
};

} // namespace

std::shared_ptr<prach_processor_factory>
srsran::create_prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory,
                                          task_executor&                                  async_task_executor,
                                          sampling_rate                                   srate,
                                          unsigned                                        max_nof_ports,
                                          unsigned                                        max_nof_concurrent_requests)
{
  return std::make_shared<prach_processor_factory_sw>(
      ofdm_prach_factory, async_task_executor, srate, max_nof_ports, max_nof_concurrent_requests);
}
