/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_UCI_CONSTANTS_H
#define SRSGNB_RAN_UCI_CONSTANTS_H

namespace srsgnb {
namespace uci_constants {

/// \brief Maximum number of HARQ ACK bits that can be multiplexed in a single message.
///
/// The value is deduced from the maximum number of bits that can be encoded with the polar encoder as per TS38.212
/// section 5.2.1.
static constexpr unsigned MAX_NOF_HARQ_ACK_BITS = 1706;

/// \brief Maximum number of CSI Part 1 bits that can be multiplexed in a single message.
///
/// The value is deduced from the maximum number of bits that can be encoded with the polar encoder as per TS38.212
/// section 5.2.1.
static constexpr unsigned MAX_NOF_CSI_PART1_BITS = 1706;

} // namespace uci_constants
} // namespace srsgnb

#endif // SRSGNB_RAN_UCI_CONSTANTS_H
