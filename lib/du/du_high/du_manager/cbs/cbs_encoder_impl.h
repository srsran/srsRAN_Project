/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

/// \file
/// \brief GSM-7 and UCS-2 encoder for CBS messages.

#pragma once

#include "srsran/du/du_high/du_manager/cbs/cbs_encoder.h"

namespace srsran {

/// \brief GSM-7 and UCS-2 encoder for CBS messages.
///
/// This class implements ASCII to GSM-7 encoding using the default alphabet and the default alphabet extension tables
/// defined in TS23.038 Section 6.2.1, as well as UTF-8 to UCS-2 encoding as defined in TS23.038 Section 6.2.3. The
/// GSM-7 encoded messages are packed following the CBS packing format defined in TS23.038 Section 6.1.2.2.
class cbs_encoder_impl : public cbs_encoder
{
public:
  // See interface for documentation.
  std::vector<uint8_t> encode_cb_data(const std::string& message, unsigned data_coding_scheme) const override;

private:
  /// Supported CBS message encoding options.
  enum class encoding_type {
    /// GSM-7 7-bit character encoding using the default alphabet and its extension table.
    GSM7,
    /// UCS-2 16-bit character encoding supporting all unicode code points in the basic multilingual plane.
    UCS2,
    /// Invalid or unsupported encoding formats.
    INVALID
  };

  /// Selects either GSM-7 or UCS-2 encoding based on the SIB-7 and SIB-8 \c dataCodingScheme field value.
  static encoding_type select_cbs_encoding(uint8_t data_coding_scheme);

  /// \brief Fills a CB-Data information element with the contents of a CBS message using GSM-7 encoding.
  ///
  /// Only printable ASCII characters in the default GSM-7 alphabet and its extension table are supported by this
  /// method.
  ///
  /// \param[in] message ETWS/CMAS warning message string.
  /// \return A packed CB-Data IE with the contents of the message.
  /// \remark An error is thrown if any character in the warning message is not present in the GSM-7 default alphabet
  ///         or is not a printable ASCII character.
  static std::vector<uint8_t> fill_cb_data_gsm7(const std::string& message);

  /// \brief Fills a CB-Data information element with the contents of a CBS message using UCS-2 encoding.
  ///
  /// All unicode code points in the basic multilingual plane are supported by this method.
  ///
  /// \param[in] message ETWS/CMAS warning message string.
  /// \return A packed CB-Data IE with the contents of the message.
  /// \remark An error is thrown if any character in the warning message is not present in the basic multilingual
  ///         unicode plane.
  static std::vector<uint8_t> fill_cb_data_ucs2(const std::string& message);

  /// CB Data IE Information Page field number of bytes. See TS23.041 Section 9.4.2.2.5.
  static constexpr unsigned info_page_nof_bytes = 82;
  /// CB Data IE Information Page Length field number of bytes.
  static constexpr unsigned info_len_nof_bytes = 1;
  /// Maximum number of CB Data IE information pages.
  static constexpr unsigned max_nof_cb_data_pages = 15;
};

std::unique_ptr<cbs_encoder> create_cbs_encoder()
{
  return std::make_unique<cbs_encoder_impl>();
}

} // namespace srsran
