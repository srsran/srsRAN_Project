/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                             bool               avoid_wisdom,
                             const std::string& wisdom_filename)
  {
    fftw_config.avoid_wisdom    = avoid_wisdom;
    fftw_config.wisdom_filename = wisdom_filename;

    if ((optimization_flag == "") || (optimization_flag == "fftw_estimate")) {
      fftw_config.optimization_flag = fftw_plan_optimization::fftw_estimate;
    } else if (optimization_flag == "fftw_measure") {
      fftw_config.optimization_flag = fftw_plan_optimization::fftw_measure;
    } else {
      report_error("Invalid FFTW optimization flag: {}", optimization_flag);
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
                                                                                 bool               avoid_wisdom,
                                                                                 const std::string& wisdom_filename)
{
#ifdef ENABLE_FFTW
  return std::make_shared<dft_processor_factory_fftw>(optimization_flag, avoid_wisdom, wisdom_filename);
#else  // ENABLE_FFTW
  return nullptr;
#endif // ENABLE_FFTW
}
