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

#include "resource_grid_impl.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

resource_grid_impl::resource_grid_impl(unsigned                          nof_ports_,
                                       unsigned                          nof_symb_,
                                       unsigned                          nof_subc_,
                                       std::unique_ptr<channel_precoder> precoder_) :
  nof_ports(nof_ports_),
  nof_symb(nof_symb_),
  nof_subc(nof_subc_),
  writer(rg_buffer, empty),
  reader(rg_buffer, empty),
  mapper(nof_ports_, nof_subc_, writer, std::move(precoder_))
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
