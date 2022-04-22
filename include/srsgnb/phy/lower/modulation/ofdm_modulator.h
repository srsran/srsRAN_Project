
#ifndef SRSGNB_PHY_LOWER_MODULATION_OFDM_MODULATOR_H
#define SRSGNB_PHY_LOWER_MODULATION_OFDM_MODULATOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

/// Setup configuration parameters.
struct ofdm_modulator_configuration {
  /// Indicates the numeorology of the OFDM modulator.
  unsigned numerology;
  /// Indicates the resource grid bandwidth in resource blocks.
  unsigned bw_rb;
  /// Indicates the DFT size.
  unsigned dft_size;
  /// Indicates the cyclic prefix.
  cyclic_prefix cp;
  /// Indicates the scaling factor at the DFT output.
  float scale;
  /// Indicates the center frequency of the carrier in Hz.
  double center_freq_hz;
};

/// \brief Describes an OFDM modulator that modulates at symbol granularity.
/// \remark Performs OFDM modulation as per TS 38.211 section 5.3.1 OFDM baseband signal generation for all channels
/// except PRACH.
/// \remark In addition to modulation, it applies phase compensation as per TS 38.211 section 5.4 Modulation and
/// upconversion
class ofdm_symbol_modulator
{
public:
  /// Default destructor.
  virtual ~ofdm_symbol_modulator() = default;

  /// \brief Gets a symbol size including cyclic prefix.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The number of samples for the given symbol index.
  virtual unsigned get_symbol_size(unsigned symbol_index) const = 0;

  /// \brief Gets the offset to a symbol including the cyclic prefixes.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The number of samples to the start of the given symbol.
  virtual unsigned get_symbol_offset(unsigned symbol_index) const = 0;

  /// \brief Computes the phase compensation (TS 138.211, Section 5.4) for a given symbol.
  /// \param[in] symbol_index Indicates the symbol index within the subframe.
  /// \return The phase compensation to be applied to the given symbol.
  virtual cf_t get_phase_compensation(unsigned symbol_index) const = 0;

  /// \brief Modulates an OFDM signal with symbol granularity.
  /// \param[out] output Provides the time domain modulated signal.
  /// \param[in] grid Provides the input as frequency-domain signal corresponding to one slot.
  /// \param[in] port_index Indicates the port index to modulate.
  /// \param[in] symbol_index Symbol index within the subframe to modulate.
  /// \note The input size must be consistent with the configured bandwidth.
  /// \note The output size must be equal to the the symbol size.
  virtual void
  modulate(span<cf_t> output, const resource_grid_reader& grid, unsigned port_index, unsigned symbol_index) = 0;
};

/// \brief Describes an OFDM modulator with slot granularity.
/// \remark Performs OFDM modulation as per TS 38.211 section 5.3.1 OFDM baseband signal generation for all channels
/// except PRACH.
/// \remark In addition to modulation, it applies phase compensation as per TS 38.211 section 5.4 Modulation and
/// upconversion.
class ofdm_slot_modulator
{
public:
  /// Default destructor.
  virtual ~ofdm_slot_modulator() = default;

  /// \brief Gets a slot size.
  /// \param[in] slot_index Indicates the slot index within the subframe.
  /// \return The number of samples for the given slot index.
  virtual unsigned get_slot_size(unsigned slot_index) const = 0;

  /// \brief Modulates an OFDM signal with slot granularity.
  /// \param[out] output Provides the time domain modulated signal destination.
  /// \param[in] grid Provides the input as frequency-domain signal corresponding to one slot.
  /// \param[in] port_index Indicates the port index to modulate.
  /// \param[in] slot_index Slot index within the subframe to modulate.
  /// \note The input size must be consistent with the configured bandwidth.
  /// \note The output size must be equal to the slot size.
  virtual void
  modulate(span<cf_t> output, const resource_grid_reader& grid, unsigned port_index, unsigned slot_index) = 0;
};

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
} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_MODULATION_OFDM_MODULATOR_H
