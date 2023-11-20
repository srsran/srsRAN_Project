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

#include "srsran/adt/span.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder_properties.h"
#include "srsran/ofh/timing/slot_symbol_point.h"

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
