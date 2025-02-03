/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Interface for an encoder for CBS messages.

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace srsran {

/// Interface for a CBS message encoder.
class cbs_encoder
{
public:
  /// Default destructor.
  virtual ~cbs_encoder() = default;

  /// \brief Fills a CB-Data Information Element with the contents of a CBS message.
  ///
  /// This method encodes the CBS message following the procedures in TS23.038 Section 6.2, and packs the encoded
  /// characters into pages of a CB-Data Information Element (see TS23.041 Section 9.4.2.2.5). The encoding algorithm is
  /// derived from the \c data_coding_scheme value (see TS23.038 Section 5).
  ///
  /// An error is thrown in either one of these conditions:
  ///   - The requested \c data_coding_scheme is invalid or unsupported.
  ///   - The message to be encoded contains characters that are not supported by the chosen coding scheme.
  ///   - The size of the encoded message exceeds the maximum size of the CB-Data IE.
  ///
  /// \param[in] message            CBS message to be encoded.
  /// \param[in] data_coding_scheme Data coding scheme that will be applied to the message. Values: {0, ..., 0xff}
  /// \return A packed CB-Data IE that contains the encoded message.
  virtual std::vector<uint8_t> encode_cb_data(const std::string& message, unsigned data_coding_scheme) const = 0;
};

/// Creates and returns a CBS encoder.
std::unique_ptr<cbs_encoder> create_cbs_encoder();

} // namespace srsran
