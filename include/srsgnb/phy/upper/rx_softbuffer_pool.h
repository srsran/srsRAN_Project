
#ifndef SRSGNB_RX_SOFTBUFFER_POOL_H
#define SRSGNB_RX_SOFTBUFFER_POOL_H

#include "srsgnb/phy/upper/rx_softbuffer.h"
#include "srsgnb/ran/slot_point.h"
#include <cstdint>

namespace srsgnb {

/// Identifies a receive softbuffer.
struct rx_softbuffer_identifier {
  /// Indicates the UE identifier.
  uint16_t rnti;
  /// Indicates the HARQ process identifier.
  uint8_t harq_ack_id;

  /// Equal comparison to other identifier.
  bool operator==(const rx_softbuffer_identifier& other) const
  {
    return (rnti == other.rnti) && (harq_ack_id == other.harq_ack_id);
  }
};

/// Describes a receive softbuffer pool. Unique for each sector.
class rx_softbuffer_pool
{
public:
  /// Default destructor.
  virtual ~rx_softbuffer_pool() = default;

  /// \brief Reserves and gets a softbuffer for a given identifier and number of codeblocks.
  ///
  /// Reserves a softbuffer for the given identifier. It guarantees that reserved softbuffers keep the CRC and data
  /// values.
  ///
  /// The softbuffers are reserved until:
  /// - the softbuffer identifier is freed using free_softbuffer(),
  /// - a softbuffer reservation is performed with the same identifier but different number of codeblocks, or
  /// - softbuffer reservation expires (run_slot()).
  ///
  /// The pool does not initialise or modify the contents of the softbuffers. The modules that suse the softbuffers are
  /// responsible to initialise and modify their contents upon new transmissions.
  ///
  /// \param[in] slot Indicates the slot context in which the reservation occurs.
  /// \param[in] context Indicates the context for the softbuffer processing.
  /// \return A reference to the reserved softbuffer.
  virtual rx_softbuffer&
  reserve_softbuffer(const slot_point& slot, const rx_softbuffer_identifier& id, unsigned nof_codeblocks) = 0;

  /// \brief Makes available a softbuffer that was reserved with a given identifier.
  /// \remark If the indicated identifier is invalid, the call is ignored.
  /// \param[in] context Indicates the softbuffer identifier.
  virtual void free_softbuffer(const rx_softbuffer_identifier& id) = 0;

  /// \brief Runs internal state machines to expire softbuffers that are reserved and not freed.
  /// \param[in] slot Indicates the current slot.
  virtual void run_slot(const slot_point& slot) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_RX_SOFTBUFFER_POOL_H
