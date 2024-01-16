/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/uci/uci_decoder.h"
#include <random>

namespace srsran {

class uci_decoder_spy : public uci_decoder
{
public:
  struct entry_t {
    std::vector<log_likelihood_ratio> llr;
    configuration                     config;
    std::vector<uint8_t>              message;
    uci_status                        status;
  };

  uci_decoder_spy() : rgen(0), bin_dist(0, 1)
  {
    // Do nothing.
  }

  uci_status decode(span<uint8_t> message, span<const log_likelihood_ratio> llr, const configuration& config) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.config   = config;

    // Copy input soft bits.
    entry.llr.resize(llr.size());
    srsvec::copy(entry.llr, llr);

    // Generate random decoded message.
    std::generate(message.begin(), message.end(), [this]() { return rgen() & 1; });
    entry.message.resize(message.size());
    srsvec::copy(entry.message, message);

    // Generate random status.
    entry.status = (bin_dist(rgen) == 1) ? uci_status::valid : uci_status::invalid;

    return entry.status;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t>                    entries;
  std::mt19937                            rgen;
  std::uniform_int_distribution<unsigned> bin_dist;
};

PHY_SPY_FACTORY_TEMPLATE(uci_decoder);

} // namespace srsran