/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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