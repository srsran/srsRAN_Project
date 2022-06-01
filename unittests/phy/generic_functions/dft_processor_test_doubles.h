/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef UNITTEST_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_TEST_DOUBLES_H
#define UNITTEST_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_TEST_DOUBLES_H

#include "srsgnb/phy/generic_functions/generic_functions_factories.h"
#include <random>

namespace srsgnb {

class dft_processor_spy : public dft_processor
{
private:
  struct entry {
    std::vector<cf_t> input;
    std::vector<cf_t> output;
  };

  unsigned                              size;
  direction                             dir;
  std::vector<cf_t>                     input_buffer;
  std::vector<cf_t>                     output_buffer;
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  std::vector<entry>                    entries;

public:
  dft_processor_spy(unsigned seed, const configuration& config) :
    size(config.size), dir(config.dir), input_buffer(config.size), output_buffer(config.size), rgen(seed), dist(-1, 1)
  {
    // Do nothing.
  }
  direction        get_direction() const override { return dir; }
  unsigned         get_size() const override { return size; }
  span<cf_t>       get_input() override { return input_buffer; }
  span<const cf_t> run() override
  {
    // Generate some random output.
    for (cf_t& value : output_buffer) {
      value = {dist(rgen), dist(rgen)};
    }

    entries.emplace_back();
    entry& e = entries.back();
    e.input  = input_buffer;
    e.output = output_buffer;

    return output_buffer;
  }
  void reset() { entries.clear(); }

  const std::vector<entry>& get_entries() const { return entries; }
};

class dft_processor_factory_spy : public dft_processor_factory
{
private:
  struct entry {
    dft_processor_spy* dft;
    unsigned           seed;
  };

  unsigned seed = 1234;

  std::vector<entry> entries;

public:
  std::unique_ptr<dft_processor> create(const dft_processor::configuration& config) override
  {
    entries.emplace_back();
    entry& e = entries.back();
    e.dft    = new dft_processor_spy(seed, config);
    e.seed   = seed;

    // Increment seed.
    ++seed;

    return std::unique_ptr<dft_processor_spy>(e.dft);
  }

  void reset() { entries.clear(); }

  const std::vector<entry>& get_entries() const { return entries; }
};

} // namespace srsgnb

#endif // UNITTEST_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_TEST_DOUBLES_H
