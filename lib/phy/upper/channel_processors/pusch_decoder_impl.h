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
#include "srsran/phy/upper/channel_processors/pusch_decoder.h"

namespace srsran {

/// Implementation of the PUSCH decoder.
class pusch_decoder_impl : public pusch_decoder
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
    crc_set({std::move(crcs.crc16), std::move(crcs.crc24A), std::move(crcs.crc24B)})
  {
    srsran_assert(crc_set.crc16, "Invalid CRC16 calculator.");
    srsran_assert(crc_set.crc24A, "Invalid CRC24A calculator.");
    srsran_assert(crc_set.crc24B, "Invalid CRC24B calculator.");
    srsran_assert(crc_set.crc16->get_generator_poly() == crc_generator_poly::CRC16, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24A->get_generator_poly() == crc_generator_poly::CRC24A, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24B->get_generator_poly() == crc_generator_poly::CRC24B, "Wrong TB CRC calculator.");
  };

  // See interface for the documentation.
  void decode(span<uint8_t>                    transport_block,
              pusch_decoder_result&            stats,
              rx_softbuffer*                   soft_codeword,
              span<const log_likelihood_ratio> llrs,
              const configuration&             cfg) override;

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
};

} // namespace srsran
