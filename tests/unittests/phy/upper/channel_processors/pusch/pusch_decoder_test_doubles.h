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

#include "../../../phy_test_utils.h"
#include "pusch_decoder_buffer_test_doubles.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"

namespace srsran {

class pusch_decoder_spy : public pusch_decoder
{
public:
  struct entry_t {
    span<uint8_t>            transport_block;
    pusch_decoder_result     stats;
    unique_rx_softbuffer     softbuffer;
    pusch_decoder_buffer_spy input;
    configuration            config;
    pusch_decoder_notifier*  notifier;
  };

  ~pusch_decoder_spy() { srsran_assert(entries.empty(), "Entries must be cleared."); }

  void set_codeword_length(unsigned codeword_length_) { codeword_length = codeword_length_; }

  pusch_decoder_buffer& new_data(span<uint8_t>           transport_block,
                                 unique_rx_softbuffer    softbuffer,
                                 pusch_decoder_notifier& notifier,
                                 const configuration&    cfg) override
  {
    entries.emplace_back();
    entry_t& entry        = entries.back();
    entry.transport_block = transport_block;
    entry.stats           = {};
    entry.softbuffer      = std::move(softbuffer);
    entry.input.resize(codeword_length);
    entry.notifier = &notifier;
    entry.config   = cfg;

    entry.stats.tb_crc_ok            = true;
    entry.stats.nof_codeblocks_total = 123;
    entry.stats.ldpc_decoder_stats   = {};
    entry.notifier->on_sch_data(entry.stats);

    return entry.input;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  unsigned             codeword_length;
  std::vector<entry_t> entries;
};

PHY_SPY_FACTORY_TEMPLATE(pusch_decoder);

} // namespace srsran