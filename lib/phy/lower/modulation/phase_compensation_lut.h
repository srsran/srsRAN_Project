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

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"

namespace srsgnb {

/// \brief Phase compensation as per TS38.211 Section 5.4.
///
/// Implements the phase compensation for OFDM modulation and demodulation as described in TS38.211 Section 5.4. The
/// phase compensation is implemented as a look-up table populated at construction time.
class phase_compensation_lut
{
private:
  /// Stores the coefficients for every symbol in a subframe.
  static_vector<cf_t, MAX_NSYMB_PER_SLOT * get_nof_slots_per_subframe(subcarrier_spacing::kHz240)> coefficients;

public:
  /// \brief Constructs the phase compensation look-up table.
  /// \param[in] scs                 Subcarrier spacing.
  /// \param[in] cp                  Cyclic Prefix.
  /// \param[in] dft_size            DFT size.
  /// \param[in] center_frequency_Hz Center frequency in Hz.
  /// \param[in] is_tx               Set to true if the phase correction table is for transmission.
  phase_compensation_lut(subcarrier_spacing scs,
                         cyclic_prefix      cp,
                         unsigned           dft_size,
                         double             center_frequency_Hz,
                         bool               is_tx)
  {
    double sample_rate_Hz = static_cast<double>(scs_to_khz(scs) * dft_size) * 1e3;
    srsran_assert(std::isnormal(sample_rate_Hz),
                  "Invalid sampling rate from SCS {} kHz and DFT size {}.",
                  scs_to_khz(scs),
                  dft_size);

    unsigned                          nslot_per_subframe = get_nof_slots_per_subframe(scs);
    unsigned                          nsymb_per_slot     = get_nsymb_per_slot(cp);
    double                            sign_two_pi        = ((is_tx) ? -1 : 1) * 2.0 * M_PI;
    static const std::complex<double> i                  = {0.0, 1.0};

    // For each symbol in a subframe.
    for (unsigned symbol = 0, symbol_offset = 0; symbol != nslot_per_subframe * nsymb_per_slot; ++symbol) {
      // Add cyclic prefix length to the symbol offset.
      symbol_offset += cp.get_length(symbol, to_numerology_value(scs), dft_size);

      // Calculate the time between the start of the subframe and the start of the symbol.
      double start_time_s = static_cast<double>(symbol_offset) / sample_rate_Hz;

      // Calculate the phase in radians.
      double symbol_phase = sign_two_pi * center_frequency_Hz * start_time_s;

      // Calculate phase compensation.
      coefficients.emplace_back(static_cast<cf_t>(std::exp(i * symbol_phase)));

      // Advance the symbol size.
      symbol_offset += dft_size;
    }
  }

  /// \brief Get the phase compensation for a symbol.
  /// \param[in] symbol_index Symbol index within a subframe.
  /// \return The phase compensation coefficient for a given symbol within a subframe.
  /// \remark An assertion is triggered if the symbol index exceeds the number of symbols in a subframe.
  cf_t get_coefficient(unsigned symbol_index) const
  {
    srsran_assert(symbol_index < coefficients.size(),
                  "The symbol index within a subframe {} exceeds the number of symbols in the subframe {}.",
                  symbol_index,
                  coefficients.size());
    return coefficients[symbol_index];
  }
};

} // namespace srsgnb
