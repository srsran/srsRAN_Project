/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/ran/pucch/pucch_constants.h"
#include "srsgnb/ran/resource_block.h"

namespace srsgnb {

/// \brief Calculates the effective code rate for a PUCCH Format 2 transmission.
/// \param[in] nof_prb           Transmission bandwidth in PRB.
/// \param[in] nof_symbols       Transmission duration in symbols.
/// \param[in] nof_payload_bits  Total number of payload bits.
/// \return The effective code rate of the PUCCH Format 2 transmission.
constexpr float pucch_format2_code_rate(unsigned nof_prb, unsigned nof_symbols, unsigned nof_payload_bits)
{
  // PUCCH format 2 channel bits are modulated as QPSK and mapped to two of every three resource elements.
  unsigned nof_channel_bits = (nof_prb * pucch_constants::FORMAT2_NOF_DATA_SC * nof_symbols * 2);

  // Calculate code rate.
  return static_cast<float>(nof_payload_bits) / static_cast<float>(nof_channel_bits);
}

} // namespace srsgnb
