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

#pragma once

#include "pdcp_bearer_logger.h"
#include "pdcp_tx_buf_info.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/security/security.h"
#include "srsran/support/sdu_window.h"

namespace srsran {

struct pdcp_tx_window_element {
  /// The actual SDU with additional meta info
  pdcp_tx_buf_info sdu_info;
  /// Holds the SDU length even in case no SDU is stored (e.g. UM bearer or discard timer off). Required for tracking
  /// the total number of bytes in TX window to consider the desired buffer size from lower layers.
  uint32_t sdu_length;
};

/// Wrapper to PDCP tx window.
/// It provides interfaces for the PDCP bearers, for the higher and lower layers
class pdcp_tx_window
{
public:
  pdcp_tx_window(pdcp_rb_type rb_type_, pdcp_rlc_mode rlc_mode_, pdcp_sn_size sn_size_, pdcp_bearer_logger logger_);

  bool has_sn(uint32_t count) const;

  /// \brief Add SDU info to TX window
  ///
  /// This method adds the SDU info to TX window. For AM, the SDU info shall also include a deep copy of the SDU buffer
  /// for a possible data recovery procedure. Otherwise the SDU buffer may be empty. In any case, the SDU length must be
  /// provided explicitly to track the total number of bytes in flight.
  ///
  /// \param sdu_info The SDU info that shall be stored in the TX window.
  /// \param sdu_length The length of the SDU (regardless of the presence of a SDU buffer copy in the SDU info).
  void add_sdu(pdcp_tx_buf_info&& sdu_info, uint32_t sdu_length);

  /// \brief Remove SDU from TX window
  /// This method removes an SDU from the TX window. It will keep track of the PDU bytes in the window.
  ///
  /// \return true if the SDU exists in the window and was removed, false otherwise.
  void remove_sdu(uint32_t count);

  pdcp_tx_buf_info& operator[](uint32_t count);

  void clear();

  uint32_t get_nof_sdus() const;

  uint32_t get_sdu_bytes() const;

  uint32_t get_pdu_bytes(security::integrity_enabled integrity) const;

private:
  pdcp_bearer_logger logger;

  /// \brief Tx window.
  /// This container is used to store discard timers of transmitted SDUs and, only for AM, a copy of the SDU for data
  /// recovery procedure. Upon expiry of a discard timer, the PDCP Tx entity instructs the lower layers to discard the
  /// associated PDCP PDU. See section 5.2.1 and 7.3 of TS 38.323.
  sdu_window<pdcp_tx_window_element, pdcp_bearer_logger> tx_window;

  /// Creates the tx_window according to sn_size
  /// \param sn_size Size of the sequence number (SN)
  /// \return unique pointer to tx_window instance
  void create_tx_window();

  pdcp_rb_type  rb_type;
  pdcp_rlc_mode rlc_mode;
  pdcp_sn_size  sn_size;

  uint32_t sdu_bytes = 0;
  uint32_t nof_sdus  = 0;
};

} // namespace srsran
