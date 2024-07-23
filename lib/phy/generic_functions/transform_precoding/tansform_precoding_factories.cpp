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

#include "transform_precoder_dft_impl.h"
#include "srsran/phy/generic_functions/transform_precoding/transform_precoding_factories.h"
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"

#include <srsran/phy/constants.h>

using namespace srsran;

namespace {

/// Implements a transform precoder factory based on DFT algorithm.
class transform_precoder_dft_factory : public transform_precoder_factory
{
public:
  transform_precoder_dft_factory(std::shared_ptr<dft_processor_factory> dft_factory_, unsigned max_nof_prb_) :
    dft_factory(std::move(dft_factory_)), max_nof_prb(max_nof_prb_)
  {
    srsran_assert(dft_factory, "Invalid DFt factory.");
  }

  // See interface for documentation.
  std::unique_ptr<transform_precoder> create() override
  {
    // Create DFT processors for each valid number of PRBs within the limit.
    transform_precoder_dft_impl::collection_dft_processors dft_processors;
    for (unsigned nof_prb = 0; nof_prb != max_nof_prb; ++nof_prb) {
      if (is_transform_precoding_nof_prb_valid(nof_prb)) {
        // Prepare DFT size.
        dft_processor::configuration dft_config;
        dft_config.dir  = dft_processor::direction::INVERSE;
        dft_config.size = NRE * nof_prb;

        dft_processors.emplace(nof_prb, dft_factory->create(dft_config));
      }
    }

    return std::make_unique<transform_precoder_dft_impl>(std::move(dft_processors));
  }

private:
  /// DFT processor factory.
  std::shared_ptr<dft_processor_factory> dft_factory;
  /// Maximum number of PRB.
  unsigned max_nof_prb;
};

} // namespace

std::shared_ptr<transform_precoder_factory>
srsran::create_dft_transform_precoder_factory(std::shared_ptr<dft_processor_factory> dft_factory, unsigned max_nof_prb)
{
  return std::make_shared<transform_precoder_dft_factory>(std::move(dft_factory), max_nof_prb);
}
