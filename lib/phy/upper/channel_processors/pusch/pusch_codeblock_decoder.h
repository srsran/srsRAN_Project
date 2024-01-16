/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder.h"
#include "srsran/ran/pusch/pusch_constants.h"
#include <memory>

namespace srsran {

/// \brief PUSCH code block decoder.
///
/// Reverts the rate matching, LDPC encoding and CRC check.
class pusch_codeblock_decoder
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

  /// \brief PUSCH code block decoder constructor.
  ///
  /// Sets up the internal components, namely LDPC rate dematcher, LDPC decoder and all the CRC calculators.
  ///
  /// \param[in] rdem  Pointer to an LDPC rate dematcher object.
  /// \param[in] dec   Pointer to an LDPC decoder object.
  /// \param[in] crcs  Structure with pointers to three CRC calculator objects, with generator polynomials of type \c
  ///                  CRC16, \c CRC24A and \c CRC24B.
  pusch_codeblock_decoder(std::unique_ptr<ldpc_rate_dematcher> rdem, std::unique_ptr<ldpc_decoder> dec, sch_crc& crcs) :
    dematcher(std::move(rdem)),
    decoder(std::move(dec)),
    crc_set({std::move(crcs.crc16), std::move(crcs.crc24A), std::move(crcs.crc24B)})
  {
    srsran_assert(dematcher, "Invalid dematcher.");
    srsran_assert(crc_set.crc16, "Invalid CRC16 calculator.");
    srsran_assert(crc_set.crc24A, "Invalid CRC24A calculator.");
    srsran_assert(crc_set.crc24B, "Invalid CRC24B calculator.");
    srsran_assert(crc_set.crc16->get_generator_poly() == crc_generator_poly::CRC16, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24A->get_generator_poly() == crc_generator_poly::CRC24A, "Wrong TB CRC calculator.");
    srsran_assert(crc_set.crc24B->get_generator_poly() == crc_generator_poly::CRC24B, "Wrong TB CRC calculator.");
  };

  /// Selects the CRC calculator from a CRC polynomial.
  crc_calculator* select_crc(crc_generator_poly poly)
  {
    srsran_assert((poly == crc_generator_poly::CRC16) || (poly == crc_generator_poly::CRC24A) ||
                      (poly == crc_generator_poly::CRC24B),
                  "Invalid CRC polynomial.");
    if (poly == crc_generator_poly::CRC16) {
      return crc_set.crc16.get();
    }
    if (poly == crc_generator_poly::CRC24A) {
      return crc_set.crc24A.get();
    }
    if (poly == crc_generator_poly::CRC24B) {
      return crc_set.crc24B.get();
    }
    return nullptr;
  }

  /// \brief Rate matches.
  ///
  /// Reverts the rate matching process.
  ///
  /// \param[in,out] rm_buffer Rate matching buffer.
  /// \param[in] cb_llrs       New soft bits to write in the soft buffer.
  /// \param[in] new_data      Set to true for indicating a new transmission.
  /// \param[in] metadata      Code block metadata.
  void rate_match(span<log_likelihood_ratio>       rm_buffer,
                  span<const log_likelihood_ratio> cb_llrs,
                  bool                             new_data,
                  const codeblock_metadata&        metadata);

  /// \brief Rate matches.
  ///
  /// Reverts the rate matching process and applies LDPC decoding.
  ///
  /// \param[out] cb_data            Code block data after decoding.
  /// \param[in,out] rm_buffer       Rate matching buffer.
  /// \param[in] cb_llrs             New soft bits to write in the soft buffer.
  /// \param[in] new_data            Set to true for indicating a new transmission.
  /// \param[in] crc_poly            CRC polynomial used for the code block.
  /// \param[in] use_early_stop      Set to true to allow the LDPC decoder to stop decoding when the CRC matches.
  /// \param[in] nof_ldpc_iterations Number of LDPC decoder iterations.
  /// \param[in] metadata            Code block metadata.
  /// \return The number of iterations if the CRC matches after the LDPC decoder. Otherwise, \c nullopt.
  optional<unsigned> decode(bit_buffer                       cb_data,
                            span<log_likelihood_ratio>       rm_buffer,
                            span<const log_likelihood_ratio> cb_llrs,
                            bool                             new_data,
                            srsran::crc_generator_poly       crc_poly,
                            bool                             use_early_stop,
                            unsigned                         nof_ldpc_iterations,
                            const codeblock_metadata&        metadata);

private:
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
