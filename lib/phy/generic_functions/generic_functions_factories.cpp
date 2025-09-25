/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "dft_processor_generic_impl.h"
#include "srsran/support/cpu_features.h"
#include "srsran/support/error_handling.h"

#ifdef HAVE_FFTW
#include "dft_processor_fftw_impl.h"
#endif // HAVE_FFTW

#ifdef __x86_64__
#include "dft_processor_ci16_avx2.h"
#endif // __x86_64__

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

#ifdef __x86_64__
class dft_processor_ci16_factory_avx2 : public dft_processor_ci16_factory
{
public:
  std::unique_ptr<dft_processor_ci16> create(const dft_processor_ci16::configuration& config) override
  {
    return create_dft_processor_ci16_avx2(config.size, config.dir);
  }
};
#endif // __x86_64__

#ifdef HAVE_FFTW
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
#endif // HAVE_FFTW

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
#ifdef HAVE_FFTW
  return std::make_shared<dft_processor_factory_fftw>(
      optimization_flag, plan_creation_timeout_s, avoid_wisdom, wisdom_filename);
#else  // HAVE_FFTW
  return nullptr;
#endif // HAVE_FFTW
}

std::shared_ptr<dft_processor_ci16_factory> srsran::create_dft_processor_ci16_factory_avx2()
{
#ifdef __x86_64__
  if (cpu_supports_feature(cpu_feature::avx2)) {
    return std::make_shared<dft_processor_ci16_factory_avx2>();
  }
#endif // __x86_64__

  return nullptr;
}
