/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/phy/upper/channel_processors/uci_decoder.h"

namespace srsran {

/// PUSCH UCI decoder interface to notify the decoding of a UCI message field.
class pusch_uci_decoder_notifier
{
public:
  /// Default destructor.
  virtual ~pusch_uci_decoder_notifier() = default;

  /// \brief Notifies the completion of a PUSCH UCI message field decoding.
  /// \param[in] message decoded PUSCH UCI message.
  /// \param[in] status  UCI decoding status.
  virtual void on_uci_decoded(span<const uint8_t> message, const uci_status& status) = 0;
};

} // namespace srsran
