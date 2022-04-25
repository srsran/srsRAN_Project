
#ifndef SRSGNB_UPPER_PHY_RX_SOFTBUFFER_H
#define SRSGNB_UPPER_PHY_RX_SOFTBUFFER_H

#include "srsgnb/adt/span.h"

namespace srsgnb {

/// \brief Describes a receiver softbuffer.
///
/// It stores a CRC flag and LLR buffer for a number of codeblocks.
class rx_softbuffer
{
public:
  /// Default destructor.
  virtual ~rx_softbuffer() = default;

  /// Get the number of codeblocks.
  virtual unsigned get_nof_codeblocks() const = 0;

  /// Reset all codeblocks CRC.
  virtual void reset_codeblocks_crc();

  /// \brief Get all codeblocks CRC.
  /// \return An view of all the codeblocks CRC.
  /// \remark The number of codeblocks must not exceed get_nof_codeblocks().
  virtual span<bool> get_codeblocks_crc();

  /// \brief Get a codeblock LLR buffer.
  /// \param[in] codeblock_id Indicates the codeblock identifier.
  /// \param[in] codeblock_size Indicates the codeblock size.
  /// \return A view of the codeblock LLR.
  /// \remark The codeblocks identifier must not exceed get_max_nof_codeblocks()-1.
  virtual span<int8_t> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_UPPER_PHY_RX_SOFTBUFFER_H
