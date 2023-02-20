/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/phy/lower/modulation/ofdm_demodulator.h"
#include "srsran/phy/lower/modulation/ofdm_modulator.h"
#include "srsran/phy/lower/modulation/ofdm_prach_demodulator.h"

namespace srsran {

/// Describes an OFDM modulator factory.
class ofdm_modulator_factory
{
public:
  /// Default destructor.
  virtual ~ofdm_modulator_factory() = default;

  /// \brief Creates an OFDM modulator that modulates with symbol granularity.
  /// \param[in] config Provides the configuration parameters.
  /// \return A unique pointer to an OFDM symbol modulator if the provided parameters are valid, \c nullptr otherwise.
  virtual std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& config) = 0;

  /// \brief Creates an OFDM modulator that modulates with slot granularity.
  /// \param[in] config Provides the configuration parameters.
  /// \return A unique pointer to an OFDM slot modulator if the provided parameters are valid, \c nullptr otherwise.
  virtual std::unique_ptr<ofdm_slot_modulator>
  create_ofdm_slot_modulator(const ofdm_modulator_configuration& config) = 0;
};

/// Describes an OFDM demodulator factory.
class ofdm_demodulator_factory
{
public:
  /// Default destructor.
  virtual ~ofdm_demodulator_factory() = default;

  /// \brief Creates an OFDM demodulator that demodulates with symbol granularity.
  /// \param[in] config Provides the configuration parameters.
  /// \return A unique pointer to an OFDM symbol demodulator if the provided parameters are valid, \c nullptr otherwise.
  virtual std::unique_ptr<ofdm_symbol_demodulator>
  create_ofdm_symbol_demodulator(const ofdm_demodulator_configuration& config) = 0;

  /// \brief Creates an OFDM demodulator that modulates with slot granularity.
  /// \param[in] config Provides the configuration parameters.
  /// \return A unique pointer to an OFDM slot demodulator if the provided parameters are valid, \c nullptr otherwise.
  virtual std::unique_ptr<ofdm_slot_demodulator>
  create_ofdm_slot_demodulator(const ofdm_demodulator_configuration& config) = 0;
};

/// OFDM PRACH demodulator factory.
class ofdm_prach_demodulator_factory
{
public:
  /// Default destructor.
  virtual ~ofdm_prach_demodulator_factory() = default;

  /// Creates an OFDM PRACH demodulator.
  virtual std::unique_ptr<ofdm_prach_demodulator> create() = 0;
};

/// Describes the necessary parameters for creating generic OFDM modulator and demodulator factories.
struct ofdm_factory_generic_configuration {
  /// Provides a DFT factory.
  std::shared_ptr<dft_processor_factory> dft_factory;
};

/// Creates a generic OFDM modulator factory.
std::shared_ptr<ofdm_modulator_factory>
create_ofdm_modulator_factory_generic(ofdm_factory_generic_configuration& config);

/// Creates a generic OFDM demodulator factory.
std::shared_ptr<ofdm_demodulator_factory>
create_ofdm_demodulator_factory_generic(ofdm_factory_generic_configuration& config);

/// \brief Creates a software generic PRACH demodulator.
/// \param[in] dft_factory     DFT factory.
/// \param[in] dft_size_15kHz  DFT size for 15kHz subcarrier spacing.
std::shared_ptr<ofdm_prach_demodulator_factory>
create_ofdm_prach_demodulator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory, unsigned dft_size_15kHz);

} // namespace srsran
