/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/ran/prach/prach_preamble_information.h"
#include <random>

namespace srsgnb {

class prach_generator_spy : public prach_generator
{
public:
  struct generate_entry {
    configuration     config;
    std::vector<cf_t> result;
  };

  span<const cf_t> generate(const configuration& config) override
  {
    // Extract sequence length.
    unsigned sequence_length = get_prach_preamble_long_info(config.format).sequence_length;

    // Create entry.
    entries.emplace_back();
    generate_entry& entry = entries.back();
    entry.config          = config;
    entry.result.resize(sequence_length);

    // Generates the random result.
    std::generate(entry.result.begin(), entry.result.end(), [&, n = 0]() mutable {
      return std::exp(COMPLEX_J * static_cast<float>((n++) * config.preamble_index) /
                      static_cast<float>(sequence_length));
    });

    return entry.result;
  }

  const std::vector<generate_entry>& get_entries() const { return entries; }

  void clear_entries() { entries.clear(); }

private:
  std::vector<generate_entry> entries;
};

class prach_generator_factory_spy : public prach_generator_factory
{
public:
  std::unique_ptr<prach_generator> create() override
  {
    std::unique_ptr<prach_generator_spy> generator = std::make_unique<prach_generator_spy>();
    entries.emplace_back(generator.get());
    return generator;
  }

  const std::vector<prach_generator_spy*>& get_entries() const { return entries; }

  void clear_entries() { entries.clear(); }

private:
  std::vector<prach_generator_spy*> entries;
};

} // namespace srsgnb