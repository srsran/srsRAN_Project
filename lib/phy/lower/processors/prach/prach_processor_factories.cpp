/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/lower/processors/prach/prach_processor_factories.h"
#include "prach_processor_impl.h"
#include "prach_processor_worker.h"
#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "srsgnb/support/error_handling.h"

using namespace srsgnb;

namespace {

class prach_processor_factory_sw : public prach_processor_factory
{
private:
  std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory;
  unsigned                                        dft_size_15kHz;
  unsigned                                        max_nof_concurrent_requests;

public:
  prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory>& ofdm_prach_factory_,
                             unsigned                                         dft_size_15kHz_,
                             unsigned                                         max_nof_concurrent_requests_) :
    ofdm_prach_factory(ofdm_prach_factory_),
    dft_size_15kHz(dft_size_15kHz_),
    max_nof_concurrent_requests(max_nof_concurrent_requests_)
  {
    report_fatal_error_if_not(ofdm_prach_factory, "Invalid OFDM PRACH factory.");
    report_fatal_error_if_not(dft_size_15kHz, "Zero is an invalid DFT size of 15kHz.");
    report_fatal_error_if_not(max_nof_concurrent_requests, "Zero is an invalid maximum number of concurrent reqs.");
  }

  std::unique_ptr<prach_processor> create(task_executor& async_task_executor) override
  {
    std::vector<std::unique_ptr<prach_processor_worker>> workers;
    workers.reserve(max_nof_concurrent_requests);
    for (unsigned count = 0; count != max_nof_concurrent_requests; ++count) {
      workers.push_back(
          std::make_unique<prach_processor_worker>(ofdm_prach_factory->create(), async_task_executor, dft_size_15kHz));
    }

    return std::make_unique<prach_processor_impl>(std::move(workers));
  }
};

} // namespace

std::shared_ptr<prach_processor_factory>
srsgnb::create_prach_processor_factory_sw(std::shared_ptr<ofdm_prach_demodulator_factory> ofdm_prach_factory,
                                          unsigned                                        dft_size_15kHz,
                                          unsigned                                        max_nof_concurrent_requests)
{
  return std::make_shared<prach_processor_factory_sw>(ofdm_prach_factory, dft_size_15kHz, max_nof_concurrent_requests);
}
