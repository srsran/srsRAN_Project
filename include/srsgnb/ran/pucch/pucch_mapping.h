/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/constants.h"

#pragma once

namespace srsgnb {

/// \brief Configuration of group and sequence hopping as described in TS38.331 PUCCH-ConfigCommon IE.
enum class pucch_group_hopping {
  /// Implies neither group or sequence hopping is enabled.
  NEITHER,
  /// Enables group hopping and disables sequence hopping.
  ENABLE,
  /// Disables group hopping and enables sequence hopping.
  DISABLE
};

/// \brief PUCCH Formats as described in TS38.213 Section 9.2.
enum class pucch_format { FORMAT_0, FORMAT_1, FORMAT_2, FORMAT_3, FORMAT_4, NOF_FORMATS };

/// Defines whether the PUCCH within the current slot belongs to a PUCCH repetition. For more details, refer to
/// TS38.213, Section 9.2.6.
enum class pucch_repetition_tx_slot { no_multi_slot, starts, continues, ends };

/// PUCCH does not make use spatial multiplexing.
static constexpr unsigned PUCCH_MAX_LAYERS = 1;

/// Maximum number of symbols (without DM-RS) that NR-PUCCH Format 1 can transmit.
static constexpr unsigned PUCCH_FORMAT1_N_MAX = 7;

/// Maximum number of HARQ-ACK carried by a PUCCH format 1 resource.
static constexpr unsigned PUCCH_FORMAT1_MAX_HARQ_ACK = 2;

/// Minimum number of symbols (including DM-RS) that NR-PUCCH Format 1 can transmit.
static constexpr unsigned PUCCH_FORMAT1_MIN_NSYMB = 4;

/// Maximum number of symbols (including DM-RS) that NR-PUCCH Format 1 can transmit.
static constexpr unsigned PUCCH_FORMAT1_MAX_NSYMB = 14;

/// Minimum number of symbols that NR-PUCCH Format 2 can transmit.
static constexpr unsigned PUCCH_FORMAT2_MIN_NSYMB = 1;

/// Maximum number of symbols that NR-PUCCH Format 2 can transmit.
static constexpr unsigned PUCCH_FORMAT2_MAX_NSYMB = 2;

/// Maximum number of PRBs allocated for NR-PUCCH Format 2.
static constexpr unsigned PUCCH_FORMAT2_MAX_NPRB = 16;

/// Minimum number of UCI payload bits carried by a PUCCH Format 2 resource.
static constexpr unsigned PUCCH_FORMAT2_MIN_UCI_NBITS = 3;

/// Number of control data subcarriers per Resource Block for PUCCH Format 2, as per TS38.213 Section 9.1.5.2.
static constexpr unsigned PUCCH_FORMAT2_NOF_DATA_SC = 8;

/// \brief Maximum number of resource elements used by PUCCH.
///
/// It corresponds to PUCCH Format 3 with a bandwidth of 16 PRBs and a duration of 14 symbols, two of which are occupied
/// by the DM-RS.
static constexpr unsigned PUCCH_MAX_NOF_RE = NRE * 16 * (14 - 2);

/// \brief Maximum number of LLRs corresponding to a PUCCH.
///
/// Derives from \ref PUCCH_MAX_NOF_RE assuming QPSK modulation.
static constexpr unsigned PUCCH_MAX_NOF_LLR = PUCCH_MAX_NOF_RE * 2;

} // namespace srsgnb
