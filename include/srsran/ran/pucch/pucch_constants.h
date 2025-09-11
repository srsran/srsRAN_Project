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

#pragma once

#include "srsran/adt/interval.h"
#include "srsran/ran/resource_block.h"

namespace srsran {

namespace pucch_constants {

/// PUCCH hopping identifier, parameter \f$n_{ID}\f$ range.
constexpr interval<unsigned, false> n_id_range(0, 1024);

/// PUCCH does not make use of spatial multiplexing.
constexpr unsigned MAX_LAYERS = 1;

/// PUCCH Format 0 number of OFDM symbols range.
constexpr interval<unsigned, true> format0_nof_symbols_range(1, 2);

/// PUCCH Format 0 range for number of HARQ-ACK feedback bits.
constexpr interval<unsigned, true> format0_nof_harq_ack_range(0, 2);

/// PUCCH Format 0 initial cyclic shift range.
constexpr interval<unsigned, false> format0_initial_cyclic_shift_range(0, 12);

/// Maximum number of PRBs allocated for PUCCH Formats 0, 1 and 4.
constexpr unsigned FORMAT0_1_4_MAX_NPRB = 1;

/// Maximum number of symbols (without DM-RS) that PUCCH Format 1 can transmit.
constexpr unsigned FORMAT1_N_MAX = 7;

/// Minimum number of symbols that PUCCH Format 2 can transmit.
constexpr unsigned FORMAT2_MIN_NSYMB = 1;

/// Maximum number of symbols that PUCCH Format 2 can transmit.
constexpr unsigned FORMAT2_MAX_NSYMB = 2;

/// Maximum number of PRBs allocated for PUCCH Format 2.
constexpr unsigned FORMAT2_MAX_NPRB = 16;

/// Minimum number of UCI payload bits carried by a PUCCH Format 2 resource.
constexpr unsigned FORMAT2_MIN_UCI_NBITS = 3;

/// Number of control data subcarriers per Resource Block for PUCCH Format 2, as per TS38.213 Section 9.1.5.2.
constexpr unsigned FORMAT2_NOF_DATA_SC = 8;

/// PUCCH Format 1 initial cyclic shift range.
constexpr interval<unsigned, false> format1_initial_cyclic_shift_range(0, 12);

/// PUCCH Format 1 initial OCC range.
constexpr interval<unsigned, false> format1_time_domain_occ_range(0, 7);

/// Minimum number of symbols that PUCCH Format 3 can transmit.
constexpr unsigned FORMAT3_MIN_NSYMB = 4;

/// Maximum number of symbols that PUCCH Format 3 can transmit.
constexpr unsigned FORMAT3_MAX_NSYMB = 14;

/// Maximum number of PRBs allocated for PUCCH Format 3.
constexpr unsigned FORMAT3_MAX_NPRB = 16;

/// Minimum number of UCI payload bits carried by a PUCCH Format 3 resource.
constexpr unsigned FORMAT3_MIN_UCI_NBITS = 3;

/// Minimum number of OFDM symbols allocated to a PUCCH Format 4 transmission.
constexpr unsigned FORMAT4_MIN_NSYMB = 4;

/// Maximum number of symbols that PUCCH Format 4 can transmit.
constexpr unsigned FORMAT4_MAX_NSYMB = 14;

/// Minimum number of UCI payload bits carried by a PUCCH Format 4 resource.
constexpr unsigned FORMAT4_MIN_UCI_NBITS = 3;

/// \brief Maximum number of resource elements used by PUCCH.
///
/// It corresponds to PUCCH Format 3 with a bandwidth of 16 PRBs and a duration of 14 symbols, two of which are occupied
/// by the DM-RS.
constexpr unsigned MAX_NOF_RE = NOF_SUBCARRIERS_PER_RB * 16 * (14 - 2);

/// \brief Maximum number of LLRs corresponding to a PUCCH.
///
/// Derives from \ref PUCCH_MAX_NOF_RE assuming QPSK modulation.
constexpr unsigned MAX_NOF_LLR = MAX_NOF_RE * 2;

/// \brief Maximum effective code rate for PUCCH formats 2, 3 and 4.
///
/// Maximum value given by TS38.213 Table 9.2.5.2-1 and TS38.331 Section 6.3.2 Enumeration \e PUCCH-MaxCodeRate within
/// Information Element \e PUCCH-FormatConfig.
constexpr float MAX_CODE_RATE = 0.80F;

/// [Implementation-defined] Maximum number of PUCCH resources in a cell.
constexpr size_t MAX_NOF_CELL_PUCCH_RESOURCES = 256;

/// Maximum number of common PUCCH resources in a cell.
/// \remark See TS 38.331, section 9.2.1, maximum value is given by the number of possible values of r_PUCCH, which
/// is 16.
constexpr size_t MAX_NOF_CELL_COMMON_PUCCH_RESOURCES = 16;

/// Maximum supported UCI payload length in number of bits for PUCCH Formats 2, 3 and 4.
constexpr unsigned FORMATS_2_3_4_MAX_UCI_NBITS = 1706;

} // namespace pucch_constants
} // namespace srsran
