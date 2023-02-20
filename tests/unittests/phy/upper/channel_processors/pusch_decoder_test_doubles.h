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

#include "../../phy_test_utils.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"

namespace srsran {

class pusch_decoder_spy : public pusch_decoder
{
public:
  struct entry_t {
    span<uint8_t>                    transport_block;
    pusch_decoder_result             stats;
    rx_softbuffer*                   soft_codeword;
    span<const log_likelihood_ratio> llrs;
    configuration                    config;
  };

  void decode(span<uint8_t>                    transport_block,
              pusch_decoder_result&            stats,
              rx_softbuffer*                   soft_codeword,
              span<const log_likelihood_ratio> llrs,
              const configuration&             cfg) override
  {
    stats.tb_crc_ok            = true;
    stats.nof_codeblocks_total = 123;
    stats.ldpc_decoder_stats   = {};

    entries.emplace_back();
    entry_t& entry        = entries.back();
    entry.transport_block = transport_block;
    entry.stats           = stats;
    entry.soft_codeword   = soft_codeword;
    entry.llrs            = llrs;
    entry.config          = cfg;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

private:
  std::vector<entry_t> entries;
};

PHY_SPY_FACTORY_TEMPLATE(pusch_decoder);

} // namespace srsran