/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"
#include "time_alignment_estimator_dft_impl.h"
#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/support/srsran_assert.h"
#include <memory>

using namespace srsran;

namespace {

class time_alignment_estimator_dft_factory : public time_alignment_estimator_factory
{
public:
  time_alignment_estimator_dft_factory(std::shared_ptr<dft_processor_factory> dft_proc_factory_) :
    dft_proc_factory(std::move(dft_proc_factory_))
  {
    srsran_assert(dft_proc_factory, "Invalid DFT processor factory.");
  }

  std::unique_ptr<time_alignment_estimator> create() override
  {
    time_alignment_estimator_dft_impl::collection_dft_processors dft_processors;
    for (unsigned dft_size     = time_alignment_estimator_dft_impl::min_dft_size,
                  dft_size_max = time_alignment_estimator_dft_impl::max_dft_size;
         dft_size <= dft_size_max;
         dft_size *= 2) {
      dft_processor::configuration dft_proc_config;
      dft_proc_config.size = dft_size;
      dft_proc_config.dir  = time_alignment_estimator_dft_impl::dft_direction;

      dft_processors.emplace(dft_size, dft_proc_factory->create(dft_proc_config));
    }

    return std::make_unique<time_alignment_estimator_dft_impl>(std::move(dft_processors));
  }

private:
  std::shared_ptr<dft_processor_factory> dft_proc_factory;
};

} // namespace

std::shared_ptr<time_alignment_estimator_factory>
srsran::create_time_alignment_estimator_dft_factory(std::shared_ptr<dft_processor_factory> dft_proc_factory)
{
  return std::make_shared<time_alignment_estimator_dft_factory>(std::move(dft_proc_factory));
}
