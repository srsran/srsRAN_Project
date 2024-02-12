/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "uci_status.h"
#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/ran/sch/modulation_scheme.h"

namespace srsran {

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
  /// \param[in]  llr     The received soft bits, as a sequence of log-likelihood ratios.
  /// \return The decoding status.
  virtual uci_status
  decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) = 0;
};

} // namespace srsran
