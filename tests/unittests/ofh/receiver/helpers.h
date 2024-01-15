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

#include "../../phy/support/resource_grid_mapper_test_doubles.h"
#include "../../phy/support/resource_grid_test_doubles.h"
#include "srsran/phy/support/prach_buffer.h"

namespace srsran {
namespace ofh {
namespace testing {

/// PRACH buffer dummy implementation.
class prach_buffer_dummy : public prach_buffer
{
  unsigned                 nof_symbols;
  static_vector<cf_t, 839> buffer;

public:
  prach_buffer_dummy(unsigned nof_symbols_, bool long_format = true) :
    nof_symbols((nof_symbols_ == 0) ? 1 : nof_symbols_), buffer(long_format ? 839 : 139)
  {
  }

  unsigned get_max_nof_ports() const override { return 1; }

  unsigned get_max_nof_td_occasions() const override { return 1; }

  unsigned get_max_nof_fd_occasions() const override { return 1; }

  unsigned get_max_nof_symbols() const override { return nof_symbols; }

  unsigned get_sequence_length() const override { return buffer.size(); }

  span<cf_t> get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) override
  {
    return buffer;
  }

  span<const cf_t>
  get_symbol(unsigned i_port, unsigned i_td_occasion, unsigned i_fd_occasion, unsigned i_symbol) const override
  {
    return buffer;
  }
};

/// Spy implementation of the resource grid writer that returns if the functions were called.
class resource_grid_writer_bool_spy : public resource_grid_writer
{
  bool     grid_written     = false;
  unsigned nof_prbs_written = 0;

public:
  unsigned get_nof_ports() const override { return 1; };
  unsigned get_nof_subc() const override { return 51 * NRE; };
  unsigned get_nof_symbols() const override { return MAX_NSYMB_PER_SLOT; };
  void     put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override
  {
    grid_written = true;
    nof_prbs_written += symbols.size() / NRE;
  }

  span<const cf_t>
  put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override
  {
    grid_written = true;
    nof_prbs_written += symbols.size() / NRE;
    return {};
  }

  span<const cf_t> put(unsigned                            port,
                       unsigned                            l,
                       unsigned                            k_init,
                       const bounded_bitset<NRE * MAX_RB>& mask,
                       span<const cf_t>                    symbols) override
  {
    grid_written = true;
    nof_prbs_written += symbols.size() / NRE;
    return {};
  }

  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override
  {
    grid_written = true;
    nof_prbs_written += symbols.size() / NRE;
  }

  void put(unsigned port, unsigned l, unsigned k_init, unsigned stride, span<const cf_t> symbols) override
  {
    grid_written = true;
    nof_prbs_written += divide_ceil(symbols.size() * stride, NRE);
  }

  /// Returns true if the gris has been written, otherise false.
  bool has_grid_been_written() const { return grid_written; }

  /// Returns the number of PRBs written.
  unsigned get_nof_prbs_written() const { return nof_prbs_written; }
};

class resource_grid_dummy_with_spy_writer : public resource_grid
{
  resource_grid_writer_bool_spy& writer;
  resource_grid_reader_spy       reader;
  resource_grid_mapper_dummy     mapper;

public:
  explicit resource_grid_dummy_with_spy_writer(resource_grid_writer_bool_spy& writer_) :
    writer(writer_), reader(1, 14, 51)
  {
  }

  void                        set_all_zero() override {}
  resource_grid_writer&       get_writer() override { return writer; }
  const resource_grid_reader& get_reader() const override { return reader; }
  resource_grid_mapper&       get_mapper() override { return mapper; }
};

} // namespace testing
} // namespace ofh
} // namespace srsran
