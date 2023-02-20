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

#include "srsgnb/phy/upper/channel_processors/uci_decoder.h"
#include <random>

namespace srsgnb {

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

private:
  std::vector<entry_t>                    entries;
  std::mt19937                            rgen;
  std::uniform_int_distribution<unsigned> bin_dist;
};

PHY_SPY_FACTORY_TEMPLATE(uci_decoder);

} // namespace srsgnb