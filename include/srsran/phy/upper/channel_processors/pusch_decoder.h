/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH decoder interface declaration.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/phy/upper/channel_processors/pusch_decoder_result.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"
#include "srsran/phy/upper/rx_softbuffer.h"
#include "srsran/support/stats.h"

namespace srsran {

/// \brief PUSCH decoder interface.
///
/// User interface of the PHY Uplink Shared Channel (PUSCH) decoder.
class pusch_decoder
{
public:
  /// PUSCH decoder configuration.
  struct configuration {
    /// Configuration of the LDPC segmenter.
    segmenter_config segmenter_cfg = {};
    /// Maximum number of iterations of the LDPC decoder.
    unsigned nof_ldpc_iterations = 6;
    /// Flag for LDPC decoder early stopping: \c true to activate.
    bool use_early_stop = true;
    /// Flag to denote new data (first HARQ transmission).
    bool new_data = true;
  };

  /// Default destructor.
  virtual ~pusch_decoder() = default;

  /// \brief Decodes a PUSCH codeword.
  ///
  /// Recovers a UL-SCH transport block from a PUSCH codeword. Reverting the encoding operations described in TS 38.212
  /// Sections 6.2.1-6.2.6, the codeword is first split into rate-matched codeblocks. Then, each codeblock is restored
  /// to its base rate, combined with previous retransmissions, and decoded. Finally, if all blocks pass the CRC check,
  /// the data bits from all codeblocks are concatenated to form the UL-SCH transport block. If applicable, a last
  /// transport-block CRC is computed and verified.
  ///
  /// \param[out]    transport_block The decoded transport block, with packed (8 bits per entry) representation.
  /// \param[out]    stats           Decoding statistics.
  /// \param[in,out] soft_codeword   A soft-buffer for combining log-likelihood ratios from different retransmissions.
  /// \param[in]     llrs            The received codeword, as a sequence of log-likelihood ratios.
  /// \param[in]     new_data        Flags new data transmissions (as opposed to retransmissions of previously failed
  ///                                transport blocks).
  /// \param[in]     blk_cfg         Transport block configuration.
  /// \param[in]     alg_cfg         LDPC decoding algorithm configuration.
  virtual void decode(span<uint8_t>                    transport_block,
                      pusch_decoder_result&            stats,
                      rx_softbuffer*                   soft_codeword,
                      span<const log_likelihood_ratio> llrs,
                      const configuration&             cfg) = 0;
};

} // namespace srsran
