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

#include "srsran/adt/span.h"
#include "srsran/phy/upper/log_likelihood_ratio.h"

namespace srsran {

/// \brief Describes a transmitter buffer.
///
/// The aim is to provide the physical layer shared channel FEC with a persistent storage of the rate matcher buffer
/// among transmissions for a given user and HARQ process.
///
class tx_buffer
{
public:
  /// Default destructor.
  virtual ~tx_buffer() = default;

  /// Gets the number of codeblocks.
  virtual unsigned get_nof_codeblocks() const = 0;

  /// \brief Gets the codeblock absolute identifier.
  ///
  /// The absolute codeblock identifier refers to the codeblock identifier within the common codeblock memory pool. It
  /// is available for decoders which use external memory.
  ///
  /// \param[in] codeblock_id Codeblock identifier.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual unsigned get_absolute_codeblock_id(unsigned codeblock_id) const = 0;

  /// \brief Gets an encoded read-write codeblock.
  /// \param[in] codeblock_id   Codeblock identifier.
  /// \param[in] codeblock_size Codeblock size.
  /// \return A view of the codeblock bits.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual bit_buffer get_codeblock(unsigned codeblock_id, unsigned codeblock_size) = 0;
};

} // namespace srsran
