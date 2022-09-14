/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/channel_estimation.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

namespace srsgnb {

/// PUCCH demodulator interface for Formats 2, 3 and 4.
class pucch_demodulator
{
public:
  /// Collects PUCCH format 2 demodulation parameters.
  struct format2_configuration {
    // Add here PUCCH demodulator parameters...
  };

  /// Collects PUCCH format 3 demodulation parameters.
  struct format3_configuration {
    // Add here PUCCH demodulator parameters...
  };

  /// Collects PUCCH format 4 demodulation parameters.
  struct format4_configuration {
    // Add here PUCCH demodulator parameters...
  };

  /// Default destructor.
  virtual ~pucch_demodulator() = default;

  /// \brief Demodulates a PUCCH format 2 transmission.
  /// \param[out] llr       Destination PUCCH soft-bit buffer.
  /// \param[in]  grid      Input resource grid.
  /// \param[in]  estimates Input resource grid.
  /// \param[in]  config    PUCCH format 2 configuration parameters.
  virtual void demodulate_format2(span<log_likelihood_ratio>   llr,
                                  const resource_grid_reader&  grid,
                                  const channel_estimate&      estimates,
                                  const format2_configuration& config) = 0;

  /// \brief Demodulates a PUCCH format 3 transmission.
  /// \param[out] llr       Destination PUCCH soft-bit buffer.
  /// \param[in]  grid      Input resource grid.
  /// \param[in]  estimates Input resource grid.
  /// \param[in]  config    PUCCH format 3 configuration parameters.
  virtual void demodulate_format3(span<log_likelihood_ratio>   llr,
                                  const resource_grid_reader&  grid,
                                  const channel_estimate&      estimates,
                                  const format3_configuration& config) = 0;

  /// \brief Demodulates a PUCCH format 4 transmission.
  /// \param[out] llr       Destination PUCCH soft-bit buffer.
  /// \param[in]  grid      Input resource grid.
  /// \param[in]  estimates Input resource grid.
  /// \param[in]  config    PUCCH format 4 configuration parameters.
  virtual void demodulate_format4(span<log_likelihood_ratio>   llr,
                                  const resource_grid_reader&  grid,
                                  const channel_estimate&      estimates,
                                  const format4_configuration& config) = 0;
};

} // namespace srsgnb
