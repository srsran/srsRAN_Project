
#ifndef SRSGNB_PBCH_ENCODER_DOUBLES_H
#define SRSGNB_PBCH_ENCODER_DOUBLES_H

#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include <vector>

namespace srsgnb {

class pbch_encoder_spy : public pbch_encoder
{
private:
  struct entry_t {
    pbch_msg_t           msg;
    std::vector<uint8_t> encoded;
  };
  std::vector<entry_t> entries;

public:
  void encode(const pbch_msg_t& pbch_msg, span<uint8_t> encoded) override
  {
    // Generate new entry
    entry_t entry = {};
    entry.msg     = pbch_msg;
    entry.encoded.resize(encoded.size());

    // Generate message
    for (unsigned i = 0, len = encoded.size(); i != len; ++i) {
      encoded[i]       = static_cast<uint8_t>(i & 0xff);
      entry.encoded[i] = encoded[i];
    }

    // Append entry
    entries.emplace_back(entry);
  }

  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};
} // namespace srsgnb
#endif // SRSGNB_PBCH_ENCODER_DOUBLES_H
