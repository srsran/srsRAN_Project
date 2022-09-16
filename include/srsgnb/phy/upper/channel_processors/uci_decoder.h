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

#include "srsgnb/phy/upper/channel_processors/uci_status.h"

namespace srsgnb {

/// \brief Uplink Control Information decoder.
///
/// Decodes UCI payloads containing SR, HARQ-ACK and/or CSI bits, transmitted on PUCCH, reversing the steps described
/// in TS38.212 Sections 6.3.1.2, 6.3.1.3, 6.3.1.4 and 6.3.1.5.
///
/// Decodes UCI payloads of either HARQ-ACK or CSI bits, transmitted on PUSCH, reversing the steps described in
/// TS38.212 Sections 6.3.2.2, 6.3.2.3 and 6.3.2.4.
class uci_decoder
{
public:
  /// Collects UCI decoder configuration parameters.
  struct configuration {
    /// Transmission modulation.
    modulation_scheme modulation;
    // Add here other parameters.
    // ...
  };

  /// Default destructor.
  virtual ~uci_decoder() = default;

  /// \brief Decodes Uplink Control Information carried in either PUCCH or PUSCH.
  /// \param[out] message View of the decoded message.
  /// \param[in]  llr     The received soft-bits, as a sequence of log-likelihood ratios.
  /// \return The decoding status.
  virtual uci_status
  decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) = 0;
};

} // namespace srsgnb
