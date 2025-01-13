/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "pdcp_tx_window.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/support/srsran_assert.h"
#include <utility>

using namespace srsran;

pdcp_tx_window::pdcp_tx_window(pdcp_rb_type       rb_type_,
                               pdcp_rlc_mode      rlc_mode_,
                               pdcp_sn_size       sn_size_,
                               pdcp_bearer_logger logger_) :
  logger(std::move(logger_)),
  tx_window(logger, pdcp_window_size(pdcp_sn_size_to_uint(sn_size_))),
  rb_type(rb_type_),
  rlc_mode(rlc_mode_),
  sn_size(sn_size_)
{
}

bool pdcp_tx_window::has_sn(uint32_t count) const
{
  return tx_window.has_sn(count);
}

pdcp_tx_sdu_info& pdcp_tx_window::operator[](uint32_t count)
{
  return tx_window[count];
}

void pdcp_tx_window::add_sdu(uint32_t count, byte_buffer sdu, unique_timer discard_timer)
{
  pdcp_tx_sdu_info& sdu_info = tx_window.add_sn(count);
  sdu_info.count             = count;
  sdu_info.discard_timer     = std::move(discard_timer);
  sdu_info.sdu_length        = sdu.length();

  sdu_bytes += sdu_info.sdu_length;
  nof_sdus++;
  if (rlc_mode == pdcp_rlc_mode::am) {
    sdu_info.sdu = std::move(sdu);
  }
}

void pdcp_tx_window::remove_sdu(uint32_t count)
{
  if (tx_window.has_sn(count)) {
    sdu_bytes -= tx_window[count].sdu_length;
    nof_sdus--;
    tx_window.remove_sn(count);
  }
}

void pdcp_tx_window::clear()
{
  sdu_bytes = 0;
  nof_sdus  = 0;
  tx_window.clear();
}

uint32_t pdcp_tx_window::get_sdu_bytes() const
{
  return sdu_bytes;
}

uint32_t pdcp_tx_window::get_pdu_bytes(security::integrity_enabled integrity) const
{
  return sdu_bytes + nof_sdus * (pdcp_data_header_size(sn_size) +
                                 pdcp_data_trailer_size(rb_type, integrity == security::integrity_enabled::on));
}
