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

#include "srsran/phy/generic_functions/generic_functions_factories.h"
#include "srsran/support/error_handling.h"
#include <random>

namespace srsran {

class dft_processor_spy : public dft_processor
{
public:
  struct entry {
    std::vector<cf_t> input;
    std::vector<cf_t> output;
  };

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
    // Generate some random output if ouput buffer was not set.
    if (generate_random_output) {
      for (cf_t& value : output_buffer) {
        value = {dist(rgen), dist(rgen)};
      }
    }

    entries.emplace_back();
    entry& e = entries.back();
    e.input  = input_buffer;
    e.output = output_buffer;

    return output_buffer;
  }

  void clear_entries() { entries.clear(); }

  const std::vector<entry>& get_entries() const { return entries; }

  void set_output_buffer(const std::vector<cf_t>& buffer)
  {
    report_fatal_error_if_not(
        buffer.size() == size, "Buffer size {} is not equal to the DFT size {}.", buffer.size(), size);
    generate_random_output = false;
    output_buffer          = buffer;
  }

private:
  bool                                  generate_random_output = true;
  unsigned                              size;
  direction                             dir;
  std::vector<cf_t>                     input_buffer;
  std::vector<cf_t>                     output_buffer;
  std::mt19937                          rgen;
  std::uniform_real_distribution<float> dist;
  std::vector<entry>                    entries;
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

  void clear_entries() { entries.clear(); }

  const std::vector<entry>& get_entries() const { return entries; }
};

} // namespace srsran
