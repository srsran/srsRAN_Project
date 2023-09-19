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

#include "srsran/adt/span.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder_properties.h"
#include "srsran/ofh/slot_symbol_point.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane message decoder interface.
class uplane_message_decoder
{
public:
  virtual ~uplane_message_decoder() = default;

  /// Peeks and returns the filter index of the given message.
  ///
  /// \param[in] message Message to peek.
  /// \return Message filter index or reserved if the filter index cannot be peeked.
  virtual filter_index_type peek_filter_index(span<const uint8_t> message) const = 0;

  /// Peeks and returns the slot symbol point of the given message.
  ///
  /// \param[in] message Message to peek.
  /// \return Message slot symbol point or invalid slot if the slot symbol point cannot be peeked.
  virtual slot_symbol_point peek_slot_symbol_point(span<const uint8_t> message) const = 0;

  /// Decodes the given message into results and returns true on success, false otherwise.
  ///
  /// \param[out] results Results of decoding the message. On error, results value is undefined.
  /// \param[in] message Message to be decoded.
  /// \return True on success, false otherwise.
  virtual bool decode(uplane_message_decoder_results& results, span<const uint8_t> message) = 0;
};

} // namespace ofh
} // namespace srsran
