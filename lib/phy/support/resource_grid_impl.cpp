/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "resource_grid_impl.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

resource_grid_impl::resource_grid_impl(unsigned nof_ports_, unsigned nof_symb_, unsigned nof_subc_) :
  nof_ports(nof_ports_), nof_symb(nof_symb_), nof_subc(nof_subc_), writer(rg_buffer, empty), reader(rg_buffer, empty)
{
  // Reserve memory for the internal buffer.
  rg_buffer.reserve({nof_subc, nof_symb, nof_ports});

  // Set all the resource elements to zero.
  srsvec::zero(rg_buffer.get_data());
  empty = (1U << nof_ports) - 1;
}

void resource_grid_impl::set_all_zero()
{
  // For each non-empty port, set the underlying resource elements to zero.
  for (unsigned port = 0; port != nof_ports; ++port) {
    if (!reader.is_port_empty(port)) {
      srsvec::zero(rg_buffer.get_view<static_cast<unsigned>(resource_grid_dimensions::port)>({port}));
    }
  }
  empty = (1U << nof_ports) - 1;
}

resource_grid_writer& resource_grid_impl::get_writer()
{
  return writer;
}

const resource_grid_reader& resource_grid_impl::get_reader() const
{
  return reader;
}
