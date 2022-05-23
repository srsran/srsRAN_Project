/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PUSCH decoder interface declaration.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_DECODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_DECODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/phy/upper/rx_softbuffer.h"
#include "srsgnb/support/stats.h"

namespace srsgnb {

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

  /// PUSCH decoding statistics.
  struct statistics {
    /// Denotes whether the received transport block passed the CRC.
    bool tb_crc_ok = false;
    /// Total number of codeblocks in the current codeword.
    unsigned nof_codeblocks_total = 0;
    /// \brief LDPC decoding statistics.
    ///
    /// Provides access to LDPC decoding statistics such as the number of decoded codeblocks (via
    /// <tt>ldpc_stats->get_nof_observations()</tt>) or the average number of iterations for correctly decoded
    /// codeblocks (via <tt>ldpc_stats->get_mean()</tt>).
    sample_statistics<unsigned> ldpc_decoder_stats = {};
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
                      statistics&                      stats,
                      rx_softbuffer*                   soft_codeword,
                      span<const log_likelihood_ratio> llrs,
                      const configuration&             cfg) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUSCH_DECODER_H
