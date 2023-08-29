/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/ran/pdsch/pdsch_constants.h"

namespace srsran {

/// Implementation of the PUSCH decoder.
class pusch_decoder_impl : public pusch_decoder, private pusch_decoder_buffer
{
public:
  /// CRC calculators used in shared channels.
  struct sch_crc {
    /// For short TB checksums.
    std::unique_ptr<crc_calculator> crc16;
    /// For long TB checksums.
    std::unique_ptr<crc_calculator> crc24A;
    /// For segment-specific checksums.
    std::unique_ptr<crc_calculator> crc24B;
  };

  /// \brief PUSCH decoder constructor.
  ///
  /// Sets up the internal components, namely LDPC segmenter, LDPC rate dematcher, LDPC decoder and all the CRC
  /// calculators.
  ///
  /// \param[in] seg   Pointer to an LDPC segmenter object.
  /// \param[in] rdem  Pointer to an LDPC rate dematcher object.
  /// \param[in] dec   Pointer to an LDPC decoder object.
  /// \param[in] crcs  Structure with pointers to three CRC calculator objects, with generator polynomials of type \c
  ///                  CRC16, \c CRC24A and \c CRC24B.
  pusch_decoder_impl(std::unique_ptr<ldpc_segmenter_rx>   seg,
                     std::unique_ptr<ldpc_rate_dematcher> rdem,
                     std::unique_ptr<ldpc_decoder>        dec,
                     sch_crc                              crcs) :
    segmenter(std::move(seg)),
    dematcher(std::move(rdem)),
    decoder(std::move(dec)),
    crc_set({std::move(crcs.crc16), std::move(crcs.crc24A), std::move(crcs.crc24B)}),
    softbits_buffer(pdsch_constants::CODEWORD_MAX_SIZE.value())
  {
    srsran_assert(segmenter, "Invalid segmenter.");
    srsran_assert(dematcher, "Invalid dematcher.");
    srsran_assert(crc_set.crc16, "Invalid CRC16 calculator.");
    srsran_assert(crc_set.crc24A, "Invalid CRC24A calculator.");
    srsran_assert(crc_set.crc24B, "Invalid CRC24B calculator.");
    srsran_assert(crc_set.crc16->get_generator_poly() == crc_generator_poly::CRC16, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24A->get_generator_poly() == crc_generator_poly::CRC24A, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24B->get_generator_poly() == crc_generator_poly::CRC24B, "Wrong TB CRC calculator.");
  };

  // See interface for the documentation.
  pusch_decoder_buffer& new_data(span<uint8_t>           transport_block,
                                 rx_softbuffer&          softbuffer,
                                 pusch_decoder_notifier& notifier,
                                 const configuration&    cfg) override;

private:
  /// Pointer to an LDPC segmenter.
  std::unique_ptr<ldpc_segmenter_rx> segmenter;
  /// Pointer to an LDPC rate-dematcher.
  std::unique_ptr<ldpc_rate_dematcher> dematcher;
  /// Pointer to an LDPC decoder.
  std::unique_ptr<ldpc_decoder> decoder;
  /// \brief Pointer to a CRC calculator for TB-wise checksum.
  ///
  /// Only the CRC calculator with generator polynomial crc_generator_poly::CRC24A, used for long transport blocks, is
  /// needed. Indeed, if a transport block is short enough not to be segmented, the CRC is verified by the decoder.
  sch_crc crc_set;
  /// Soft bit buffer.
  std::vector<log_likelihood_ratio> softbits_buffer;
  /// Counts the number of soft bits in the buffer.
  unsigned softbits_count;
  /// Current transport block.
  span<uint8_t> transport_block;
  /// Current soft bits buffer.
  rx_softbuffer* soft_codeword;
  /// Current notifier.
  pusch_decoder_notifier* result_notifier;
  /// Current PUSCH decoder configuration.
  pusch_decoder::configuration current_config;

  // See interface for the documentation.
  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override;

  // See interface for the documentation.
  void on_new_softbits(span<const log_likelihood_ratio> softbits) override;

  // See interface for the documentation.
  void on_end_softbits() override;
};

} // namespace srsran
