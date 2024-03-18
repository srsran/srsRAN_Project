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
    dft_processor::configuration dft_proc_config;
    dft_proc_config.size = time_alignment_estimator_dft_impl::dft_size;
    dft_proc_config.dir  = time_alignment_estimator_dft_impl::dft_direction;

    return std::make_unique<time_alignment_estimator_dft_impl>(dft_proc_factory->create(dft_proc_config));
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
