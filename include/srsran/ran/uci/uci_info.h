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

#include "srsran/support/math/math_utils.h"

namespace srsran {

/// \brief Gets the number of codeblocks for a transmission containing a UCI message.
///
/// The number of UCI codeblocks and codeword segmentation is described TS38.212 Section 6.3.1.2.1.
///
/// \param[in] message_length  Number of payload bits in the UCI message. Parameter \f$A\f$.
/// \param[in] codeword_length Number of encoded bits in the codeword. Parameter \f$E\f$.
/// \return The number of codeblocks the payload is divided into. One if \f$I_{seg}\f$ results in zero, otherwise two.
constexpr unsigned get_nof_uci_codeblocks(unsigned message_length, unsigned codeword_length)
{
  if (((message_length >= 360) && (codeword_length >= 1088)) || (message_length >= 1013)) {
    return 2;
  }
  return 1;
}

/// \brief Gets the number of CRC bits per UCI codeblock.
///
/// The number of CRC bits is described in TS38.212 Section 6.3.1.2.1.
///
/// \param[in] payload_size Number of payload bits in the UCI message. Parameter \f$A\f$.
/// \return The CRC number of bits per codeblock. Parameter \f$L\f$.
constexpr unsigned get_uci_crc_size(unsigned payload_size)
{
  if (payload_size < 12) {
    return 0;
  }

  if (payload_size < 20) {
    return 6;
  }

  return 11;
}

/// Gets the total number of CRC bits appended in a UCI message, considering segmentation.
constexpr unsigned get_uci_nof_crc_bits(unsigned message_length, unsigned codeword_length)
{
  // Number of codeblocks.
  unsigned nof_codeblocks = get_nof_uci_codeblocks(message_length, codeword_length);

  // Get number of CRC bits per codeblock.
  unsigned nof_crc_bits = get_uci_crc_size(message_length);

  return nof_codeblocks * nof_crc_bits;
}

} // namespace srsran
