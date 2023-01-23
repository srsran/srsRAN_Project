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

#include "srsgnb/adt/bit_buffer.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"

namespace srsgnb {

/// \brief Describes a receiver softbuffer.
///
/// The aim is to provide the physical layer shared channel FEC with a persistent storage of CRC and soft bits buffer
/// among transmissions for a given user and HARQ process.
///
/// The CRCs are set to \c true if the codeblocks have been successfully decoded.  By looking at these flags, codeblocks
/// decoded on a prior transmission do not require decoding.
///
/// It is the responsibility of the shared channel receiver to reset CRCs and set to zero the soft bits when new data is
/// indicated.
class rx_softbuffer
{
public:
  /// Default destructor.
  virtual ~rx_softbuffer() = default;

  /// Gets the number of codeblocks.
  virtual unsigned get_nof_codeblocks() const = 0;

  /// Resets all codeblocks CRCs.
  virtual void reset_codeblocks_crc() = 0;

  /// \brief Gets all codeblocks CRCs.
  /// \return A view of the codeblocks CRCs: each entry represents the CRC state of a codeblock.
  virtual span<bool> get_codeblocks_crc() = 0;

  /// \brief Gets a codeblock soft-bit buffer.
  /// \param[in] codeblock_id Indicates the codeblock identifier.
  /// \param[in] codeblock_size Indicates the codeblock size.
  /// \return A view of the codeblock soft bits.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual span<log_likelihood_ratio> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) = 0;

  /// \brief Gets a codeblock data-bit buffer.
  /// \param[in] codeblock_id Indicates the codeblock identifier.
  /// \param[in] data_size Indicates the data size, i.e. the number of data/information bits in the codeblock.
  /// \return A view of the codeblock data bits (the information message).
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  /// \note Data bits do not include filler bits, codeblock-specific CRC bits or zero-padding bits.
  virtual bit_buffer get_codeblock_data_bits(unsigned codeblock_id, unsigned data_size) = 0;
};

} // namespace srsgnb
