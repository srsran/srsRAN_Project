/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_GENERIC_FUNCTIONS_GENERIC_FUNCTIONS_FACTORIES_H
#define SRSGNB_PHY_GENERIC_FUNCTIONS_GENERIC_FUNCTIONS_FACTORIES_H

#include "srsgnb/phy/generic_functions/dft_processor.h"

namespace srsgnb {

/// Describes a Discrete Fourier Transform (DFT) processor factory.
class dft_processor_factory
{
public:
  /// Default destructor.
  virtual ~dft_processor_factory() = default;

  /// \brief Creates a DFT processor.
  /// \param[in] config Provides the DFT processor parameters.
  /// \return A unique pointer of a DFT processor if the configuration is valid. Otherwise, \c nullptr.
  virtual std::unique_ptr<dft_processor> create(const dft_processor::configuration& config) = 0;
};

/// Provides the necessary initialization parameters to dft_processor_fftw_impl.
struct dft_processor_factory_fftw_configuration {
  /// Sets to true to avoid loading the FFTW wisdom from a file.
  bool avoid_wisdom;
  /// Provides the FFTW wisdom filename. Leave empty for default value and ignore if wisdom is disabled.
  std::string wisdom_filename;
};

std::unique_ptr<dft_processor_factory>
create_dft_processor_factory_fftw(const dft_processor_factory_fftw_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_GENERIC_FUNCTIONS_GENERIC_FUNCTIONS_FACTORIES_H
