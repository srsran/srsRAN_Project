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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "dft_processor_fftw_impl.h"
#include "dft_processor_generic_impl.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

namespace {

class dft_processor_factory_generic : public dft_processor_factory
{
public:
  std::unique_ptr<dft_processor> create(const dft_processor::configuration& config) override
  {
    std::unique_ptr<dft_processor_generic_impl> dft = std::make_unique<dft_processor_generic_impl>(config);
    if (!dft->is_valid()) {
      return nullptr;
    }
    return dft;
  }
};

#ifdef ENABLE_FFTW
class dft_processor_factory_fftw : public dft_processor_factory
{
  dft_processor_fftw_configuration fftw_config;

public:
  dft_processor_factory_fftw(const std::string& optimization_flag,
                             double             plan_creation_timeout_s,
                             bool               avoid_wisdom,
                             const std::string& wisdom_filename)
  {
    fftw_config.avoid_wisdom            = avoid_wisdom;
    fftw_config.wisdom_filename         = wisdom_filename;
    fftw_config.plan_creation_timeout_s = plan_creation_timeout_s;

    if ((optimization_flag == "") || (optimization_flag == "estimate")) {
      fftw_config.optimization_flag = fftw_plan_optimization::fftw_estimate;
    } else if (optimization_flag == "measure") {
      fftw_config.optimization_flag = fftw_plan_optimization::fftw_measure;
    } else if (optimization_flag == "exhaustive") {
      fftw_config.optimization_flag = fftw_plan_optimization::fftw_exhaustive;
    } else {
      report_error("Invalid FFTW optimization flag: {}\n", optimization_flag);
    }
  }

private:
  std::unique_ptr<dft_processor> create(const dft_processor::configuration& dft_config) override
  {
    return std::make_unique<dft_processor_fftw_impl>(fftw_config, dft_config);
  }
};
#endif // ENABLE_FFTW

} // namespace

std::shared_ptr<dft_processor_factory> srsran::create_dft_processor_factory_generic()
{
  return std::make_shared<dft_processor_factory_generic>();
}

std::shared_ptr<dft_processor_factory> srsran::create_dft_processor_factory_fftw(const std::string& optimization_flag,
                                                                                 double plan_creation_timeout_s,
                                                                                 bool   avoid_wisdom,
                                                                                 const std::string& wisdom_filename)
{
#ifdef ENABLE_FFTW
  return std::make_shared<dft_processor_factory_fftw>(
      optimization_flag, plan_creation_timeout_s, avoid_wisdom, wisdom_filename);
#else  // ENABLE_FFTW
  return nullptr;
#endif // ENABLE_FFTW
}
