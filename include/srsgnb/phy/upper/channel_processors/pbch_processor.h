
#ifndef SRSGNB_SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_PROCESSOR_H_
#define SRSGNB_SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_PROCESSOR_H_

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_context.h"

namespace srsgnb {

class pbch_processor
{
public:
  /// Defines the BCH payload size
  static constexpr unsigned BCH_PAYLOAD_SIZE = 32;

  /// Describes the SS/PBCH Block PDU
  struct pdu_t {
    /// Current slot context
    slot_context_t slot_context;
    /// Physical Cell identifier
    unsigned phys_cell_id;
    /// PSS power allocation in dB, relative to SSS
    float beta_pss;
    /// SSB opportunity index in a burst
    unsigned ssb_block_index;
    /// SSB offset between the common resource grid and the beginning of the SSB in 15kHz subcarriers
    unsigned ssb_subcarrier_offset;
    /// SSB offset between the point A and the begining if the common resource grid in RB
    unsigned ssb_offset_pointA;
    /// SSB pattern case
    ssb_pattern_case pattern_case;
    /// PBCH payload, generated from the BCH-MIB packing
    std::array<uint8_t, BCH_PAYLOAD_SIZE> bch_payload;
  };

  /// Default destructor
  virtual ~pbch_processor() = default;

  /// Processes the SS/PBCH Block PDU writing in grid the generated signal
  /// \param [in] pdu provides the necessary data to
  /// \param [out] grid
  virtual void proccess(const pdu_t& pdu, resource_grid_writer& grid) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PBCH_PROCESSOR_H_
