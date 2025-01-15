/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

class baseband_gateway_buffer_reader;
struct lower_phy_rx_symbol_context;

/// \brief Lower physical layer PUxCH processor - Baseband interface.
///
/// Processes baseband samples with OFDM symbol granularity. The OFDM symbol size is inferred from the slot numerology.
class puxch_processor_baseband
{
public:
  /// Default destructor.
  virtual ~puxch_processor_baseband() = default;

  /// \brief Processes a baseband OFDM symbol.
  ///
  /// \param[in] samples Baseband samples to process.
  /// \param[in] context OFDM Symbol context.
  /// \return \c true if the signal is processed, \c false otherwise.
  virtual bool process_symbol(const baseband_gateway_buffer_reader& samples,
                              const lower_phy_rx_symbol_context&    context) = 0;
};

} // namespace srsran
