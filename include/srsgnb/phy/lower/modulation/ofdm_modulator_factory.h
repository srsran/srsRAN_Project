
#ifndef SRSGNB_PHY_LOWER_MODULATION_OFDM_MODULATOR_FACTORY_H
#define SRSGNB_PHY_LOWER_MODULATION_OFDM_MODULATOR_FACTORY_H

#include "srsgnb/phy/generic_functions/dft_processor_factory.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include <memory>

namespace srsgnb {

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

/// OFDM factory configuration. Provides the necessary parameters
struct ofdm_modulator_factory_configuration {
  /// OFDM modulator implementation type.
  std::string type;
  /// Provides DFT processor factory.
  dft_processor_factory* dft_factory;
};

/// \brief Creates an OFDM modulator factory.
/// \param [in] config Provides the required parameters to create the factory.
/// \return A unique pointer of an OFDM modulator factory if the configuration is valid. Otherwise, nullptr.
std::unique_ptr<ofdm_modulator_factory>
create_ofdm_modulator_factory(const ofdm_modulator_factory_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_MODULATION_OFDM_MODULATOR_FACTORY_H
