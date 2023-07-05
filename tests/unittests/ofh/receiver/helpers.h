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

#include "../../phy/support/resource_grid_mapper_test_doubles.h"

namespace srsran {
namespace ofh {
namespace testing {

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
