/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/generic_functions/generic_functions_factories.h"
#include "dft_processor_fftw_impl.h"
#include "dft_processor_fftx_impl.h"
#include "dft_processor_generic_impl.h"

using namespace srsgnb;

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

class dft_processor_factory_fftx : public dft_processor_factory
{
public:
  std::unique_ptr<dft_processor> create(const dft_processor::configuration& config) override
  {
    std::unique_ptr<dft_processor_fftx_impl> dft = std::make_unique<dft_processor_fftx_impl>(config);
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
  dft_processor_factory_fftw(bool avoid_wisdom, std::string wisdom_filename)
  {
    fftw_config.avoid_wisdom    = avoid_wisdom;
    fftw_config.wisdom_filename = wisdom_filename;
  }

private:
  std::unique_ptr<dft_processor> create(const dft_processor::configuration& dft_config) override
  {
    return std::make_unique<dft_processor_fftw_impl>(fftw_config, dft_config);
  }
};
#endif // ENABLE_FFTW

} // namespace

std::shared_ptr<dft_processor_factory> srsgnb::create_dft_processor_factory_generic()
{
  return std::make_shared<dft_processor_factory_generic>();
}

std::shared_ptr<dft_processor_factory> srsgnb::create_dft_processor_factory_fftx()
{
  return std::make_shared<dft_processor_factory_fftx>();
}

std::shared_ptr<dft_processor_factory> srsgnb::create_dft_processor_factory_fftw(bool               avoid_wisdom,
                                                                                 const std::string& wisdom_filename)
{
#ifdef ENABLE_FFTW
  return std::make_shared<dft_processor_factory_fftw>(avoid_wisdom, wisdom_filename);
#else  // ENABLE_FFTW
  return nullptr;
#endif // ENABLE_FFTW
}