
#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_TEST_DOUBLES_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_TEST_DOUBLES_H

#include "srsgnb/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsgnb/srsvec/copy.h"

namespace srsgnb {

class pdsch_encoder_spy : public pdsch_encoder
{
private:
  struct entry_t {
    segmenter_config     config;
    std::vector<uint8_t> transport_block;
    std::vector<uint8_t> codeword;
  };
  std::vector<entry_t> entries;

public:
  void encode(span<uint8_t> codeword, span<const uint8_t> transport_block, const segmenter_config& cfg) override
  { // Generate new entry.
    entry_t entry = {};
    entry.config  = cfg;
    entry.codeword.resize(codeword.size());

    // Fill data.
    entry.transport_block.resize(transport_block.size());
    srsvec::copy(entry.transport_block, transport_block);

    // Generate encoded message.
    for (unsigned i = 0, len = codeword.size(); i != len; ++i) {
      codeword[i]       = static_cast<uint8_t>(i & 0xff);
      entry.codeword[i] = codeword[i];
    }

    // Append entry.
    entries.emplace_back(entry);
  }

  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_TEST_DOUBLES_H
