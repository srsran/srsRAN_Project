/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcp_tx_window.h"
#include "srsran/pdcp/pdcp_config.h"
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

pdcp_tx_xdu_info& pdcp_tx_window::operator[](uint32_t count)
{
  return tx_window[count].sdu_info;
}

void pdcp_tx_window::add_sdu(pdcp_tx_xdu_info&& sdu_info)
{
  pdcp_tx_window_element& elem = tx_window.add_sn(sdu_info.count);
  elem.sdu_info                = std::move(sdu_info);
  elem.sdu_length              = elem.sdu_info.buf.length();

  sdu_bytes += elem.sdu_length;
  nof_sdus++;
  if (rlc_mode != pdcp_rlc_mode::am) {
    // No need to store the SDU for non-AM bearer
    elem.sdu_info.buf.clear();
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
