
#ifndef SRSGNB_PBCH_ENCODER_DOUBLES_H
#define SRSGNB_PBCH_ENCODER_DOUBLES_H

#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include <vector>

namespace srsgnb {

class pbch_encoder_spy : public pbch_encoder
{
private:
  std::vector<pbch_msg_t> entries;

public:
  void     encode(const pbch_msg_t& pbch_msg, span<uint8_t> encoded) override { entries.emplace_back(pbch_msg); }
  void     reset() { entries.clear(); }
  unsigned get_nof_entries() const { return entries.size(); }
  const std::vector<pbch_msg_t> get_entries() const { return entries; }
};
} // namespace srsgnb
#endif // SRSGNB_PBCH_ENCODER_DOUBLES_H
