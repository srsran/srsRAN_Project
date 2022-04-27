
#ifndef SRSGNB_UPPER_PHY_RX_SOFTBUFFER_H
#define SRSGNB_UPPER_PHY_RX_SOFTBUFFER_H

#include "srsgnb/adt/span.h"

namespace srsgnb {

/// \brief Describes a receiver softbuffer.
///
/// The aim of this interface is providing to the physical layer shared channel FEC with a persistent storage of CRC and
/// soft-bits buffer among transmissions for a given user and HARQ process.
///
/// The CRCs indicate true if the codeblocks have been successfully decoded. This intends that codeblocks decoded on a
/// prior transmission do not require decoding.
///
/// It is responsibility of the shared channel receiver to reset CRCs and set to zero the soft-bits when new data is
/// indicated.
class rx_softbuffer
{
public:
  /// Default destructor.
  virtual ~rx_softbuffer() = default;

  /// Get the number of codeblocks.
  virtual unsigned get_nof_codeblocks() const = 0;

  /// Reset all codeblocks CRC.
  virtual void reset_codeblocks_crc() = 0;

  /// \brief Get all codeblocks CRCs.
  /// \return A view of the codeblocks CRCs. Where each entry represents the CRC state of a codeblock.
  virtual span<bool> get_codeblocks_crc() = 0;

  /// \brief Get a codeblock soft-bit buffer.
  /// \param[in] codeblock_id Indicates the codeblock identifier.
  /// \param[in] codeblock_size Indicates the codeblock size.
  /// \return A view of the codeblock soft-bits.
  /// \remark The codeblock identifier must not exceed get_max_nof_codeblocks()-1.
  virtual span<int8_t> get_codeblock_soft_bits(unsigned codeblock_id, unsigned codeblock_size) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_UPPER_PHY_RX_SOFTBUFFER_H
