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

#include "../../../phy_test_utils.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"

namespace srsran {

class pusch_decoder_spy : public pusch_decoder, public pusch_decoder_buffer
{
public:
  struct entry_t {
    span<uint8_t>                     transport_block;
    pusch_decoder_result              stats;
    rx_softbuffer*                    soft_codeword;
    std::vector<log_likelihood_ratio> llrs;
    configuration                     config;
    pusch_decoder_notifier*           notifier;
  };

  pusch_decoder_buffer& new_data(span<uint8_t>           transport_block,
                                 rx_softbuffer&          softbuffer,
                                 pusch_decoder_notifier& notifier,
                                 const configuration&    cfg) override
  {
    entries.emplace_back();
    entry_t& entry        = entries.back();
    entry.transport_block = transport_block;
    entry.stats           = {};
    entry.soft_codeword   = &softbuffer;
    entry.llrs            = {};
    entry.notifier        = &notifier;
    entry.config          = cfg;

    return *this;
  }

  span<log_likelihood_ratio> get_next_block_view(unsigned block_size) override
  {
    temp_llr.resize(block_size);
    return temp_llr;
  }

  void on_new_softbits(span<const log_likelihood_ratio> softbits) override
  {
    entry_t& entry = entries.back();
    entry.llrs.insert(entry.llrs.end(), softbits.begin(), softbits.end());
  }

  void on_end_softbits() override
  {
    entry_t& entry                   = entries.back();
    entry.stats.tb_crc_ok            = true;
    entry.stats.nof_codeblocks_total = 123;
    entry.stats.ldpc_decoder_stats   = {};
    entry.notifier->on_sch_data(entry.stats);
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<log_likelihood_ratio> temp_llr;
  std::vector<entry_t>              entries;
};

PHY_SPY_FACTORY_TEMPLATE(pusch_decoder);

} // namespace srsran