/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

/// [Implementation-specific] Maximum Logical channels per TB.
static constexpr size_t MAX_LC_PER_TB = 4;

/// Maximum SSB opportunities per slot. This can be derived from the candidate ODFM symbols indices within the ranges
/// 0-13, 14-27, 28-41, 42-55, etc.. from TS 38.213, Section 4.1.
static constexpr size_t MAX_SSB_PER_SLOT = 2;

/// [Implementation defined] This corresponds to "Number of search space sets per slot" in Table 13-11, TS 38.213.
static constexpr size_t MAX_SIB1_PDUS_PER_SLOT = 2;

/// [Implementation defined] This corresponds to maximum number of RARs that can be scheduled per slot.
static constexpr size_t MAX_RAR_PDUS_PER_SLOT = 16;

/// [Implementation defined] This corresponds to maximum number of GRANTS that can be scheduled per RAR.
static constexpr size_t MAX_GRANTS_PER_RAR = 16;

/// [Implementation defined] This corresponds to maximum number of UEs that can be scheduled per slot.
static constexpr size_t MAX_UE_PDUS_PER_SLOT = 16;

/// [Implementation defined] Maximum number of Paging scheduled per slot.
static constexpr size_t MAX_PAGING_PDUS_PER_SLOT = 16;

/// [Implementation defined] Maximum number of CSI-RS PDUs scheduler per slot.
static constexpr size_t MAX_CSI_RS_PDUS_PER_SLOT = 2;

/// [Implementation defined] This corresponds to maximum number of PDCCH PDUs that can be scheduled per slot.
static constexpr size_t MAX_DL_PDCCH_PDUS_PER_SLOT = MAX_SIB1_PDUS_PER_SLOT + MAX_RAR_PDUS_PER_SLOT +
                                                     MAX_UE_PDUS_PER_SLOT + MAX_PAGING_PDUS_PER_SLOT +
                                                     MAX_CSI_RS_PDUS_PER_SLOT;

/// [Implementation defined] This corresponds to maximum number of PDSCH PDUs that can be scheduled per slot.
static constexpr size_t MAX_DL_PDSCH_PDUS_PER_SLOT = MAX_SIB1_PDUS_PER_SLOT + MAX_RAR_PDUS_PER_SLOT +
                                                     MAX_UE_PDUS_PER_SLOT + MAX_PAGING_PDUS_PER_SLOT +
                                                     MAX_CSI_RS_PDUS_PER_SLOT;

/// [Implementation defined] Maximum number of PRACH occasions per slot supported by the current implementation.
static constexpr size_t MAX_PRACH_OCCASIONS_PER_SLOT = 1;

/// [Implementation defined] Maximum number of preambles per time-frequency PRACH occasion supported by the current
/// implementation.
static constexpr unsigned MAX_PREAMBLES_PER_PRACH_OCCASION = 64;

/// [Implementation defined] Maximum number of codewords per PDSCH grant.
static constexpr size_t MAX_CODEWORDS_PER_PDSCH = 2;

/// [Implementation defined] Maximum number of PUSCH PDUs per slot.
static constexpr size_t MAX_PUSCH_PDUS_PER_SLOT = 16U;

/// [Implementation defined] Maximum number of UL PDCCH PDUs per slot.
static constexpr size_t MAX_UL_PDCCH_PDUS_PER_SLOT = 16U;

/// [Implementation defined] Maximum number of PUCCH PDUs per slot.
static constexpr size_t MAX_PUCCH_PDUS_PER_SLOT = 16U;

/// [Implementation defined] Maximum number of codeblocks per PDU.
static constexpr size_t MAX_CBS_PER_PDU = 64;

/// [Implementation defined] Maximum number of downlink PDUs per slot.
static constexpr size_t MAX_DL_PDUS_PER_SLOT = MAX_SSB_PER_SLOT + MAX_DL_PDCCH_PDUS_PER_SLOT + MAX_UE_PDUS_PER_SLOT +
                                               MAX_SIB1_PDUS_PER_SLOT + MAX_RAR_PDUS_PER_SLOT +
                                               MAX_PAGING_PDUS_PER_SLOT + MAX_CSI_RS_PDUS_PER_SLOT;

/// [Implementation defined] Maximum number of uplink PDUs per slot.
static constexpr size_t MAX_UL_PDUS_PER_SLOT =
    MAX_PRACH_OCCASIONS_PER_SLOT + MAX_PUCCH_PDUS_PER_SLOT + MAX_PUSCH_PDUS_PER_SLOT;

/// [Implementation defined] Maximum number of UCI PDUS per UCI indication.
static constexpr size_t MAX_UCI_PDUS_PER_UCI_IND = 8;

/// [Implementation defined] Maximum number of HARQ VALUES per HARQ PDU.
static constexpr size_t MAX_HARQ_VALUES_PER_HARQ_PDU = 8;

} // namespace srsgnb
